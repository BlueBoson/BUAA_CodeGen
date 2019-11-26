#include "GrammaticalAnalyser.h"

const bool GrammaticalAnalyser::printEnter = false;
const bool GrammaticalAnalyser::printQuit = true;
const bool GrammaticalAnalyser::printPosition = false;
GrammaticalAnalyser* GrammaticalAnalyser::instance = NULL;

GrammaticalAnalyser& GrammaticalAnalyser::getInstance(std::ifstream& _ifs, std::ofstream& _ofs) {
	if (instance == NULL) {
		instance = new GrammaticalAnalyser(_ifs, _ofs);
	} else if (instance->ifs != &_ifs || instance->ofs != &_ofs) {
		deleteInstance();
		instance = new GrammaticalAnalyser(_ifs, _ofs);
	}
	return *instance;
}

void GrammaticalAnalyser::deleteInstance() {
	delete instance;
	instance = NULL;
}

void GrammaticalAnalyser::analyse(std::ofstream& mfs, SymbolTable& table) {
	MidCode::init(mfs);
	symbolTable = &table;
	nextTk();
	r_program();
}

GrammaticalAnalyser::GrammaticalAnalyser(std::ifstream& _ifs, std::ofstream& _ofs) {
	ifs = &_ifs;
	ofs = &_ofs;
	curFunc = ArgType::VOID;
	isRet = false;
	lastTkLine = 0;
	symbolTable = nullptr;
	lexicalAnalyser = &LexicalAnalyser::getInstance(_ifs);
}

GrammaticalAnalyser::~GrammaticalAnalyser() {
	LexicalAnalyser::deleteInstance();
}

Token& GrammaticalAnalyser::newTk() {
	return tkQueue.back();
}

Token& GrammaticalAnalyser::curTk() {
	return tkQueue.front();
}

TkType GrammaticalAnalyser::newType() {
	return tkQueue.back().getType();
}

TkType GrammaticalAnalyser::curType() {
	return tkQueue.front().getType();
}

std::string GrammaticalAnalyser::curValue() {
	return tkQueue.front().getValue();
}

Token GrammaticalAnalyser::popTk() {
	Token r = tkQueue.front();
	tkQueue.pop();
	return r;
}

void GrammaticalAnalyser::nextTk() {
	if (!tkQueue.empty()) {
		lastTkLine = tkQueue.front().getLine();
		confirmTk(tkQueue.front());
		tkQueue.pop();
	}
	if (tkQueue.empty()) {
		tkQueue.push(lexicalAnalyser->getNextToken());
	}
	if (curType() == TkType::ILLEGAL) {
		Error::raiseError(curTk(), ErrorType::ILLEGAL_TK);
	}
}

void GrammaticalAnalyser::preReadTk() {
	tkQueue.push(lexicalAnalyser->getNextToken());
}

bool GrammaticalAnalyser::assertCurTk(TkType type) {
	if (curType() == type) {
		nextTk();
		return true;
	} else {
		Error::unexpectedToken(curTk(), type);
		if (type == TkType::RPARENT) {
			Error::raiseError(curTk(), ErrorType::UNMATCHED_PAREN);
		} else if (type == TkType::RBRACK) {
			Error::raiseError(curTk(), ErrorType::UNMATCHED_BRACK);
		} else if (type == TkType::SEMICN) {
			Error::raiseError(lastTkLine, ErrorType::UNMATCHED_SEMI);
		} else {
			nextTk();
		}
		return false;
	}
}

bool GrammaticalAnalyser::assertNewTk(TkType type) {
	if (newType() == type) {
		return true;
	} else {
		Error::unexpectedToken(newTk(), type);
		return false;
	}
}

void GrammaticalAnalyser::confirmTk(Token tk) {
	using namespace std;
	if (printPosition) {
		*ofs << "line" + to_string(tk.getLine()) + " column" + to_string(tk.getColumn()) 
			+ " " + Token::getTypeName(tk.getType()) << " " << tk.getValue() << endl;
	} else {
		*ofs << Token::getTypeName(tk.getType()) << " " << tk.getValue() << endl;
	}
}

void GrammaticalAnalyser::tryPush(IdenType it) {
	if (!symbolTable->push(curTk(), it)) {
		Error::raiseError(curTk(), ErrorType::DUPLICATED_NAME);
	}
	nextTk();
}

IdenType GrammaticalAnalyser::useIden() {
	if (curType() != TkType::IDENFR) {
		Error::unexpectedToken(curTk(), TkType::IDENFR);
		nextTk();
		return IdenType::VAR_INT;
	} else {
		IdenType itype = symbolTable->find(curTk());
		if (itype == IdenType::NONE) {
			Error::raiseError(curTk(), ErrorType::UNDEFINED_NAME);
		}
		nextTk();
		return itype;
	}
}

void GrammaticalAnalyser::enterFunc(ArgType rtype) {
	curFunc = rtype;
	isRet = false;
}

void GrammaticalAnalyser::checkRet(ArgType rtype) {
	isRet = true;
	if (curFunc != rtype) {
		if (curFunc == ArgType::VOID) {
			Error::raiseError(curTk(), ErrorType::UNMATCHED_VOID_RETURN);
		} else {
			Error::raiseError(curTk(), ErrorType::UNMATCHED_RET_RETURN);
		}
	}
}

