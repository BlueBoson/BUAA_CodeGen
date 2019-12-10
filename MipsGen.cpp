#include "MipsGen.h"

std::vector<RegInfo> regVec() {
	std::vector<RegInfo> vec;
	vec.push_back({ ZERO_TV, true });
	vec.push_back({ "", true });
	vec.push_back({ RET_TV, true });
	vec.push_back({ "", true });
	for (int i = 0; i < 4; ++i) {
		vec.push_back({ "", false });
	}
	for (int i = 0; i < 8; ++i) {
		vec.push_back({ "", false });
	}
	for (int i = 0; i < 8; ++i) {
		vec.push_back({ "", false });
	}
	for (int i = 8; i < 10; ++i) {
		vec.push_back({ "", false });
	}
	vec.push_back({ "", true });
	vec.push_back({ "", true });
	vec.push_back({ "", true });
	vec.push_back({ "", true });
	vec.push_back({ "", true });
	vec.push_back({ "", true });
	return vec;
}

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
	mid(mid), obj(obj), table(table), srDepth(0), trDepth(0), srCount(0), trCount(0) {
	regs = regVec();
	globals = table.getGlobals();
}

void MipsGen::genCode(MipsCode code) {
	obj << code.toString() << std::endl;
}

void MipsGen::load(int rt, int rs, std::string iorl, ArgType atype) {
	MipsType type = (atype == ArgType::INT) ? MipsType::LW : MipsType::LB;
	genCode(MipsCode::iOp(type, rt, rs, iorl));
}

void MipsGen::store(int rt, int rs, std::string iorl, ArgType atype) {
	MipsType type = (atype == ArgType::INT) ? MipsType::SW : MipsType::SB;
	genCode(MipsCode::iOp(type, rt, rs, iorl));
}

void MipsGen::load(int rt, int rs, int iorl, ArgType atype) {
	MipsType type = (atype == ArgType::INT) ? MipsType::LW : MipsType::LB;
	genCode(MipsCode::iOp(type, rt, rs, iorl));
}

void MipsGen::store(int rt, int rs, int iorl, ArgType atype) {
	MipsType type = (atype == ArgType::INT) ? MipsType::SW : MipsType::SB;
	genCode(MipsCode::iOp(type, rt, rs, iorl));
}


void MipsGen::gen() {
	if (mid.empty()) {
		return;
	}
	iter = mid.begin();
	global();
	// obj << ".globl " <<FUNC_PREFIX << S_MAIN << std::endl;
	obj << ".text" << std::endl;
	obj << "j " << FUNC_PREFIX << S_MAIN << std::endl;
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
			error("illegal const type");
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
		} else {
			error("illegal const type");
		}
		++iter;
	}
	for (auto str : table.getStrs()) {
		obj << str.second << ": .asciiz ";
		for (auto ch : str.first) {
			if (ch == '\\') {
				obj << "\\\\";
			} else {
				obj << ch;
			}
		}
		obj << endl;
	}
	obj << STR_LB << ": .asciiz \"\\n\"" << endl;
}

void MipsGen::func(){
	using namespace std;
	if (iter->getType() != MidType::FUNC) {
		error("expected init func");
	} else {
		string funcName = iter->getOp2();
		obj << FUNC_PREFIX << funcName << ":" << endl;
		++iter;
		if (funcName == S_MAIN) {
			enterFunc(true);
			funcBody(true);
			genCode(MipsCode::iOp(MipsType::LI, R_V0, 0, SYS_EXIT));
			genCode(MipsCode::syscall());
			++iter;
		} else {
			enterFunc(false);
			funcBody(false);
			quitFunc();
			vars.clear();
			++iter;
		}
	}
}

