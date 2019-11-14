#pragma once
#include <string>
#include <iostream>
#include <fstream>

constexpr auto RET_TV = "#RET";
constexpr auto ZERO_TV = "#ZERO";

enum class MidType {
	// no op
	NONE,
	// only res-op
	END_FUNC,
	LABEL,
	JMP,
	PUSH,
	CALL,
	READ,
	RETURN,
	// op1, op2
	FUNC,
	WRITE,
	// op1, op2, res
	VAR,
	PARAM,
	CONST,
	BEQ,
	BNE,
	BLTZ,
	BLEZ,
	BGTZ,
	BGEZ,
	ASSIGN,
	LARRAY_ASSIGN,
	RARRAY_ASSIGN,
	MINU,
	PLUS,
	MUL,
	DIV
};

class MidCode {
public:
	static void init(std::ofstream& _ofs);
	static std::string genTv();
	static std::string genLb();
	MidCode(MidType type);
	MidCode(MidType type, std::string resOp);
	MidCode(MidType type, std::string op1, std::string op2);
	MidCode(MidType type, std::string resOp, std::string op1, std::string op2);
	void setOp1(std::string op1);
	void setOp2(std::string op2);
	void setResOp(std::string resOp);
	void emit();

private:
	static int tvCount;
	static int lbCount;
	static std::ofstream* ofs;
	MidType type;
	std::string resOp;
	std::string op1;
	std::string op2;
};

