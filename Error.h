#pragma once

#include <fstream>
#include <iostream>
#include "Token.h"
#include "grammar.h"

enum class ErrorType {
	ILLEGAL_TK,
	DUPLICATED_NAME,
	UNDEFINED_NAME,
	UNMATCHED_ARG_NUM,
	UNMATCHED_ARG_TYPE,
	ILLEGAL_CONDITION_TYPE,
	UNMATCHED_VOID_RETURN,
	UNMATCHED_RET_RETURN,
	ILLEGAL_INDEX,
	ASSIGN_CONST,
	UNMATCHED_SEMI,
	UNMATCHED_PAREN,
	UNMATCHED_BRACK,
	MISSING_WHILE,
	ILLEGAL_CONST_DEF,
};

class Error {
public:
	static void setLogFile(std::ofstream& _ofs);
	static void cancelLogFile();
	static void setErrorFile(std::ofstream& _efs);
	static void cancelErrorFile();
	static void unexpectedToken(Token& got, TkType expected);
	static void cannotEndGrammar(Token& near, GrammarType grammar);
	static void raiseError(Token& tk, ErrorType et);
	static void raiseError(int row, ErrorType et);

private:
	static std::ofstream* ofs;
	static std::ofstream* efs;
	static std::map<ErrorType, std::string> errName;
};