void GrammaticalAnalyser::quitFunc() {
	if (curFunc != ArgType::VOID && !isRet) {
		Error::raiseError(curTk(), ErrorType::UNMATCHED_RET_RETURN);
	}
}
	
void GrammaticalAnalyser::enterGrammar(GrammarType gType) {
	using namespace std;
	if (!printEnter) {
		return;
	}
	switch (gType) {
	case GrammarType::PROGRAM:
		*ofs << ">>>程序>" << endl;
		break;
	case GrammarType::CONST_DESCRIPT:
		*ofs << ">>>常量说明>" << endl;
		break;
	case GrammarType::VAR_DESCRIPT:
		*ofs << ">>>变量说明>" << endl;
		break;
	case GrammarType::RET_FUNC_DEF:
		*ofs << ">>>有返回值函数定义>" << endl;
		break;
	case GrammarType::VOID_FUNC_DEF:
		*ofs << ">>>无返回值函数定义>" << endl;
		break;
	case GrammarType::MAIN_FUNC:
		*ofs << ">>>主函数>" << endl;
		break;
	case GrammarType::CONST_DEF:
		*ofs << ">>>常量定义>" << endl;
		break;
	case GrammarType::VAR_DEF:
		*ofs << ">>>变量定义>" << endl;
		break;
	case GrammarType::DECLARE_HEAD:
		*ofs << ">>>声明头部>" << endl;
		break;
	case GrammarType::ARG_LIST:
		*ofs << ">>>参数表>" << endl;
		break;
	case GrammarType::COMP_STATE:
		*ofs << ">>>复合语句>" << endl;
		break;
	case GrammarType::INTEGER:
		*ofs << ">>>整数>" << endl;
		break;
	case GrammarType::UNSIGNED_INT:
		*ofs << ">>>无符号整数>" << endl;
		break;
	case GrammarType::STATE_SEQ:
		*ofs << ">>>语句列>" << endl;
		break;
	case GrammarType::STATEMENT:
		*ofs << ">>>语句>" << endl;
		break;
	case GrammarType::COND_STATE:
		*ofs << ">>>条件语句>" << endl;
		break;
	case GrammarType::LOOP_STATE:
		*ofs << ">>>循环语句>" << endl;
		break;
	case GrammarType::RET_FUNC_CALL:
		*ofs << ">>>有返回值函数调用语句>" << endl;
		break;
	case GrammarType::VOID_FUNC_CALL:
		*ofs << ">>>无返回值函数调用语句>" << endl;
		break;
	case GrammarType::ASSIGN_STATE:
		*ofs << ">>>赋值语句>" << endl;
		break;
	case GrammarType::READ_STATE:
		*ofs << ">>>读语句>" << endl;
		break;
	case GrammarType::WRITE_STATE:
		*ofs << ">>>写语句>" << endl;
		break;
	case GrammarType::RETURN_STATE:
		*ofs << ">>>返回语句>" << endl;
		break;
	case GrammarType::CONDITION:
		*ofs << ">>>条件>" << endl;
		break;
	case GrammarType::EXPRESSION:
		*ofs << ">>>表达式>" << endl;
		break;
	case GrammarType::STEP:
		*ofs << ">>>步长>" << endl;
		break;
	case GrammarType::VALUE_LIST:
		*ofs << ">>>值参数表>" << endl;
		break;
	case GrammarType::ITEM:
		*ofs << ">>>项>" << endl;
		break;
	case GrammarType::FACTOR:
		*ofs << ">>>因子>" << endl;
		break;
	case GrammarType::STRING:
		*ofs << ">>>字符串>" << endl;
		break;
	default:
		*ofs << ">>>未知文法>" << endl;
		break;
	}
}

