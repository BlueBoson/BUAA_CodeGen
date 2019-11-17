#include "MipsGen.h"

MipsGen* MipsGen::instance = nullptr;

MipsGen& MipsGen::getInstance(std::vector<MidCode>& mid, std::ofstream& obj, SymbolTable& table) {
	if (!instance) {
		instance = new MipsGen(mid, obj, table);
	}
	return *instance;
}

void MipsGen::deleteInstance() {
	delete instance;
	instance = nullptr;
}

MipsGen::MipsGen(std::vector<MidCode>& mid, std::ofstream& obj, SymbolTable& table) : 
	mid(mid), obj(obj), table(table), srDepth(0), trDepth(0) {
	regs = regVec();
	globals = table.getGlobals();
}

std::vector<RegInfo> regVec() {
	std::vector<RegInfo> vec;
	vec.push_back({ "$zero", "#ZERO", true });
	vec.push_back({ "$at", "", true });
	vec.push_back({ "$v0", "#RET", true });
	vec.push_back({ "$v1", "", true });
	for (int i = 0; i < 4; ++i) {
		vec.push_back({ "$a" + std::to_string(i), "", false });
	}
	for (int i = 0; i < 8; ++i) {
		vec.push_back({ "$t" + std::to_string(i), "", false });
	}
	for (int i = 0; i < 8; ++i) {
		vec.push_back({ "$s" + std::to_string(i), "", false });
	}
	for (int i = 8; i < 10; ++i) {
		vec.push_back({ "$t" + std::to_string(i), "", false });
	}
	vec.push_back({ "$k0", "", true });
	vec.push_back({ "$k1", "", true });
	vec.push_back({ "$gp", "", true });
	vec.push_back({ "$sp", "", true });
	vec.push_back({ "$fp", "", true });
	vec.push_back({ "$ra", "#ra", true });
}

void MipsGen::gen() {
	if (mid.empty()) {
		return;
	}
	iter = mid.begin();
	global();
	obj << ".globl " <<FUNC_PREFIX << S_MAIN << std::endl;
	obj << ".text" << std::endl;
	while (iter != mid.end()) {
		func();
	}
}

void MipsGen::global() {
	using namespace std;
	obj << ".data " << endl;
	while (iter != mid.end() && iter->getType() == MidType::CONST) {
		if (iter->getOp1() == S_INT) {
			obj << GLOBAL_PREFIX << iter->getResOp() + ": .word " << iter->getOp2() << endl;
		} else if (iter->getOp1() == S_CHAR) {
			obj << GLOBAL_PREFIX << iter->getResOp() + ": .byte " << iter->getOp2() << endl;
			obj << ".align 2" << endl;
		} else {
			cerr << "illegal const type" << endl;
		}
		++iter;
	}
	while (iter != mid.end() && iter->getType() == MidType::VAR) {
		if (iter->getOp1() == S_INT) {
			if (iter->getOp2().empty()) {
				obj << GLOBAL_PREFIX << iter->getResOp() + ": .word 0" << endl;
			} else {
				obj << GLOBAL_PREFIX << iter->getResOp() + ": .word 0:" << iter->getOp2() << endl;
			}
		} else if (iter->getOp1() == S_CHAR) {
			if (iter->getOp2().empty()) {
				obj << GLOBAL_PREFIX << iter->getResOp() + ": .byte 0" << endl;
			} else {
				obj << GLOBAL_PREFIX << iter->getResOp() + ": .byte 0:" << iter->getOp2() << endl;
			}
			obj << ".align 2" << endl;
		} else {
			cerr << "illegal const type" << endl;
		}
		++iter;
	}
	for (auto str : table.getStrs()) {
		obj << str.second << ": .asciiz \"" << str.first << "\"" << endl;
	}
	obj << "_srt_" << STR_LB << ": .asciiz \"\\n\"" << endl;
}

void MipsGen::func(){
	using namespace std;
	if (iter->getType() != MidType::FUNC) {
		cerr << "expected init func" << endl;
	} else {
		obj << FUNC_PREFIX << iter->getOp2() << ":" << endl;
		++iter;
		if (iter->getOp2() == S_MAIN) {
			funcBody(true);
			obj << "li $v0, 10" << endl;
			obj << "syscall" << endl;
			++iter;
		} else {
			enterFunc();
			funcBody(false);
			quitFunc();
		}
	}
}

