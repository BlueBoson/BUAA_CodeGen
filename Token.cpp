#include "Token.h"

Token::Token(TkType _type, int _line, int _column, std::string _value):type(_type), value(_value), line(_line), column(_column) {
	if (type == TkType::IDENFR || type == TkType::STRCON) {
		vType = ValueType::V_STR;
	} else if (type == TkType::INTCON) {
		vType = ValueType::V_INT;
	} else if (type == TkType::CHARCON) {
		vType = ValueType::V_CHAR;
	} else {
		vType = ValueType::V_RES;
	}
}

int Token::getLine() { return line; }

int Token::getColumn() { return column; }

TkType Token::getType() { return type; }

ValueType Token::getVType() { return vType; }

std::string Token::getValue(){ return value; }

std::map<char, TkType> getCharToSingleType() {
	std::map<char, TkType> m;
#define MAP_CHAR(name) m[C_##name] = TkType::name
	MAP_CHAR(PLUS);
	MAP_CHAR(MINU);
	MAP_CHAR(MULT);
	MAP_CHAR(DIV);
	MAP_CHAR(SEMICN);
	MAP_CHAR(COMMA);
	MAP_CHAR(LPARENT);
	MAP_CHAR(RPARENT);
	MAP_CHAR(LBRACK);
	MAP_CHAR(RBRACK);
	MAP_CHAR(LBRACE);
	MAP_CHAR(RBRACE);
#undef MAP_CHAR
	return m;
}

std::map<const std::string, TkType> getStrToResType() {
	std::map<const std::string, TkType> m;
#define MAP_RES(name) m[S_##name] = TkType::name##TK
	MAP_RES(CONST);
	MAP_RES(INT);
	MAP_RES(CHAR);
	MAP_RES(VOID);
	MAP_RES(MAIN);
	MAP_RES(IF);
	MAP_RES(ELSE);
	MAP_RES(DO);
	MAP_RES(WHILE);
	MAP_RES(FOR);
	MAP_RES(SCANF);
	MAP_RES(PRINTF);
	MAP_RES(RETURN);
#undef MAP_RES
	return m;
}

std::map<TkType, std::string> getTypeToName() {
	std::map<TkType, std::string> m;
#define NAME(type) m[TkType::type] = #type
	NAME(EMPTY);
	NAME(ILLEGAL);
	NAME(IDENFR);
	NAME(INTCON);
	NAME(CHARCON);
	NAME(STRCON);
	NAME(CONSTTK);
	NAME(INTTK);
	NAME(CHARTK);
	NAME(VOIDTK);
	NAME(MAINTK);
	NAME(IFTK);
	NAME(ELSETK);
	NAME(DOTK);
	NAME(WHILETK);
	NAME(FORTK);
	NAME(SCANFTK);
	NAME(PRINTFTK);
	NAME(RETURNTK);
	NAME(PLUS);
	NAME(MINU);
	NAME(MULT);
	NAME(DIV);
	NAME(LSS);
	NAME(LEQ);
	NAME(GRE);
	NAME(GEQ);
	NAME(EQL);
	NAME(NEQ);
	NAME(ASSIGN);
	NAME(SEMICN);
	NAME(COMMA);
	NAME(LPARENT);
	NAME(RPARENT);
	NAME(LBRACK);
	NAME(RBRACK);
	NAME(LBRACE);
	NAME(RBRACE);
#undef NAME
	return m;
}

auto Token::charToSingleType = getCharToSingleType();

auto Token::strToResType = getStrToResType();

auto Token::typeToName = getTypeToName();

TkType Token::getSingleType(char c) {
	auto iter = charToSingleType.find(c);
	if (iter == charToSingleType.end()) {
		return TkType::ILLEGAL;
	} else {
		return iter->second;
	}
}

TkType Token::getResType(std::string& s) {
	auto iter = strToResType.find(s);
	if (iter == strToResType.end()) {
		return TkType::ILLEGAL;
	} else {
		return iter->second;
	}
}

std::string Token::getTypeName(TkType type) {
	auto iter = typeToName.find(type);
	if (iter == typeToName.end()) {
		return S_EMPTY;
	} else {
		return iter->second;
	}
}