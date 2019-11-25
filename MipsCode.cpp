#include "MipsCode.h"

std::vector<std::string> getRegName() {
	using namespace std;
	vector<string> vec;
	vec.push_back("$zero");
	vec.push_back("$at");
	vec.push_back("$v0");
	vec.push_back("$v1");
	for (int i = 0; i < 4; ++i) {
		vec.push_back("$a" + to_string(i));
	}
	for (int i = 0; i < 8; ++i) {
		vec.push_back("$t" + to_string(i));
	}
	for (int i = 0; i < 8; ++i) {
		vec.push_back("$s" + to_string(i));
	}
	for (int i = 8; i < 10; ++i) {
		vec.push_back("$t" + to_string(i));
	}
	vec.push_back("$k0");
	vec.push_back("$k1");
	vec.push_back("$gp");
	vec.push_back("$sp");
	vec.push_back("$fp");
	vec.push_back("$ra");
	return vec;
}

std::vector<std::string> MipsCode::regName = getRegName();

MipsCode::MipsCode(MipsType type, int rd, int rs, int rt, std::string iorl) : 
	type(type), rd(rd), rs(rs), rt(rt), iorl(iorl) {

}

MipsCode MipsCode::rOp(MipsType type, int rd, int rs, int rt) {
	return MipsCode(type, rd, rs, rt, "");
}

MipsCode MipsCode::iOp(MipsType type, int rt, int rs, std::string iorl) {
	return MipsCode(type, 0, rs, rt, iorl);
}

MipsCode MipsCode::iOp(MipsType type, int rt, int rs, int iorl) {
	return MipsCode(type, 0, rs, rt, std::to_string(iorl));
}

MipsCode MipsCode::jOp(MipsType type, std::string label) {
	return MipsCode(type, 0, 0, 0, label);
}

MipsCode MipsCode::syscall() {
	return MipsCode(MipsType::SYSCALL, 0, 0, 0, "");
}

std::string MipsCode::toString() {
	using namespace std;
	string op;
	string srd = regName[rd];
	string srs = regName[rs];
	string srt = regName[rt];
	switch (type) {
	case MipsType::SYSCALL:
		op = "syscall";
		break;
	case MipsType::ADDU:
		op = "addu " + srd + ", " + srs + ", " + srt;
		break;
	case MipsType::SUBU:
		op = "subu " + srd + ", " + srs + ", " + srt;
		break;
	case MipsType::MUL:
		op = "mul " + srd + ", " + srs + ", " + srt;
		break;
	case MipsType::DIV:
		op = "div " + srd + ", " + srs + ", " + srt;
		break;
	case MipsType::JR:
		op = "jr " + srs;
		break;
	case MipsType::LA:
		op = "la " + srt + ", " + iorl;
		break;
	case MipsType::LI:
		op = "li " + srt + ", " + iorl;
		break;
	case MipsType::ADDIU:
		op = "addiu " + srt + ", " + srs + ", " + iorl;
		break;
	case MipsType::SUBIU:
		op = "subiu " + srt + ", " + srs + ", " + iorl;
		break;
	case MipsType::MULI:
		op = "mul " + srt + ", " + srs + ", " + iorl;
		break;
	case MipsType::DIVI:
		op = "div " + srt + ", " + srs + ", " + iorl;
		break;
	case MipsType::BEQ:
		op = "beq " + srs + ", " + srt + ", " + iorl;
		break;
	case MipsType::BNE:
		op = "bne " + srs + ", " + srt + ", " + iorl;
		break;
	case MipsType::BGEZ:
		op = "bgez " + srs + ", " + iorl;
		break;
	case MipsType::BGTZ:
		op = "bgtz " + srs + ", " + iorl;
		break;
	case MipsType::BLEZ:
		op = "blez " + srs + ", " + iorl;
		break;
	case MipsType::BLTZ:
		op = "bltz " + srs + ", " + iorl;
		break;
	case MipsType::LW:
		op = "lw " + srt + ", " + iorl + "(" + srs + ")";
		break;
	case MipsType::SW:
		op = "sw " + srt + ", " + iorl + "(" + srs + ")";
		break;
	case MipsType::LB:
		op = "lb " + srt + ", " + iorl + "(" + srs + ")";
		break;
	case MipsType::SB:
		op = "sb " + srt + ", " + iorl + "(" + srs + ")";
		break;
	case MipsType::J:
		op = "j " + iorl;
		break;
	case MipsType::JAL:
		op = "jal " + iorl;
		break;
	default:
		break;
	}
	return op;
}