void MipsGen::enterFunc(bool main) {
	using namespace std;
	srDepth = 0;
	trDepth = 0;
	if (!main) {
		int len = sizeof(R_SS) / sizeof(R_SS[0]);
		for (int i = 0; i < len; ++i) {
			genCode(MipsCode::iOp(MipsType::SW, R_SS[i], R_SP, -4 * i));
		}
	}
	for (int i = 0; iter != mid.end() && iter->getType() == MidType::PARAM; ++iter, ++i) {
		std::string paraName = iter->getResOp();
		int sr = allocSr();
		ArgType atype = iter->getOp1() == S_CHAR ? ArgType::CHAR : ArgType::INT;
		vars[paraName] = { true, sr, -4 * (i + 1), atype, RunVarType::LOCAL};
		regs[sr] = { iter->getResOp(), true };
		if (i < 4) {
			genCode(MipsCode::iOp(MipsType::ADDIU, sr, R_AS[i], "0"));
			store(sr, R_SP, 4 * (i + 1), atype);
		} else {
			load(sr, R_SP, 4 * (i + 1), atype);
		}
	}
	if (!main) {
		int len = sizeof(R_SS) / sizeof(R_SS[0]);
		genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, -4 * len));
		srDepth += 4 * len;
	}
	while (iter != mid.end() && iter->getType() == MidType::CONST) {
		std::string paraName = iter->getResOp();
		int sr = allocSr();
		ArgType atype = iter->getOp1() == S_CHAR ? ArgType::CHAR : ArgType::INT;
		vars[paraName] = { true, sr, srDepth, atype, RunVarType::LOCAL };
		regs[sr] = { iter->getResOp(), true };
		genCode(MipsCode::iOp(MipsType::LI, sr, 0, iter->getOp2()));
		store(sr, R_SP, 0, atype);
		genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, -4));
		srDepth += 4;
		++iter;
	}
	int old = srDepth;
	while (iter != mid.end() && iter->getType() == MidType::VAR) {
		std::string paraName = iter->getResOp();
		ArgType atype = iter->getOp1() == S_CHAR ? ArgType::CHAR : ArgType::INT;
		vars[paraName] = { false, 0, srDepth, atype, RunVarType::LOCAL };
		int space = 0;
		if (iter->getOp2().empty()) {
			space = 4;
		} else {
			space = 4 * stoi(iter->getOp2());
		}
		srDepth += space;
		++iter;
	}
	if (srDepth > old) {
		genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, old - srDepth));
	}
}