void MipsGen::enterFunc() {
	using namespace std;
	int len = sizeof(R_SS) / sizeof(R_SS[0]);
	for (int i = 0; i < len; ++i) {
		genCode(MipsCode::iOp(MipsType::SW, R_SS[i], R_SP, -4 * i));
	}
	genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, -4 * len));
	for (int i = 0; iter != mid.end() && iter->getType() == MidType::PARAM; ++iter, ++i) {
		std::string paraName = iter->getResOp();
		int sr = allocSr(paraName);
		ArgType atype = iter->getOp1() == S_CHAR ? ArgType::CHAR : ArgType::INT;
		vars[paraName] = { true, true, sr, -4 * (i + 1), atype, RunVarType::LOCAL};
		if (i < 4) {
			genCode(MipsCode::iOp(MipsType::ADDIU, sr, R_AS[i], "0"));
			store(sr, R_SP, 4 * (i + 1), atype);
		} else {
			load(sr, R_SP, 4 * (i + 1), atype);
		}
	}
	while (iter != mid.end() && iter->getType() == MidType::CONST) {
		std::string paraName = iter->getResOp();
		int sr = allocSr(paraName);
		ArgType atype = iter->getOp1() == S_CHAR ? ArgType::CHAR : ArgType::INT;
		vars[paraName] = { true, true, sr, srDepth, atype, RunVarType::LOCAL };
		genCode(MipsCode::iOp(MipsType::LI, sr, 0, iter->getOp2()));
		store(sr, R_SP, 0, atype);
		genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, -4));
		srDepth += 4;
		++iter;
	}
	int old = srDepth;
	while (iter != mid.end() && iter->getType() == MidType::VAR) {
		std::string paraName = iter->getResOp();
		int sr = allocSr(paraName);
		ArgType atype = iter->getOp1() == S_CHAR ? ArgType::CHAR : ArgType::INT;
		vars[paraName] = { false, true, sr, srDepth, atype, RunVarType::LOCAL };
		if (iter->getOp2().empty()) {
			srDepth += 4;
		} else {
			srDepth += 4 * stoi(iter->getOp2());
		}
		++iter;
	}
	if (srDepth > old) {
		genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, old - srDepth));
	}
}