void GrammaticalAnalyser::confirmGrammar(GrammarType gType) {
	using namespace std;
	if (!printQuit) {
		return;
	}
	switch (gType) {
	case GrammarType::PROGRAM:
		*ofs << "<程序>" << endl;
		break;
	case GrammarType::CONST_DESCRIPT:
		*ofs << "<常量说明>" << endl;
		break;
	case GrammarType::VAR_DESCRIPT:
		*ofs << "<变量说明>" << endl;
		break;
	case GrammarType::RET_FUNC_DEF:
		*ofs << "<有返回值函数定义>" << endl;
		break;
	case GrammarType::VOID_FUNC_DEF:
		*ofs << "<无返回值函数定义>" << endl;
		break;
	case GrammarType::MAIN_FUNC:
		*ofs << "<主函数>" << endl;
		break;
	case GrammarType::CONST_DEF:
		*ofs << "<常量定义>" << endl;
		break;
	case GrammarType::VAR_DEF:
		*ofs << "<变量定义>" << endl;
		break;
	case GrammarType::DECLARE_HEAD:
		*ofs << "<声明头部>" << endl;
		break;
	case GrammarType::ARG_LIST:
		*ofs << "<参数表>" << endl;
		break;
	case GrammarType::COMP_STATE:
		*ofs << "<复合语句>" << endl;
		break;
	case GrammarType::INTEGER:
		*ofs << "<整数>" << endl;
		break;
	case GrammarType::UNSIGNED_INT:
		*ofs << "<无符号整数>" << endl;
		break;
	case GrammarType::STATE_SEQ:
		*ofs << "<语句列>" << endl;
		break;
	case GrammarType::STATEMENT:
		*ofs << "<语句>" << endl;
		break;
	case GrammarType::COND_STATE:
		*ofs << "<条件语句>" << endl;
		break;
	case GrammarType::LOOP_STATE:
		*ofs << "<循环语句>" << endl;
		break;
	case GrammarType::RET_FUNC_CALL:
		*ofs << "<有返回值函数调用语句>" << endl;
		break;
	case GrammarType::VOID_FUNC_CALL:
		*ofs << "<无返回值函数调用语句>" << endl;
		break;
	case GrammarType::ASSIGN_STATE:
		*ofs << "<赋值语句>" << endl;
		break;
	case GrammarType::READ_STATE:
		*ofs << "<读语句>" << endl;
		break;
	case GrammarType::WRITE_STATE:
		*ofs << "<写语句>" << endl;
		break;
	case GrammarType::RETURN_STATE:
		*ofs << "<返回语句>" << endl;
		break;
	case GrammarType::CONDITION:
		*ofs << "<条件>" << endl;
		break;
	case GrammarType::EXPRESSION:
		*ofs << "<表达式>" << endl;
		break;
	case GrammarType::STEP:
		*ofs << "<步长>" << endl;
		break;
	case GrammarType::VALUE_LIST:
		*ofs << "<值参数表>" << endl;
		break;
	case GrammarType::ITEM:
		*ofs << "<项>" << endl;
		break;
	case GrammarType::FACTOR:
		*ofs << "<因子>" << endl;
		break;
	case GrammarType::STRING:
		*ofs << "<字符串>" << endl;
		break;
	default:
		*ofs << "<未知文法>" << endl;
		break;
	}
}

void GrammaticalAnalyser::r_program() {
	enterGrammar(GrammarType::PROGRAM);
	if (curType() == TkType::CONSTTK) {
		r_constDescript();
	}
	if (curType() == TkType::INTTK || curType() == TkType::CHARTK) {
		preReadTk();
		assertNewTk(TkType::IDENFR);
		preReadTk();
		if (newTk().getType() != TkType::LPARENT) {
			r_varDescript();
		}
	}
	for (TkType type = curType(); type == TkType::INTTK || type == TkType::CHARTK || type == TkType::VOIDTK; type = curType()) {
		if (type == TkType::VOIDTK) {
			preReadTk();
			if (newType() == TkType::MAINTK) {
				break;
			} else {
				r_voidFuncDef();
			}
		} else {
			r_retFuncDef();
		}
	}
	r_mainFunc();
	confirmGrammar(GrammarType::PROGRAM);
}

void GrammaticalAnalyser::r_constDescript() {
	enterGrammar(GrammarType::CONST_DESCRIPT);
	do {
		assertCurTk(TkType::CONSTTK);
		r_constDef();
		assertCurTk(TkType::SEMICN);
	} while (curType() == TkType::CONSTTK);
	confirmGrammar(GrammarType::CONST_DESCRIPT);
}

void GrammaticalAnalyser::r_varDescript() {
	enterGrammar(GrammarType::VAR_DESCRIPT);
	do {
		r_varDef();
		assertCurTk(TkType::SEMICN);
		if (curType() != TkType::INTTK && curType() != TkType::CHARTK) {
			break;
		}
		preReadTk();
		if (newType() != TkType::IDENFR) {
			break;
		}
		preReadTk();
	} while (newType() == TkType::LBRACK || newType() == TkType::COMMA || newType() == TkType::SEMICN);
	confirmGrammar(GrammarType::VAR_DESCRIPT);
}

void GrammaticalAnalyser::r_retFuncDef() {
	enterGrammar(GrammarType::RET_FUNC_DEF);
	Token idTk = r_declareHead();
	symbolTable->newSub(idTk.getValue());
	assertCurTk(TkType::LPARENT);
	auto argList = r_argList();
	symbolTable->addArgs(idTk, argList);
	assertCurTk(TkType::RPARENT);
	assertCurTk(TkType::LBRACE);
	r_compState();
	symbolTable->exitSub();
	quitFunc();
	MidCode(MidType::END_FUNC, idTk.getValue()).emit();
	assertCurTk(TkType::RBRACE);
	confirmGrammar(GrammarType::RET_FUNC_DEF);
}

void GrammaticalAnalyser::r_voidFuncDef() {
	enterGrammar(GrammarType::VOID_FUNC_DEF);
	assertCurTk(TkType::VOIDTK);
	Token idTk = curTk();
	if (curType() == TkType::IDENFR) {
		Token func = curTk();
		tryPush(IdenType::VOID_FUNC);
	} else {
		Error::unexpectedToken(curTk(), TkType::IDENFR);
		nextTk();
	}
	MidCode(MidType::FUNC, "void", idTk.getValue()).emit();
	symbolTable->newSub(idTk.getValue());
	enterFunc(ArgType::VOID);
	assertCurTk(TkType::LPARENT);
	auto argList = r_argList();
	symbolTable->addArgs(idTk, argList);
	assertCurTk(TkType::RPARENT);
	assertCurTk(TkType::LBRACE);
	r_compState();
	assertCurTk(TkType::RBRACE);
	symbolTable->exitSub();
	MidCode(MidType::END_FUNC, idTk.getValue()).emit();
	confirmGrammar(GrammarType::VOID_FUNC_DEF);
}

