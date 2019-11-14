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

class SymbolTable {
public:
	static SymbolTable& getInstance();
	static void deleteInstance();
	bool push(Token& tk, IdenType type);
	IdenType find(Token& tk);
	void addArgs(Token& tk, std::vector<ArgType>& args);
	std::vector<ArgType> getArgs(Token& tk);
	void newSub();
	void exitSub();

private:
	static SymbolTable* instance;
	std::vector<std::map<const std::string, IdenType>> tables;
	std::map<const std::string, std::vector<ArgType>> funcArgs;
};

