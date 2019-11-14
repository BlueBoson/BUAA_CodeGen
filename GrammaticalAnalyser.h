#pragma once

#include <queue>
#include "LexicalAnalyser.h"
#include "SymbolTable.h"
#include "grammar.h"
#include "Error.h"
#include "MidCode.h"

class GrammaticalAnalyser {
public:
	static GrammaticalAnalyser& getInstance(std::ifstream& _ifs, std::ofstream& _ofs);
	static void deleteInstance();
	void analyse(std::ofstream& mfs, SymbolTable& table);

private:
	static const bool printEnter;
	static const bool printQuit;
	static const bool printPosition;
	static GrammaticalAnalyser* instance;
	std::ifstream* ifs;
	std::ofstream* ofs;
	SymbolTable* symbolTable;
	LexicalAnalyser* lexicalAnalyser;
	std::queue<Token> tkQueue;
	ArgType curFunc;
	bool isRet;
	int lastTkLine;

	GrammaticalAnalyser(std::ifstream& _ifs, std::ofstream& _ofs);
	~GrammaticalAnalyser();
	Token& newTk();
	Token& curTk();
	TkType newType();
	TkType curType();
	std::string curValue();
	Token popTk();
	void nextTk();
	void preReadTk();
	bool assertCurTk(TkType type);
	bool assertNewTk(TkType type);
	void confirmTk(Token tk);
	void enterGrammar(GrammarType gType);
	void confirmGrammar(GrammarType gType);
	void tryPush(IdenType it);
	IdenType useIden();
	void enterFunc(ArgType rtype);
	void checkRet(ArgType rtype);
	void quitFunc();

	void r_program();
	void r_constDescript();
	void r_varDescript();
	void r_retFuncDef();
	void r_voidFuncDef();
	void r_mainFunc();
	void r_constDef();
	void r_varDef();
	Token r_declareHead();
	std::vector<ArgType> r_argList();
	void r_compState();
	int r_integer();
	int r_unsignedInt();
	void r_stateSeq();
	void r_statement();
	void r_condState();
	void r_loopState();
	void r_retFuncCall(std::string retSym = "");
	void r_voidFuncCall();
	void r_assignState();
	void r_readState();
	void r_writeState();
	void r_returnState();
	void r_conditon(std::string label);
	ArgType r_expression(std::string& symbol);
	int r_step();
	std::vector<ArgType> r_valueList();
	ArgType r_item(std::string& symbol);
	ArgType r_factor(std::string& symbol);
	std::string r_string();
};