void GrammaticalAnalyser::r_mainFunc() {
	enterGrammar(GrammarType::MAIN_FUNC);
	symbolTable->newSub("main");
	enterFunc(ArgType::VOID);
	assertCurTk(TkType::VOIDTK);
	assertCurTk(TkType::MAINTK);
	MidCode(MidType::FUNC, "void", "main").emit();
	assertCurTk(TkType::LPARENT);
	assertCurTk(TkType::RPARENT);
	assertCurTk(TkType::LBRACE);
	r_compState();
	assertCurTk(TkType::RBRACE);
	symbolTable->exitSub();
	MidCode(MidType::END_FUNC, "main").emit();
	confirmGrammar(GrammarType::MAIN_FUNC);
}

void GrammaticalAnalyser::r_constDef() {
	enterGrammar(GrammarType::CONST_DEF);
	IdenType type;
	MidCode code(MidType::CONST);
	code.setOp1(curValue());
	if (curType() == TkType::INTTK) {
		type = IdenType::CONST_INT;
	} else if (curType() == TkType::CHARTK) {
		type = IdenType::CONST_CHAR;
	} else {
		Error::cannotEndGrammar(curTk(), GrammarType::CONST_DEF);
		nextTk();
		return;
	}
	nextTk();
	if (curType() != TkType::IDENFR) {
		Error::unexpectedToken(curTk(), TkType::IDENFR);
		nextTk();
	}
	do {
		if (curType() == TkType::COMMA) {
			nextTk();
		}
		if (curType() == TkType::IDENFR) {
			code.setResOp(curValue());
			tryPush(type);
		} else {
			Error::unexpectedToken(curTk(), TkType::IDENFR);
			nextTk();
		}
		assertCurTk(TkType::ASSIGN);
		if (curType() == TkType::INTCON || curType() == TkType::PLUS || curType() == TkType::MINU) {
			if (type != IdenType::CONST_INT) {
				Error::raiseError(curTk(), ErrorType::ILLEGAL_CONST_DEF);
			}
			code.setOp2(std::to_string(r_integer()));
			code.emit();
		} else if (curType() == TkType::CHARCON) {
			if (type != IdenType::CONST_CHAR) {
				Error::raiseError(curTk(), ErrorType::ILLEGAL_CONST_DEF);
			}
			code.setOp2(std::string({ C_SQUOTE }) + curValue() + std::string({ C_SQUOTE }));
			code.emit();
			assertCurTk(TkType::CHARCON);
		} else {
			Error::raiseError(curTk(), ErrorType::ILLEGAL_CONST_DEF);
			while (curType() != TkType::COMMA && curType() != TkType::SEMICN && curType() != TkType::EMPTY) {
				nextTk();
			}
		}
	} while (curType() == TkType::COMMA);
	confirmGrammar(GrammarType::CONST_DEF);
}

void GrammaticalAnalyser::r_varDef() {
	enterGrammar(GrammarType::VAR_DEF);
	IdenType type;
	MidCode code(MidType::VAR);
	code.setOp1(curValue());
	if (curType() == TkType::INTTK) {
		type = IdenType::VAR_INT;
	} else if (curType() == TkType::CHARTK) {
		type = IdenType::VAR_CHAR;
	} else {
		Error::cannotEndGrammar(curTk(), GrammarType::VAR_DEF);
		nextTk();
		return;
	}
	nextTk();
	if (curType() != TkType::IDENFR) {
		Error::unexpectedToken(curTk(), TkType::IDENFR);
		nextTk();
	}
	do {
		if (curType() == TkType::COMMA) {
			nextTk();
		}
		Token idTk = curTk();
		if (curType() == TkType::IDENFR) {
			code.setResOp(curValue());
			tryPush(type);
		} else {
			Error::unexpectedToken(curTk(), TkType::IDENFR);
			nextTk();
		}
		if (curType() == TkType::LBRACK) {
			nextTk();
			int iDim = r_unsignedInt();
			std::string dim = std::to_string(iDim);
			code.setOp2(dim);
			symbolTable->addDim(idTk, iDim);
			assertCurTk(TkType::RBRACK);
		}
		code.emit();
	} while (curType() == TkType::COMMA);
	confirmGrammar(GrammarType::VAR_DEF);
}

Token GrammaticalAnalyser::r_declareHead() {
	enterGrammar(GrammarType::DECLARE_HEAD);
	IdenType type;
	MidCode code(MidType::FUNC);
	code.setOp1(curValue());
	if (curType() == TkType::INTTK) {
		type = IdenType::INT_FUNC;
		enterFunc(ArgType::INT);
	} else if (curType() == TkType::CHARTK) {
		type = IdenType::CHAR_FUNC;
		enterFunc(ArgType::CHAR);
	} else {
		Error::cannotEndGrammar(curTk(), GrammarType::DECLARE_HEAD);
		nextTk();
		return curTk();
	}
	nextTk();
	Token idTk = curTk();
	if (curType() == TkType::IDENFR) {
		code.setOp2(curValue());
		code.emit();
		tryPush(type);
	} else {
		Error::unexpectedToken(curTk(), TkType::IDENFR);
		nextTk();
	}
	confirmGrammar(GrammarType::DECLARE_HEAD);
	return idTk;
}

