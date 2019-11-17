#pragma once
#include "MipsCode.h"
#include "MidCode.h"
#include "SymbolTable.h"
#include "symbols.h"
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

constexpr auto GLOBAL_PREFIX = "_global_";
constexpr auto FUNC_PREFIX = "_func_";
constexpr auto LABEL_PREFIX = "_label_";
constexpr auto STR_LB = "_linebreak";

enum class RunVarType {
	NONE,
	GLOBAL,
	LOCAL,
	TEMP,
};

struct RegInfo {
	std::string name;
	std::string var;
	bool full;
};

struct RunVarInfo {
	bool inMem;
	bool alive;
	int reg;
	int stackPos;
	ArgType atype;
	RunVarType type;
};


class MipsGen {
public:
	static MipsGen& getInstance(std::vector<MidCode>& mid, std::ofstream& obj, SymbolTable& table);
	static void deleteInstance();
	void gen();

private:
	static MipsGen* instance;
	MipsGen(std::vector<MidCode>& mid, std::ofstream& obj, SymbolTable& table);
	std::vector<MidCode>& mid;
	std::ofstream& obj;
	SymbolTable& table;
	std::vector<MidCode>::iterator iter;
	std::vector<RegInfo> regs;
	std::map<std::string, RunVarInfo> vars;
	std::map<const std::string, varInfo> globals;
	int srDepth;
	int trDepth;

	void genCode(MipsCode code);
	void load(int rt, int rs, std::string iorl, ArgType atype);
	void store(int rt, int rs, std::string iorl, ArgType atype);
	void load(int rt, int rs, int iorl, ArgType atype);
	void store(int rt, int rs, int iorl, ArgType atype);
	void global();
	void func();
	void enterFunc();
	void funcBody(bool main);
	void quitFunc();
	int use(const std::string& varName);
	int change(const std::string& varName);
	void wb(const std::string& varName);
	void deactive(const std::string& varName);
	void popTr();
	void freshSr();
	int allocSr(std::string& varName);
	int allocTr(std::string& varName);
};

