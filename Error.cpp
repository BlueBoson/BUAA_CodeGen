
#include "Error.h"

std::ofstream* Error::ofs = NULL;
std::ofstream* Error::efs = NULL;
bool Error::error = false;

void Error::setLogFile(std::ofstream& _ofs) {
	ofs = &_ofs;
}

void Error::cancelLogFile() {
	ofs = NULL;
}

void Error::setErrorFile(std::ofstream& _efs) {
	error = false;
	efs = &_efs;
}

void Error::cancelErrorFile() {
	efs = NULL;
}

void Error::unexpectedToken(Token& get, TkType expected) {
	using namespace std;
	std::string info = "line " + to_string(get.getLine()) + " column " + to_string(get.getColumn()) 
		+ ": expected token type " + Token::getTypeName(expected)
		+ " but got " + Token::getTypeName(get.getType()) + " " + get.getValue();
	if (ofs) {
		*ofs << info << endl;
	} else {
		cout << info << endl;
	}
}

void Error::cannotEndGrammar(Token& near, GrammarType grammar) {
	using namespace std;
	string info = "line " + to_string(near.getLine()) + " column " + to_string(near.getColumn())
		+ " got " + Token::getTypeName(near.getType()) + ": cannot end grammar legally";
	if (ofs) {
		*ofs << info << endl;
	} else {
		cout << info << endl;
	}
}

void Error::raiseError(Token& tk, ErrorType et) {
	raiseError(tk.getLine(), et);
}

void Error::raiseError(int row, ErrorType et) {
	error = true;
	using namespace std;
	string info = to_string(row) + " " + errName[et];
	if (efs) {
		*efs << info << endl;
	} else {
		cout << info << endl;
	}
}

std::map<ErrorType, std::string> getErrName() {
	using namespace std;
	map<ErrorType, string> nameMap;
#define MAP(x,s) nameMap[ErrorType::x] = #s
	MAP(ILLEGAL_TK, a);
	MAP(DUPLICATED_NAME, b);
	MAP(UNDEFINED_NAME, c);
	MAP(UNMATCHED_ARG_NUM, d);
	MAP(UNMATCHED_ARG_TYPE, e);
	MAP(ILLEGAL_CONDITION_TYPE, f);
	MAP(UNMATCHED_VOID_RETURN, g);
	MAP(UNMATCHED_RET_RETURN, h);
	MAP(ILLEGAL_INDEX, i);
	MAP(ASSIGN_CONST, j);
	MAP(UNMATCHED_SEMI, k);
	MAP(UNMATCHED_PAREN, l);
	MAP(UNMATCHED_BRACK, m);
	MAP(MISSING_WHILE, n);
	MAP(ILLEGAL_CONST_DEF, o);
#undef MAP
	return nameMap;
}

bool Error::hasError() {
	return error;
}

std::map<ErrorType, std::string> Error::errName = getErrName();