void MipsGen::funcBody(bool main) {
	using namespace std;
	vector<MidCode> params;
	for (MidType type = iter->getType(); iter != mid.end() && type != MidType::END_FUNC; type = (++iter)->getType()) {
		switch (type) {
		case MidType::LABEL:
			popTr();
			obj << LABEL_PREFIX << iter->getResOp() << ":" << endl;
			freshSr();
			break;
		case MidType::JMP:
			popTr();
			genCode(MipsCode::jOp(MipsType::J, LABEL_PREFIX + iter->getResOp()));
			freshSr();
			break;
		case MidType::PUSH:
			params.push_back(*iter);
			break;
		case MidType::CALL:
		{
			int old = trDepth;
			for (auto& iter : vars) {
				if (iter.second.type == RunVarType::TEMP && iter.second.inReg) {
					iter.second.stackPos = srDepth + trDepth;
					genCode(MipsCode::iOp(MipsType::SW, iter.second.reg, R_SP, old - trDepth));
					trDepth += 4;
				}
			}
			int raPos = srDepth + trDepth;
			genCode(MipsCode::iOp(MipsType::SW, R_RA, R_SP, old - trDepth));
			trDepth += 4;
			genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, -(trDepth - old + 4 * params.size())));
			trDepth += 4 * params.size();
			for (int i = 0; i < params.size(); ++i) {
				if (i < 4) {
					genCode(MipsCode::iOp(MipsType::ADDIU, R_AS[i], use(params[i].getResOp()), 0));
				} else {
					genCode(MipsCode::iOp(MipsType::SW, use(params[i].getResOp()), R_SP, 4 * (i + 1)));
				}
			}
			genCode(MipsCode::jOp(MipsType::JAL, FUNC_PREFIX + iter->getResOp()));
			for (auto& iter : vars) {
				if (iter.second.type == RunVarType::TEMP && iter.second.inReg) {
					genCode(MipsCode::iOp(MipsType::LW, iter.second.reg, R_SP, srDepth + trDepth - iter.second.stackPos));
				}
			}
			genCode(MipsCode::iOp(MipsType::LW, R_RA, R_SP, srDepth + trDepth - raPos));
			genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, trDepth - old));
			trDepth = old;
			params.clear();
			clearGlobal();
			break;

		}
		case MidType::READ:
			if (iter->getOp1() == S_INT) {
				genCode(MipsCode::iOp(MipsType::LI, R_V0, 0, SYS_READ_INT));
			} else {
				genCode(MipsCode::iOp(MipsType::LI, R_V0, 0, SYS_READ_CHR));
			}
			genCode(MipsCode::syscall());
			genCode(MipsCode::iOp(MipsType::ADDIU, change(iter->getResOp()), R_V0, 0));
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
			genCode(MipsCode::iOp(objType, 0, r, LABEL_PREFIX + iter->getResOp()));
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
			// MipsType objType = (globals[iter->getResOp()].type == IdenType::VAR_INT) ? MipsType::SW : MipsType::SB;
			if (vars.find(iter->getResOp()) == vars.end()) {
				MipsType objType = (globals[iter->getResOp()].type == IdenType::VAR_INT) ? MipsType::SW : MipsType::SB;
				if (objType == MipsType::SB) {
					genCode(MipsCode::iOp(objType, rr, use(iter->getOp1()), GLOBAL_PREFIX + iter->getResOp()));
				} else {
					genCode(MipsCode::iOp(MipsType::MULI, R_V1, use(iter->getOp1()), 4));
					genCode(MipsCode::iOp(objType, rr, R_V1, GLOBAL_PREFIX + iter->getResOp()));
				}
			} else {
				MipsType objType = (vars[iter->getResOp()].atype == ArgType::INT) ? MipsType::SW : MipsType::SB;
				int rindex = use(iter->getOp1());
				int offset = trDepth + srDepth - vars[iter->getResOp()].stackPos;
				genCode(MipsCode::iOp(MipsType::MULI, R_V1, rindex, 4));
				genCode(MipsCode::rOp(MipsType::SUBU, R_V1, R_SP, R_V1));
				genCode(MipsCode::iOp(objType, rr, R_V1, offset));
			}
		}
			break;
		case MidType::RARRAY_ASSIGN:
		{
			int rr = use(iter->getOp2());
			// MipsType objType = (globals[iter->getResOp()].type == IdenType::VAR_INT) ? MipsType::LW : MipsType::LB;
			if (vars.find(iter->getOp1()) == vars.end()) {
				MipsType objType = (globals[iter->getOp1()].type == IdenType::VAR_INT) ? MipsType::LW : MipsType::LB;
				if (objType == MipsType::LB) {
					genCode(MipsCode::iOp(objType, change(iter->getResOp()), rr, GLOBAL_PREFIX + iter->getOp1()));
				} else {
					genCode(MipsCode::iOp(MipsType::MULI, R_V1, rr, 4));
					genCode(MipsCode::iOp(objType, change(iter->getResOp()), R_V1, GLOBAL_PREFIX + iter->getOp1()));
				}
				wb(iter->getResOp());
			} else {
				MipsType objType = (vars[iter->getOp1()].atype == ArgType::INT) ? MipsType::LW : MipsType::LB;
				int rchange = change(iter->getResOp());
				int offset = trDepth + srDepth - vars[iter->getOp1()].stackPos;
				genCode(MipsCode::iOp(MipsType::MULI, R_V1, rr, 4));
				genCode(MipsCode::rOp(MipsType::SUBU, R_V1, R_SP, R_V1));
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
			error("unexpected midtype");
			++iter;
			break;
		}
	}
}

