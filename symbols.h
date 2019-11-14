#pragma once

constexpr auto C_END = '\0';
constexpr auto C_WHITESPACE = ' ';
constexpr auto S_EMPTY = "";
constexpr auto C_SQUOTE = '\'';
constexpr auto C_DQUOTE = '\"';

constexpr auto C_PLUS = '+';
constexpr auto C_MINU = '-';
constexpr auto C_MULT = '*';
constexpr auto C_DIV = '/';

constexpr auto C_LSS = '<';
constexpr auto C_GRE = '>';
constexpr auto C_EQ = '=';
constexpr auto C_EXC = '!';

constexpr auto C_SEMICN = ';';
constexpr auto C_COMMA = ',';
constexpr auto C_LPARENT = '(';
constexpr auto C_RPARENT = ')';
constexpr auto C_LBRACK = '[';
constexpr auto C_RBRACK = ']';
constexpr auto C_LBRACE = '{';
constexpr auto C_RBRACE = '}';

constexpr auto S_CONST = "const";
constexpr auto S_INT = "int";
constexpr auto S_CHAR = "char";
constexpr auto S_VOID = "void";
constexpr auto S_MAIN = "main";
constexpr auto S_IF = "if";
constexpr auto S_ELSE = "else";
constexpr auto S_DO = "do";
constexpr auto S_WHILE = "while";
constexpr auto S_FOR = "for";
constexpr auto S_SCANF = "scanf";
constexpr auto S_PRINTF = "printf";
constexpr auto S_RETURN = "return";

inline bool isDigit(char x) {
	return x >= '0' && x <= '9';
}

inline bool isZero(char x) {
	return x == '0';
}

inline bool isPosDigit(char x) {
	return x >= '1' && x <= '9';
}

inline bool isLetter(char x) {
	return (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || x == '_';
}

inline bool isLegalChar(char x) {
	return x == '+' || x == '-' || x == '*' || x == '/' || (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || (x >= '0' && x <= '9') || x == '_';
}

inline bool isStrEle(char x) {
	return x == 32 || x == 33 || (x >= 35 && x <= 126);
}