#pragma once

#include <map>
#include <vector>
#include "Token.h"
#include "Error.h"

enum class IdenType {
	NONE,
	CONST_INT,
	CONST_CHAR,
	VAR_INT,
	VAR_CHAR,
	INT_FUNC,
	CHAR_FUNC,
	VOID_FUNC
};

enum class ArgType {
	INT,
	CHAR,
	VOID,
};

struct varInfo {
	IdenType type;
	int dim;
};

struct funcInfo {
	IdenType type;
	std::vector<ArgType> args;
};

class SymbolTable {
public:
	static SymbolTable& getInstance();
	static void deleteInstance();
	bool push(Token& tk, IdenType type);
	IdenType find(Token& tk);
	void addArgs(Token& tk, std::vector<ArgType>& args);
	std::vector<ArgType> getArgs(Token& tk);
	void addDim(Token& tk, int dim);
	void newSub(std::string funcName);
	void exitSub();
	std::map<const std::string, varInfo> getGlobals();
	std::map<const std::string, varInfo> getLocals(std::string funcName);

private:
	static SymbolTable* instance;
	std::map<const std::string, varInfo> globalVars;
	std::map<const std::string, funcInfo> funcs;
	std::string curName;
	std::map<const std::string, varInfo> curVars;
	std::map<const std::string, std::map<const std::string, varInfo>> localVars;
};