void MipsGen::quitFunc() {
	popTr();
	genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, srDepth));
	int len = sizeof(R_SS) / sizeof(R_SS[0]);
	for (int i = 0; i < len; ++i) {
		genCode(MipsCode::iOp(MipsType::LW, R_SS[i], R_SP, -4 * i));
	}
	freshSr();
	regs = regVec();
	genCode(MipsCode::rOp(MipsType::JR, 0, R_RA, 0));
}

int MipsGen::use(const std::string& varName) {
	if (varName == RET_TV) {
		return R_V0;
	}
	if (varName == ZERO_TV) {
		return R_ZERO;
	}
	int reg = 0;
	if (vars.find(varName) == vars.end()) {
		if (globals.find(varName) == globals.end()) {
			error("use var not existed:" + varName);
		} else {
			reg = allocSr();
			ArgType atype = (globals.find(varName)->second.type == IdenType::VAR_INT || globals.find(varName)->second.type == IdenType::CONST_INT) ?
				ArgType::INT : ArgType::CHAR;
			load(reg, R_ZERO, GLOBAL_PREFIX + varName, atype);
			vars[varName] = { true, reg, 0, atype, RunVarType::GLOBAL };
		}
	} else {
		RunVarInfo& info = vars.find(varName)->second;
		if (info.inReg) {
			reg = info.reg;
		} else {
			if (info.type == RunVarType::LOCAL) {
				reg = allocSr();
				load(reg, R_SP, trDepth + srDepth - info.stackPos, info.atype);
				info.inReg = true;
				info.reg = reg;
			} else if (info.type == RunVarType::GLOBAL) {
				reg = allocSr();
				load(reg, R_ZERO, GLOBAL_PREFIX + varName, info.atype);
				info.inReg = true;
				info.reg = reg;
			} else if (info.type == RunVarType::TEMP) {
				reg = allocTr();
				load(reg, R_SP, trDepth + srDepth - info.stackPos, info.atype);
			} else {
				error("use illegal var");
			}
		}
		if (info.type == RunVarType::TEMP) {
			deactive(varName);
			return reg;
		}
	}
	regs[reg].var = varName;
	regs[reg].full = true;
	return reg;
}

int MipsGen::change(const std::string& varName) {
	if (varName == RET_TV) {
		clearIv();
		return R_V0;
	}
	if (varName == ZERO_TV) {
		return R_ZERO;
	}
	int reg = 0;
	if (vars.find(varName) == vars.end()) {
		if (globals.find(varName) == globals.end()) {
			reg = allocTr();
			if (varName[1] == 't') {
				vars[varName] = { true, reg, 0, ArgType::INT, RunVarType::TEMP };
			} else {
				vars[varName] = { true, reg, 0, ArgType::INT, RunVarType::INLINE };
			}
		} else {
			reg = allocSr();
			ArgType atype = (globals.find(varName)->second.type == IdenType::VAR_INT || globals.find(varName)->second.type == IdenType::CONST_INT) ?
				ArgType::INT : ArgType::CHAR;
			// load(reg, R_ZERO, GLOBAL_PREFIX + varName, atype);
			vars[varName] = { true, reg, 0, atype, RunVarType::GLOBAL };
		}
	} else {
		RunVarInfo& info = vars.find(varName)->second;
		if (info.inReg) {
			reg = info.reg;
		} else {
			if (info.type == RunVarType::LOCAL) {
				reg = allocSr();
				// load(reg, R_SP, trDepth + srDepth - info.stackPos, info.atype);
				info.inReg = true;
				info.reg = reg;
			} else if (info.type == RunVarType::GLOBAL) {
				reg = allocSr();
				// load(reg, R_ZERO, GLOBAL_PREFIX + varName, info.atype);
				info.inReg = true;
				info.reg = reg;
			} else if (info.type == RunVarType::TEMP || info.type == RunVarType::INLINE) {
				reg = allocTr();
				// load(reg, R_SP, trDepth + srDepth - info.stackPos, info.atype);
				info.inReg = true;
				info.reg = reg;
			} else {
				error("change illegal var");
			}
		}
	}
	regs[reg].var = varName;
	regs[reg].full = true;
	return reg;
}

