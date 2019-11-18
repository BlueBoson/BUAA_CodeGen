#pragma once
#include <string>
#include <vector>

constexpr int R_ZERO = 0;
constexpr int R_V0 = 2;
constexpr int R_V1 = 3;
constexpr int R_AS[] = { 4, 5, 6, 7 };
constexpr int R_TS[] = { 8, 9, 10, 11, 12, 13, 14, 15, 24, 25 };
constexpr int R_SS[] = { 16, 17, 18, 19, 20, 21, 22, 23 };
constexpr int R_SP = 29;
constexpr int R_RA = 31;
constexpr int SYS_PRT_INT = 1;
constexpr int SYS_PRT_STR = 4;
constexpr int SYS_PRT_CHR = 11;
constexpr int SYS_READ_INT = 5;
constexpr int SYS_READ_CHR = 12;
constexpr int SYS_EXIT = 10;

enum class MipsType {
	SYSCALL,
	// R-type
	ADDU, // addu $rd, $rs, $rt
	SUBU, // subu $rd, $rs, $rt
	MUL, // mul $rd, $rs, $rt
	DIV, // extend div $rd, $rs, $rt
	JR, // jr $rs
	// I_type
	LA, // extend la $rt, label
	LI, // extend li $rt, imediate
	ADDIU, // addiu $rt, $rs, imediate
	SUBIU, // subiu $rt, $rs, imediate
	MULI, // extend muli $rt, $rs, imediate
	DIVI, // extend divi $rt, $rs, imediate
	BEQ, // beq $rs, $rt, label
	BNE, // bne $rs, $rt, label
	BGEZ, // bgez $rs, label
	BGTZ, // bgtz $rs, label
	BLEZ, // blez $rs, label
	BLTZ, // bltz $rs, label
	LW, // lw $rt, imediate($rs); extend lw $rt, label($rs)
	SW, // sw $rt, imediate($rs); extend sw $rt, label($rs)
	LB, // lb $rt, imediate($rs); extend lb $rt, label($rs)
	SB, // sb $rt, imediate($rs); extend sb $rt, label($rs)
	// J-type
	J, // j label
	JAL, // jal label
};

class MipsCode {
public:
	static MipsCode rOp(MipsType type, int rd, int rs, int rt);
	static MipsCode iOp(MipsType type, int rt, int rs, std::string iorl);
	static MipsCode iOp(MipsType type, int rt, int rs, int iorl);
	static MipsCode jOp(MipsType type, std::string label);
	static MipsCode syscall();
	MipsCode(MipsType type, int rd, int rs, int rt, std::string iorl);
	std::string toString();

private:
	static std::vector<std::string> regName;
	MipsType type;
	int rd;
	int rs;
	int rt;
	std::string iorl;
};