void MipsGen::funcBody(bool main) {
	using namespace std;
	vector<MidCode> params;
	for (MidType type = iter->getType(); iter != mid.end() && type != MidType::END_FUNC; ++iter) {
		switch (type) {
		case MidType::LABEL:
			popTr();
			obj << LABEL_PREFIX << iter->getResOp() << ":" << endl;
			freshSr();
			break;
		case MidType::JMP:
			popTr();
			genCode(MipsCode::jOp(MipsType::J, iter->getResOp()));
			freshSr();
			break;
		case MidType::PUSH:
			params.push_back(*iter);
			break;
		case MidType::CALL:
			genCode(MipsCode::iOp(MipsType::SW, R_RA, R_SP, 0));
			genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, - 4 - 4 * params.size()));
			for (int i = 0; i < params.size(); ++i) {
				if (i < 4) {
					genCode(MipsCode::iOp(MipsType::ADDIU, R_AS[i], use(params[i].getResOp()), 0));
				} else {
					genCode(MipsCode::iOp(MipsType::SW, use(params[i].getResOp()), R_SP, 4 * (i + 1)));
				}
			}
			genCode(MipsCode::jOp(MipsType::JAL, FUNC_PREFIX + iter->getResOp()));
			genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, 4 + 4 * params.size()));
			genCode(MipsCode::iOp(MipsType::LW, R_RA, R_SP, 0));
			popTr();
			params.clear();
			break;
		case MidType::READ:
			if (iter->getOp1() == S_INT) {
				genCode(MipsCode::iOp(MipsType::LI, R_V0, 0, SYS_READ_INT));
			} else {
				genCode(MipsCode::iOp(MipsType::LI, R_V0, 0, SYS_READ_CHR));
			}
			genCode(MipsCode::syscall());
			genCode(MipsCode::iOp(MipsType::ADDIU, change(iter->getResOp()), R_AS[0], 0));
			wb(iter->getResOp());
			break;
		case MidType::RETURN:
			if (!iter->getResOp().empty()) {
				genCode(MipsCode::iOp(MipsType::ADDIU, R_V0, use(iter->getResOp()), 0));
			}
			popTr();
			if (main) {
				genCode(MipsCode::iOp(MipsType::LI, R_V0, 0, SYS_EXIT));
				genCode(MipsCode::syscall());
			} else if (iter + 1 != mid.end() && (iter + 1)->getType() != MidType::END_FUNC) {
				quitFunc();
			}
			break;
		case MidType::WRITE:
			if (!iter->getOp1().empty()) {
				genCode(MipsCode::iOp(MipsType::LA, R_AS[0], 0, table.getStr(iter->getOp1())));
				genCode(MipsCode::iOp(MipsType::LI, R_V0, 0, SYS_PRT_STR));
				genCode(MipsCode::syscall());
			}
			if (!iter->getOp2().empty()) {
				genCode(MipsCode::iOp(MipsType::ADDIU, R_AS[0], use(iter->getOp2()), 0));
				if (iter->getResOp() == S_INT) {
					genCode(MipsCode::iOp(MipsType::LI, R_V0, 0, SYS_PRT_INT));
				} else {
					genCode(MipsCode::iOp(MipsType::LI, R_V0, 0, SYS_PRT_CHR));
				}
				genCode(MipsCode::syscall());
			}
			genCode(MipsCode::iOp(MipsType::LA, R_AS[0], 0, STR_LB));
			genCode(MipsCode::iOp(MipsType::LI, R_V0, 0, SYS_PRT_STR));
			genCode(MipsCode::syscall());
			break;
		case MidType::BEQ:
		case MidType::BNE:
		{
			int rl = use(iter->getOp1());
			int rr = use(iter->getOp2());
			popTr();
			MipsType objType = (type == MidType::BEQ) ? MipsType::BEQ : MipsType::BNE;
			genCode(MipsCode::iOp(objType, rl, rr, LABEL_PREFIX + iter->getResOp()));
			freshSr();
		}
			break;
		case MidType::BLTZ:
		case MidType::BLEZ:
		case MidType::BGTZ:
		case MidType::BGEZ:
		{
			int r = use(iter->getOp1());
			popTr();
			MipsType objType = (type == MidType::BLTZ) ? MipsType::BLTZ :
				(type == MidType::BLEZ) ? MipsType::BLEZ :
				(type == MidType::BGTZ) ? MipsType::BGTZ : MipsType::BGEZ;
			genCode(MipsCode::iOp(objType, r, 0, LABEL_PREFIX + iter->getResOp()));
			freshSr();
		}
			break;
		case MidType::ASSIGN:
		{
			char head = iter->getOp1()[0];
			if (isdigit(head) || head == C_PLUS || head == C_MINU || head == C_SQUOTE) {
				genCode(MipsCode::iOp(MipsType::LI, change(iter->getResOp()), 0, iter->getOp1()));
			} else {
				int rr = use(iter->getOp1());
				genCode(MipsCode::iOp(MipsType::ADDIU, change(iter->getResOp()), rr, 0));
			}
			wb(iter->getResOp());
		}
			break;
		case MidType::LARRAY_ASSIGN:
		{
			int rr = use(iter->getOp2());
			MipsType objType = (globals[iter->getResOp()].type == IdenType::VAR_INT) ? MipsType::SW : MipsType::SB;
			if (vars.find(iter->getResOp()) == vars.end()) {
				genCode(MipsCode::iOp(objType, rr, use(iter->getOp1()), GLOBAL_PREFIX + iter->getResOp()));
			} else {
				int rindex = use(iter->getOp1());
				int offset = trDepth + srDepth - vars[iter->getResOp()].stackPos;
				genCode(MipsCode::rOp(MipsType::SUBU, R_V1, R_SP, rindex));
				genCode(MipsCode::iOp(objType, rr, R_V1, offset));
			}
		}
			break;
		case MidType::RARRAY_ASSIGN:
		{
			int rr = use(iter->getOp2());
			MipsType objType = (globals[iter->getResOp()].type == IdenType::VAR_INT) ? MipsType::LW : MipsType::LB;
			if (vars.find(iter->getOp1()) == vars.end()) {
				genCode(MipsCode::iOp(objType, change(iter->getResOp()), rr, GLOBAL_PREFIX + iter->getResOp()));
				wb(iter->getResOp());
			} else {
				int rchange = change(iter->getOp1());
				int offset = trDepth + srDepth - vars[iter->getResOp()].stackPos;
				genCode(MipsCode::rOp(MipsType::SUBU, R_V1, R_SP, rr));
				genCode(MipsCode::iOp(objType, rchange, R_V1, offset));
			}

		}
			break;
		case MidType::MINU:
		case MidType::PLUS:
		case MidType::MUL:
		case MidType::DIV:
		{
			MipsType objType = (type == MidType::MINU) ? MipsType::SUBU :
				(type == MidType::PLUS) ? MipsType::ADDU :
				(type == MidType::MUL) ? MipsType::MUL : MipsType::DIV;
			int rl = use(iter->getOp1());
			int rr = use(iter->getOp2());
			genCode(MipsCode::rOp(objType, change(iter->getResOp()), rl, rr));
			wb(iter->getResOp());
		}
			break;
		default:
			cerr << "unexpected midtype" << endl;
			++iter;
			break;
		}
	}
}