void MipsGen::wb(const std::string& varName) {
	if (varName == RET_TV || varName == ZERO_TV || vars.find(varName) == vars.end()) {
		return;
	}
	RunVarInfo info = vars.find(varName)->second;
	if (info.type == RunVarType::LOCAL) {
		store(info.reg, R_SP, srDepth + trDepth - info.stackPos, info.atype);
	} else if (info.type == RunVarType::GLOBAL) {
		store(info.reg, R_ZERO, GLOBAL_PREFIX + varName, info.atype);
	}
}

void MipsGen::deactive(const std::string& varName) {
	int reg = vars.find(varName)->second.reg;
	regs[reg].full = false;
	vars.erase(varName);
}

void MipsGen::clearIv() {
	for (int i = 0; i < sizeof(R_TS) / sizeof(R_TS[0]); ++i) {
		if (regs[R_TS[i]].full) {
			auto var = regs[R_TS[i]].var;
			if (vars[var].type == RunVarType::INLINE) {
				deactive(var);
			}
		}
	}
}

void MipsGen::popTr() {
	for (int i = 0; i < sizeof(R_TS) / sizeof(R_TS[0]); ++i) {
		if (regs[R_TS[i]].full) {
			deactive(regs[R_TS[i]].var);
		}
	}
	if (trDepth) {
		genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, trDepth));
		trDepth = 0;
	}
	trCount = 0;
}

void MipsGen::freshSr() {
	for (int i = 0; i < sizeof(R_SS) / sizeof(R_SS[0]); ++i) {
		if (regs[R_SS[i]].full) {
			regs[R_SS[i]].full = false;
			vars[regs[R_SS[i]].var].inReg = false;
		}
	}
	srCount = 0;
}

int MipsGen::allocSr() {
	int reg = R_SS[srCount];
	if (regs[reg].full) {
		regs[reg].full = false;
		vars[regs[reg].var].inReg = false;
	}
	srCount = (srCount >= sizeof(R_SS) / sizeof(R_SS[0]) - 1) ? 0 : srCount + 1;
	return reg;
}

int MipsGen::allocTr() {
	for (int i = 0; i < sizeof(R_TS) / sizeof(R_TS[0]); ++i) {
		if (!regs[R_TS[i]].full) {
			return R_TS[i];
		}
	}
	int reg = R_TS[trCount];
	while (vars[regs[reg].var].type == RunVarType::INLINE) {
		trCount = (trCount >= sizeof(R_TS) / sizeof(R_TS[0]) - 1) ? 0 : trCount + 1;
		reg = R_TS[trCount];
	}
	regs[reg].full = false;
	genCode(MipsCode::iOp(MipsType::SW, reg, R_SP, 0));
	genCode(MipsCode::iOp(MipsType::ADDIU, R_SP, R_SP, -4));
	vars[regs[reg].var].inReg = false;
	vars[regs[reg].var].stackPos = srDepth + trDepth;
	trDepth += 4;
	trCount = (trCount >= sizeof(R_TS) / sizeof(R_TS[0]) - 1) ? 0 : trCount + 1;
	return reg;
}

void MipsGen::clearGlobal() {
	for (auto& iter : vars) {
		if (iter.second.type == RunVarType::GLOBAL && iter.second.inReg) {
			int reg = iter.second.reg;
			regs[reg].full = false;
			iter.second.inReg = false;
		}
	}
}

void MipsGen::error(const std::string& info) {
	std::cerr << (iter - mid.begin() + 1) << ":" << info << std::endl;
}

