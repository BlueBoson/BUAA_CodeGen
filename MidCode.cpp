#include "MidCode.h"

int MidCode::tvCount = 0;
int MidCode::lbCount = 0;
std::ofstream* MidCode::ofs = nullptr;
std::vector<MidCode> MidCode::vec;
std::vector<MidCode>* MidCode::holdVec = nullptr;

void MidCode::init(std::ofstream& _ofs) {
	ofs = &_ofs;
	tvCount = 0;
	lbCount = 0;
	vec.clear();
}

std::string MidCode::genTv() {
	return "$t" + std::to_string(tvCount++);
}

std::string MidCode::genLb() {
	return "label" + std::to_string(lbCount++);
}

void MidCode::hold(std::vector<MidCode>* vec) {
	holdVec = vec;
}

void MidCode::cancelHold() {
	holdVec = nullptr;
}

MidCode::MidCode(MidType type) : MidCode(type, "", "", "") {}

MidCode::MidCode(MidType type, std::string resOp) : MidCode(type, resOp, "", "") {}

MidCode::MidCode(MidType type, std::string op1, std::string op2) : MidCode(type, "", op1, op2) {}

MidCode::MidCode(MidType type, std::string resOp, std::string op1, std::string op2) {
	this->type = type;
	this->resOp = resOp;
	this->op1 = op1;
	this->op2 = op2;
}

void MidCode::setOp1(std::string op1) {
	this->op1 = op1;
}

void MidCode::setOp2(std::string op2) {
	this->op2 = op2;
}

void MidCode::setResOp(std::string resOp) {
	this->resOp = resOp;
}

MidType MidCode::getType() {
	return type;
}

std::string MidCode::getOp1() {
	return op1;
}

std::string MidCode::getOp2() {
	return op2;
}

std::string MidCode::getResOp() {
	return resOp;
}

std::vector<MidCode> MidCode::getVec() {
	return vec;
}

void MidCode::emit() {
	if (holdVec) {
		holdVec->push_back(*this);
		return;
	}
	using namespace std;
	vec.push_back(*this);
	*ofs << toString() << endl;
}

std::string MidCode::toString() {
	using namespace std;
	string s;
	switch (type) {
	case MidType::NONE:
		s = "<nop>";
		break;
	case MidType::END_FUNC:
		s = "<end> " + resOp;
		break;
	case MidType::LABEL:
		s = resOp + ":";
		break;
	case MidType::JMP:
		s = "<jmp> " + resOp;
		break;
	case MidType::PUSH:
		s = "<push> " + resOp;
		break;
	case MidType::CALL:
		s = "<call> " + resOp;
		break;
	case MidType::READ:
		s = "<read> " + resOp;
		break;
	case MidType::RETURN:
		s = "<ret> " + resOp;
		break;
	case MidType::FUNC:
		s = "<func> " + op1 + " " + op2 + "()";
		break;
	case MidType::WRITE:
		s = "<write> ";
		if (op1.empty()) {
			s += op2;
		} else if (op2.empty()) {
			s += op1;
		} else {
			s += op1 + ", " + op2;
		}
		break;
	case MidType::VAR:
		s = "<var> " + op1 + " " + resOp;
		if (!op2.empty()) {
			s += "[" + op2 + "]";
		}
		break;
	case MidType::PARAM:
		s = "<param> " + op1 + " " + resOp;
		break;
	case MidType::CONST:
		s = "<const> " + op1 + " " + resOp + " = " + op2;
		break;
	case MidType::BEQ:
		s = "<beq> " + op1 + ", " + op2 + ", " + resOp;
		break;
	case MidType::BNE:
		s = "<bne> " + op1 + ", " + op2 + ", " + resOp;
		break;
	case MidType::BLTZ:
		s = "<bltz> " + op1 + ", " + resOp;
		break;
	case MidType::BLEZ:
		s = "<blez> " + op1 + ", " + resOp;
		break;
	case MidType::BGTZ:
		s = "<bgtz> " + op1 + ", " + resOp;
		break;
	case MidType::BGEZ:
		s = "<bgez> " + op1 + ", " + resOp;
		break;
	case MidType::ASSIGN:
		s = resOp + " = " + op1;
		break;
	case MidType::LARRAY_ASSIGN:
		s = resOp + "[" + op1 + "] = " + op2;
		break;
	case MidType::RARRAY_ASSIGN:
		s = resOp + " = " + op1 + "[" + op2 + "]";
		break;
	case MidType::MINU:
		s = resOp + " = " + op1 + " - " + op2;
		break;
	case MidType::PLUS:
		s = resOp + " = " + op1 + " + " + op2;
		break;
	case MidType::MUL:
		s = resOp + " = " + op1 + " * " + op2;
		break;
	case MidType::DIV:
		s = resOp + " = " + op1 + " / " + op2;
		break;
	default:
		s = "!!!";
		break;
	}
	return s;
}