std::vector<ArgType> GrammaticalAnalyser::r_argList() {
	enterGrammar(GrammarType::ARG_LIST);
	std::vector<ArgType> args;
	MidCode code(MidType::PARAM);
	if (curType() == TkType::INTTK || curType() == TkType::CHARTK) {
		do {
			if (curType() == TkType::COMMA) {
				nextTk();
			}
			code.setOp1(curValue());
			if (curType() != TkType::INTTK && curType() != TkType::CHARTK) {
				Error::cannotEndGrammar(curTk(), GrammarType::ARG_LIST);
				nextTk();
				return args;
			}
			IdenType itype;
			if (curType() == TkType::INTTK) {
				args.push_back(ArgType::INT);
				itype = IdenType::VAR_INT;
			} else {
				args.push_back(ArgType::CHAR);
				itype = IdenType::VAR_CHAR;
			}
			nextTk();
			if (curType() == TkType::IDENFR) {
				code.setResOp(curValue());
				code.emit();
				tryPush(itype);
			} else {
				Error::unexpectedToken(curTk(), TkType::IDENFR);
				nextTk();
			}
		} while (curType() == TkType::COMMA);
	}
	confirmGrammar(GrammarType::ARG_LIST);
	return args;
}

void GrammaticalAnalyser::r_compState() {
	enterGrammar(GrammarType::COMP_STATE);
	if (curType() == TkType::CONSTTK) {
		r_constDescript();
	}
	if (curType() == TkType::INTTK || curType() == TkType::CHARTK) {
		r_varDescript();
	}
	r_stateSeq();
	confirmGrammar(GrammarType::COMP_STATE);
}

int GrammaticalAnalyser::r_integer() { 
	enterGrammar(GrammarType::INTEGER);
	int symbol = 1;
	if (curType() == TkType::PLUS) {
		nextTk();
	} else if (curType() == TkType::MINU) {
		symbol = -1;
		nextTk();
	}
	int value = symbol * r_unsignedInt();
	confirmGrammar(GrammarType::INTEGER);
	return value;
}

int GrammaticalAnalyser::r_unsignedInt() {
	enterGrammar(GrammarType::UNSIGNED_INT);
	int value = 0;
	if (curType() == TkType::INTCON) {
		value = std::stoi(curValue());
		nextTk();
	} else {
		Error::unexpectedToken(curTk(), TkType::INTCON);
		nextTk();
	}
	confirmGrammar(GrammarType::UNSIGNED_INT);
	return value;
}

void GrammaticalAnalyser::r_stateSeq() {
	enterGrammar(GrammarType::STATE_SEQ);
	while (curType() != TkType::RBRACE && curType() != TkType::EMPTY) {
		r_statement();
	}
	confirmGrammar(GrammarType::STATE_SEQ);
}

void GrammaticalAnalyser::r_statement() {
	enterGrammar(GrammarType::STATEMENT);
	TkType type = curType();
	if (type == TkType::IFTK) {
		r_condState();
	} else if (type == TkType::WHILETK || type == TkType::DOTK || type == TkType::FORTK) {
		r_loopState();
	} else if (type == TkType::LBRACE) {
		nextTk();
		r_stateSeq();
		assertCurTk(TkType::RBRACE);
	} else if (type == TkType::IDENFR) {
		IdenType iType = symbolTable->find(curTk());
		preReadTk();
		if (newType() == TkType::LPARENT) {
			if (iType == IdenType::INT_FUNC || iType == IdenType::CHAR_FUNC) {
				r_retFuncCall();
			} else {
				r_voidFuncCall();
			}
		} else if (newType() == TkType::ASSIGN || newType() == TkType::LBRACK) {
			r_assignState();
		} else {
			Error::cannotEndGrammar(curTk(), GrammarType::STATEMENT);
			nextTk();
		}
		assertCurTk(TkType::SEMICN);
	} else if (type == TkType::PRINTFTK) {
		r_writeState();
		assertCurTk(TkType::SEMICN);
	} else if (type == TkType::SCANFTK) {
		r_readState();
		assertCurTk(TkType::SEMICN);
	} else if (type == TkType::RETURNTK) {
		r_returnState();
		assertCurTk(TkType::SEMICN);
	} else if (type == TkType::SEMICN) {
		nextTk();
	} else {
		Error::cannotEndGrammar(curTk(), GrammarType::STATEMENT);
		nextTk();
		return;
	}
	confirmGrammar(GrammarType::STATEMENT);
}

void GrammaticalAnalyser::r_condState() {
	enterGrammar(GrammarType::COND_STATE);
	assertCurTk(TkType::IFTK);
	assertCurTk(TkType::LPARENT);
	std::string elseLabel = MidCode::genLb();
	r_conditon(elseLabel);
	assertCurTk(TkType::RPARENT);
	r_statement();
	if (curType() == TkType::ELSETK) {
		std::string endLabel = MidCode::genLb();
		MidCode(MidType::JMP, endLabel).emit();
		MidCode(MidType::LABEL, elseLabel).emit();
		nextTk();
		r_statement();
		MidCode(MidType::LABEL, endLabel).emit();
	} else {
		MidCode(MidType::LABEL, elseLabel).emit();
	}
	confirmGrammar(GrammarType::COND_STATE);
}

