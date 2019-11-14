#include "LexicalAnalyser.h"
#include <iostream>

LexicalAnalyser* LexicalAnalyser::instance = NULL;

LexicalAnalyser& LexicalAnalyser::getInstance(std::ifstream& _ifs) {
	if (instance == NULL) {
		instance = new LexicalAnalyser(_ifs);
	} else if (instance->ifs != &_ifs) {
		delete instance;
		instance = new LexicalAnalyser(_ifs);
	}
	return *instance;
}

void LexicalAnalyser::deleteInstance() {
	delete instance;
	instance = NULL;
}

void LexicalAnalyser::reInitial() {
	instance->bufInd = 0;
	instance->line = 1;
	instance->column = 0;
	instance->tkStr = S_EMPTY;
	instance->ifs->seekg(0, std::ios::beg);
}

Token LexicalAnalyser::getNextToken() {
	using namespace std;
	clearBuf();
	while (isspace(curChar)) {
		nextChar();
	}
	int oldLine = line;
	int oldColumn = column;
	Token tk = Token(TkType::EMPTY, oldLine, oldColumn);
	if (curChar == C_END) {
		tk = Token(TkType::EMPTY, oldLine, oldColumn);
	} else if (isLetter(curChar)) {
		while (isLetter(curChar) || isDigit(curChar)) {
			catChar();
			nextChar();
		}
		string symbol = getStr();
		TkType resType = Token::getResType(symbol);
		if (resType == TkType::ILLEGAL) {
			tk = Token(TkType::IDENFR, oldLine, oldColumn, symbol);
		} else {
			tk = Token(resType, oldLine, oldColumn, symbol);
		}
	} else if (isZero(curChar)) {
		tk = Token(TkType::INTCON, oldLine, oldColumn, string({ curChar }));
		nextNWChar();
	} else if (isPosDigit(curChar)) {
		while (isDigit(curChar)) {
			catChar();
			nextChar();
		}
		tk = Token(TkType::INTCON, oldLine, oldColumn, getStr());
	} else if (curChar == C_LSS) {
		nextChar();
		if (curChar == C_EQ) {
			tk = Token(TkType::LEQ, oldLine, oldColumn, string({ C_LSS, C_EQ }));
			nextNWChar();
		} else {
			tk = Token(TkType::LSS, oldLine, oldColumn, string({ C_LSS }));
		}
	} else if (curChar == C_GRE) {
		nextChar();
		if (curChar == C_EQ) {
			tk = Token(TkType::GEQ, oldLine, oldColumn, string({ C_GRE, C_EQ }));
			nextNWChar();
		} else {
			tk = Token(TkType::GRE, oldLine, oldColumn, string({ C_GRE }));
		}
	} else if (curChar == C_EQ) {
		nextChar();
		if (curChar == C_EQ) {
			tk = Token(TkType::EQL, oldLine, oldColumn, string({ C_EQ, C_EQ }));
			nextNWChar();
		} else {
			tk = Token(TkType::ASSIGN, oldLine, oldColumn, string({ C_EQ }));
		}
	} else if (curChar == C_EXC) {
		nextChar();
		if (curChar == C_EQ) {
			tk = Token(TkType::NEQ, oldLine, oldColumn, string({ C_EXC, C_EQ }));
			nextNWChar();
		} else {
			tk = Token(TkType::ILLEGAL, oldLine, oldColumn, string({ C_EXC }));
		}
	} else if (Token::getSingleType(curChar) != TkType::ILLEGAL) {
		tk = Token(Token::getSingleType(curChar), oldLine, oldColumn, string({ curChar }));
		nextNWChar();
	} else if (curChar == C_DQUOTE) {
		nextChar();
		while (isStrEle(curChar) && curChar != C_DQUOTE) {
			catChar();
			nextChar();
		}
		if (curChar == C_DQUOTE) {
			tk = Token(TkType::STRCON, oldLine, oldColumn, getStr());
			nextNWChar();
		} else {
			tk = Token(TkType::ILLEGAL, oldLine, oldColumn, string({ C_DQUOTE }) + getStr());
		}
	} else if (curChar == C_SQUOTE) {
		nextChar();
		char value = curChar;
		bool legalChar = isLegalChar(curChar);
		nextChar();
		bool singleQuote = (curChar == C_SQUOTE);
		if (legalChar && singleQuote) {
			tk = Token(TkType::CHARCON, oldLine, oldColumn, string({ value }));
			nextNWChar();
		} else if (singleQuote) {
			tk = Token(TkType::ILLEGAL, oldLine, oldColumn, string({ C_SQUOTE, value, C_SQUOTE }));
			nextNWChar();
		} else {
			tk = Token(TkType::ILLEGAL, oldLine, oldColumn, string({ C_SQUOTE, value }));
		}
	} else {
		tk = Token(TkType::ILLEGAL, oldLine, oldColumn, string({ curChar }));
		nextNWChar();
	}
	return tk;
}

LexicalAnalyser::LexicalAnalyser(std::ifstream& _ifs):curChar(C_WHITESPACE), 
	bufInd(0), line(1), column(0), tkStr(""), ifs(&_ifs) {
	using namespace std;
	ifs->seekg(0, std::ios::beg);
	nextNWChar();
}

LexicalAnalyser::~LexicalAnalyser() {}

void LexicalAnalyser::nextChar() {
	if (curChar == '\n') {
		line++;
		column = 0;
	}
	curChar = ifs->get();
	column++;
	if (ifs->eof()) {
		curChar = C_END;
	}
}

void LexicalAnalyser::catChar() {
	if (bufInd >= BUFFER_SIZE) {
		tkStr = tkStr + std::string(buffer, BUFFER_SIZE);
		bufInd = 0;
	}
	buffer[bufInd++] = curChar;
}

void LexicalAnalyser::nextNWChar() {
	do {
		nextChar();
	} while (isspace(curChar));
}

std::string LexicalAnalyser::getStr() {
	return tkStr + std::string(buffer, bufInd);
}

void LexicalAnalyser::clearBuf() {
	bufInd = 0;
	tkStr = S_EMPTY;
}