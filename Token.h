#pragma once

#include <string>
#include <map>
#include "symbols.h"

enum class TkType {
	EMPTY, // 无词
	ILLEGAL, // 非法词

	IDENFR, // 标识符
	INTCON, // 整型常量
	CHARCON, // 字符常量
	STRCON, // 字符串

	CONSTTK, // const
	INTTK, // int
	CHARTK, // char
	VOIDTK, // void
	MAINTK, // main
	IFTK, // if
	ELSETK, // else
	DOTK, // do
	WHILETK, // while
	FORTK, // for
	SCANFTK, // scanf
	PRINTFTK, // printf
	RETURNTK, // return

	PLUS, // +
	MINU, // -
	MULT, // *
	DIV, // /
	LSS, // <
	LEQ, // <=
	GRE, // >
	GEQ, // >=
	EQL, // ==
	NEQ, // !=
	ASSIGN, // =

	SEMICN, // ;
	COMMA, // ,
	LPARENT, // (
	RPARENT, // )
	LBRACK, // [
	RBRACK, // ]
	LBRACE, // {
	RBRACE, // }
};

enum class ValueType {
	V_RES,
	V_INT,
	V_CHAR,
	V_STR
};

class Token {
public:
	static TkType getSingleType(char c);
	static TkType getResType(std::string& s);
	static std::string getTypeName(TkType type);
	Token(TkType _type, int _line, int _column, std::string _value=S_EMPTY);
	TkType getType();
	ValueType getVType();
	std::string getValue();
	int getLine();
	int getColumn();

private:
	static std::map<char, TkType> charToSingleType;
	static std::map<const std::string, TkType> strToResType;
	static std::map<TkType, std::string> typeToName;
	TkType type;
	ValueType vType;
	std::string value;
	int line;
	int column;
};