void GrammaticalAnalyser::r_loopState() {
	enterGrammar(GrammarType::LOOP_STATE);
	std::string startLabel = MidCode::genLb();
	std::string endLabel = MidCode::genLb();
	if (curType() == TkType::WHILETK) {
		nextTk();
		assertCurTk(TkType::LPARENT);
		MidCode(MidType::LABEL, startLabel).emit();
		r_conditon(endLabel);
		assertCurTk(TkType::RPARENT);
		r_statement();
		MidCode(MidType::JMP, startLabel).emit();
		MidCode(MidType::LABEL, endLabel).emit();
	} else if (curType() == TkType::DOTK) {
		nextTk();
		MidCode(MidType::LABEL, startLabel).emit();
		r_statement();
		if (curType() != TkType::WHILETK) {
			Error::raiseError(lastTkLine, ErrorType::MISSING_WHILE);
			return;
		}
		assertCurTk(TkType::WHILETK);
		assertCurTk(TkType::LPARENT);
		r_conditon(endLabel);
		MidCode(MidType::JMP, startLabel).emit();
		MidCode(MidType::LABEL, endLabel).emit();
		assertCurTk(TkType::RPARENT);
	} else if (curType() == TkType::FORTK) {
		nextTk();
		assertCurTk(TkType::LPARENT);
		std::string symbol = curValue();
		IdenType itype = useIden();
		if (itype == IdenType::CONST_CHAR || itype == IdenType::CONST_INT) {
			Error::raiseError(curTk(), ErrorType::ASSIGN_CONST);
		}
		assertCurTk(TkType::ASSIGN);
		r_expression(symbol);
		assertCurTk(TkType::SEMICN);
		MidCode(MidType::LABEL, startLabel).emit();
		r_conditon(endLabel);
		assertCurTk(TkType::SEMICN);
		std::string lsymbol = curValue();
		itype = useIden();
		if (itype == IdenType::CONST_CHAR || itype == IdenType::CONST_INT) {
			Error::raiseError(curTk(), ErrorType::ASSIGN_CONST);
		}
		assertCurTk(TkType::ASSIGN);
		std::string rsymbol = curValue();
		useIden();
		MidType mtype = MidType::NONE;
		if (curType() == TkType::PLUS) {
			mtype = MidType::PLUS;
		} else if (curType() == TkType::MINU) {
			mtype = MidType::MINU;
		} else {
			Error::cannotEndGrammar(curTk(), GrammarType::LOOP_STATE);
			nextTk();
			return;
		}
		nextTk();
		int value = r_step();
		assertCurTk(TkType::RPARENT);
		r_statement();
		std::string tv = MidCode::genTv();
		MidCode(MidType::ASSIGN, tv, std::to_string(value), "").emit();
		MidCode(mtype, lsymbol, rsymbol, tv).emit();
		MidCode(MidType::JMP, startLabel).emit();
		MidCode(MidType::LABEL, endLabel).emit();
	} else {
		Error::cannotEndGrammar(curTk(), GrammarType::LOOP_STATE);
		nextTk();
		return;
	}
	confirmGrammar(GrammarType::LOOP_STATE);
}

void GrammaticalAnalyser::r_retFuncCall(std::string retSym) {
	enterGrammar(GrammarType::RET_FUNC_CALL);
	auto expArgs = symbolTable->getArgs(curTk());
	std::string symbol = curValue();
	useIden();
	assertCurTk(TkType::LPARENT);
	auto gotArgs = r_valueList();
	if (expArgs.size() != gotArgs.size()) {
		Error::raiseError(curTk(), ErrorType::UNMATCHED_ARG_NUM);
	} else if (expArgs != gotArgs) {
		Error::raiseError(curTk(), ErrorType::UNMATCHED_ARG_TYPE);
	}
	assertCurTk(TkType::RPARENT);
	MidCode(MidType::CALL, symbol).emit();
	if (!retSym.empty()) {
		MidCode(MidType::ASSIGN, retSym, RET_TV, "").emit();
	}
	confirmGrammar(GrammarType::RET_FUNC_CALL);
}

void GrammaticalAnalyser::r_voidFuncCall() {
	enterGrammar(GrammarType::VOID_FUNC_CALL);
	auto expArgs = symbolTable->getArgs(curTk());
	std::string symbol = curValue();
	useIden();
	assertCurTk(TkType::LPARENT);
	auto gotArgs = r_valueList();
	if (expArgs.size() != gotArgs.size()) {
		Error::raiseError(curTk(), ErrorType::UNMATCHED_ARG_NUM);
	} else if (expArgs != gotArgs) {
		Error::raiseError(curTk(), ErrorType::UNMATCHED_ARG_TYPE);
	}
	assertCurTk(TkType::RPARENT);
	MidCode(MidType::CALL, symbol).emit();
	confirmGrammar(GrammarType::VOID_FUNC_CALL);
}

