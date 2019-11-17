#include "MidCode.h"

int MidCode::tvCount = 0;
int MidCode::lbCount = 0;
std::ofstream* MidCode::ofs = nullptr;
std::vector<MidCode> MidCode::vec;

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
	using namespace std;
	vec.push_back(*this);
	// *ofs << resOp << op1 << op2 << ")";
	switch (type) {
	case MidType::NONE:
		*ofs << "<nop>" << endl;
		break;
	case MidType::END_FUNC:
		*ofs << "<end> " << resOp << endl;
		break;
	case MidType::LABEL:
		*ofs << resOp << ":" << endl;
		break;
	case MidType::JMP:
		*ofs << "<jmp> " << resOp << endl;
		break;
	case MidType::PUSH:
		*ofs << "<push> " << resOp << endl;
		break;
	case MidType::CALL:
		*ofs << "<call> " << resOp << endl;
		break;
	case MidType::READ:
		*ofs << "<read> " << resOp << endl;
		break;
	case MidType::RETURN:
		*ofs << "<ret> " << resOp << endl;
		break;
	case MidType::FUNC:
		*ofs << "<func> " << op1 << " " << op2 << "()" << endl;
		break;
	case MidType::WRITE:
		*ofs << "<write> ";
		if (op1.empty()) {
			*ofs << op2 << endl;
		} else if (op2.empty()) {
			*ofs << op1 << endl;
		} else {
			*ofs << op1 << ", " << op2 << endl;
		}
		break;
	case MidType::VAR:
		*ofs << "<var> " << op1 << " " << resOp;
		if (!op2.empty()) {
			*ofs << "[" << op2 << "]";
		}
		*ofs << endl;
		break;
	case MidType::PARAM:
		*ofs << "<param> " << op1 << " " << resOp << endl;
		break;
	case MidType::CONST:
		*ofs << "<const> " << op1 << " " << resOp << " = " << op2 << endl;
		break;
	case MidType::BEQ:
		*ofs << "<beq> " << op1 << ", " << op2 << ", " << resOp << endl;
		break;
	case MidType::BNE:
		*ofs << "<bne> " << op1 << ", " << op2 << ", " << resOp << endl;
		break;
	case MidType::BLTZ:
		*ofs << "<bltz> " << op1 << ", " << resOp << endl;
		break;
	case MidType::BLEZ:
		*ofs << "<blez> " << op1 << ", " << resOp << endl;
		break;
	case MidType::BGTZ:
		*ofs << "<bgtz> " << op1 << ", " << resOp << endl;
		break;
	case MidType::BGEZ:
		*ofs << "<bgez> " << op1 << ", " << resOp << endl;
		break;
	case MidType::ASSIGN:
		*ofs << resOp << " = " << op1 << endl;
		break;
	case MidType::LARRAY_ASSIGN:
		*ofs << resOp << "[" << op1 << "] = " << op2 << endl;
		break;
	case MidType::RARRAY_ASSIGN:
		*ofs << resOp << " = " << op1 << "[" << op2 << "]" << endl;
		break;
	case MidType::MINU:
		*ofs << resOp << " = " << op1 << " - " << op2 << endl;
		break;
	case MidType::PLUS:
		*ofs << resOp << " = " << op1 << " + " << op2 << endl;
		break;
	case MidType::MUL:
		*ofs << resOp << " = " << op1 << " * " << op2 << endl;
		break;
	case MidType::DIV:
		*ofs << resOp << " = " << op1 << " / " << op2 << endl;
		break;
	default:
		*ofs << "!!!" << endl;
		break;
	}
}