void GrammaticalAnalyser::r_assignState() {
	enterGrammar(GrammarType::ASSIGN_STATE);
	std::string symbol = curValue();
	IdenType itype = useIden();
	if (itype == IdenType::CONST_CHAR || itype == IdenType::CONST_INT) {
		Error::raiseError(curTk(), ErrorType::ASSIGN_CONST);
	}
	std::string index = "";
	if (curType() == TkType::LBRACK) {
		nextTk();
		index = MidCode::genTv();
		if (r_expression(index) != ArgType::INT) {
			Error::raiseError(curTk(), ErrorType::ILLEGAL_INDEX);
		}
		assertCurTk(TkType::RBRACK);
	}
	assertCurTk(TkType::ASSIGN);
	if (!index.empty()) {
		std::string rsymbol = MidCode::genTv();
		r_expression(rsymbol);
		MidCode(MidType::LARRAY_ASSIGN, symbol, index, rsymbol).emit();
	} else {
		r_expression(symbol);
	}
	confirmGrammar(GrammarType::ASSIGN_STATE);
}

void GrammaticalAnalyser::r_readState() {
	enterGrammar(GrammarType::READ_STATE);
	assertCurTk(TkType::SCANFTK);
	assertCurTk(TkType::LPARENT);
	do {
		if (curType() == TkType::COMMA) {
			nextTk();
		}
		MidCode mid(MidType::READ, curValue());
		IdenType type = useIden();
		if (type == IdenType::VAR_INT) {
			mid.setOp1(S_INT);
		} else if (type == IdenType::VAR_CHAR) {
			mid.setOp2(S_CHAR);
		}
		mid.emit();
	} while (curType() == TkType::COMMA);
	assertCurTk(TkType::RPARENT);
	confirmGrammar(GrammarType::READ_STATE);
}

void GrammaticalAnalyser::r_writeState() {
	enterGrammar(GrammarType::WRITE_STATE);
	assertCurTk(TkType::PRINTFTK);
	assertCurTk(TkType::LPARENT);
	MidCode code(MidType::WRITE);
	if (curType() == TkType::STRCON) {
		std::string value = r_string();
		code.setOp1(std::string({ C_DQUOTE }) + value + std::string({ C_DQUOTE }));
		if (curType() == TkType::COMMA) {
			nextTk();
			std::string symbol = MidCode::genTv();
			ArgType type = r_expression(symbol);
			if (type == ArgType::INT) {
				code.setResOp(S_INT);
			} else if (type == ArgType::CHAR) {
				code.setResOp(S_CHAR);
			}
			code.setOp2(symbol);
		}
		code.emit();
	} else {
		std::string symbol = MidCode::genTv();
		ArgType type = r_expression(symbol);
		if (type == ArgType::INT) {
			code.setResOp(S_INT);
		} else if (type == ArgType::CHAR) {
			code.setResOp(S_CHAR);
		}
		code.setOp2(symbol);
		code.emit();
	}
	assertCurTk(TkType::RPARENT);
	confirmGrammar(GrammarType::WRITE_STATE);
}

void GrammaticalAnalyser::r_returnState() {
	enterGrammar(GrammarType::RETURN_STATE);
	assertCurTk(TkType::RETURNTK);
	ArgType rtype = ArgType::VOID;
	std::string symbol = "";
	if (curType() == TkType::LPARENT) {
		nextTk();
		symbol = RET_TV;
		rtype = r_expression(symbol);
		assertCurTk(TkType::RPARENT);
	}
	checkRet(rtype);
	MidCode(MidType::RETURN, symbol).emit();
	confirmGrammar(GrammarType::RETURN_STATE);
}

void GrammaticalAnalyser::r_conditon(std::string label) {
	enterGrammar(GrammarType::CONDITION);
	std::string sym1 = MidCode::genTv();
	MidType mtype = MidType::NONE;
	bool legal = r_expression(sym1) == ArgType::INT;
	TkType type = curType();
	if (type == TkType::LSS || type == TkType::LEQ || type == TkType::GRE || type == TkType::GEQ
		|| type == TkType::NEQ || type == TkType::EQL) {
		if (type == TkType::LSS) {
			mtype = MidType::BGEZ;
		} else if (type == TkType::LEQ) {
			mtype = MidType::BGTZ;
		} else if (type == TkType::GRE) {
			mtype = MidType::BLEZ;
		} else if (type == TkType::GEQ) {
			mtype = MidType::BLTZ;
		} else if (type == TkType::NEQ) {
			mtype = MidType::BEQ;
		} else if (type == TkType::EQL) {
			mtype = MidType::BNE;
		}
		nextTk();
		std::string sym2 = MidCode::genTv();
		legal &= r_expression(sym2) == ArgType::INT;
		if (type == TkType::NEQ || type == TkType::EQL) {
			MidCode(mtype, label, sym1, sym2).emit();
		} else {
			std::string sym3 = MidCode::genTv();
			MidCode(MidType::MINU, sym3, sym1, sym2).emit();
			MidCode(mtype, label, sym3, "").emit();
		}
	} else {
		MidCode(MidType::BEQ, label, sym1, ZERO_TV).emit();
	}
	if (!legal) {
		Error::raiseError(curTk(), ErrorType::ILLEGAL_CONDITION_TYPE);
	}
	confirmGrammar(GrammarType::CONDITION);
}

ArgType GrammaticalAnalyser::r_expression(std::string& symbol) {
	if (curType() == TkType::EMPTY) {
		return ArgType::INT;
	}
	enterGrammar(GrammarType::EXPRESSION);
	ArgType atype = ArgType::CHAR;
	bool neg = false;
	if (curType() == TkType::PLUS || curType() == TkType::MINU) {
		neg = curType() == TkType::MINU;
		atype = ArgType::INT;
		nextTk();
	}
	std::string otv = MidCode::genTv();
	std::string ntv;
	if (r_item(otv) == ArgType::INT) {
		atype = ArgType::INT;
	}
	if (neg) {
		ntv = MidCode::genTv();
		MidCode(MidType::MINU, ntv, ZERO_TV, otv).emit();
		otv = ntv;
	}
	MidType mtype = MidType::NONE;
 	while (curType() == TkType::PLUS || curType() == TkType::MINU) {
		mtype = (curType() == TkType::PLUS) ? MidType::PLUS : MidType::MINU;
		atype = ArgType::INT;
		nextTk();
		ntv = MidCode::genTv();
		r_item(ntv);
		std::string ts = MidCode::genTv();
		MidCode(mtype, ts, otv, ntv).emit();
		otv = ts;
	}
	MidCode(MidType::ASSIGN, symbol, otv, "").emit();
	confirmGrammar(GrammarType::EXPRESSION);
	return atype;
}

int GrammaticalAnalyser::r_step() {
	enterGrammar(GrammarType::STEP);
	int value = r_unsignedInt();
	confirmGrammar(GrammarType::STEP);
	return value;
}

std::vector<ArgType> GrammaticalAnalyser::r_valueList() {
	enterGrammar(GrammarType::VALUE_LIST);
	std::vector<ArgType> args;
	std::vector<MidCode> mids;
	if (curType() != TkType::RPARENT) {
		std::string symbol = MidCode::genTv();
		args.push_back(r_expression(symbol));
		mids.push_back(MidCode(MidType::PUSH, symbol));
		while (curType() == TkType::COMMA) {
			nextTk();
			symbol = MidCode::genTv();
			args.push_back(r_expression(symbol));
			mids.push_back(MidCode(MidType::PUSH, symbol));
		}
	}
	for (auto mid : mids) {
		mid.emit();
	}
	confirmGrammar(GrammarType::VALUE_LIST);
	return args;
}

ArgType GrammaticalAnalyser::r_item(std::string& symbol) {
	if (curType() == TkType::EMPTY) {
		return ArgType::INT;
	}
	enterGrammar(GrammarType::ITEM);
	std::string otv = MidCode::genTv();
	ArgType atype = r_factor(otv);
	std::string ntv;
	while (curType() == TkType::MULT || curType() == TkType::DIV) {
		MidType mtype = (curType() == TkType::MULT) ? MidType::MUL : MidType::DIV;
		atype = ArgType::INT;
		nextTk();
		ntv = MidCode::genTv();
		r_factor(ntv);
		std::string ts = MidCode::genTv();
		MidCode(mtype, ts, otv, ntv).emit();
		otv = ts;
	}
	MidCode(MidType::ASSIGN, symbol, otv, "").emit();
	confirmGrammar(GrammarType::ITEM);
	return atype;
}

ArgType GrammaticalAnalyser::r_factor(std::string& symbol) {
	if (curType() == TkType::EMPTY) {
		return ArgType::INT;
	}
	enterGrammar(GrammarType::FACTOR);
	TkType type = curType();
	bool isInt = true;
	if (type == TkType::IDENFR) {
		IdenType itype = symbolTable->find(curTk());
		preReadTk();
		if (itype == IdenType::CHAR_FUNC || itype == IdenType::VAR_CHAR || itype == IdenType::CONST_CHAR) {
			isInt = false;
		}
		if (newType() == TkType::LPARENT) {
			r_retFuncCall(symbol);
		} else {
			std::string name = curValue();
			useIden();
			if (curType() == TkType::LBRACK) {
				std::string index = MidCode::genTv();
				nextTk();
				if (r_expression(index) != ArgType::INT) {
					Error::raiseError(curTk(), ErrorType::ILLEGAL_INDEX);
				}
				assertCurTk(TkType::RBRACK);
				MidCode(MidType::RARRAY_ASSIGN, symbol, name, index).emit();
			} else {
				MidCode(MidType::ASSIGN, symbol, name, "").emit();
			}
		}
	} else if (type == TkType::LPARENT) {
		nextTk();
		r_expression(symbol);
		assertCurTk(TkType::RPARENT);
	} else if (type == TkType::PLUS || type == TkType::MINU || type == TkType::INTCON) {
		int value = r_integer();
		MidCode(MidType::ASSIGN, symbol, std::to_string(value), "").emit();
	} else if (type == TkType::CHARCON) {
		isInt = false;
		MidCode(MidType::ASSIGN, symbol, std::string({ C_SQUOTE }) + curValue() + std::string({ C_SQUOTE }), "").emit();
		nextTk();
	} else {
		Error::cannotEndGrammar(curTk(), GrammarType::FACTOR);
		nextTk();
		return ArgType::INT;
	}
	confirmGrammar(GrammarType::FACTOR);
	return isInt ? ArgType::INT : ArgType::CHAR;
}

std::string GrammaticalAnalyser::r_string() {
	enterGrammar(GrammarType::STRING);
	std::string value = curValue();
	symbolTable->addStr(value);
	assertCurTk(TkType::STRCON);
	confirmGrammar(GrammarType::STRING);
	return value;
}