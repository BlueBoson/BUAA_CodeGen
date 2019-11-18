#include "SymbolTable.h"

SymbolTable* SymbolTable::instance = NULL;

SymbolTable& SymbolTable::getInstance() {
	if (instance == NULL) {
		instance = new SymbolTable;
	}
	return *instance;
}

SymbolTable::SymbolTable() {
	strCount = 0;
}

void SymbolTable::deleteInstance() {
	delete instance;
	instance = NULL;
}

bool SymbolTable::push(Token& tk, IdenType type) {
	auto symName = tk.getValue();
	if (curName.empty()) {
		if (funcs.find(symName) != funcs.end() || globalVars.find(symName) != globalVars.end()) {
			return false;
		}
		if (type == IdenType::CHAR_FUNC || type == IdenType::INT_FUNC || type == IdenType::VOID_FUNC) {
			std::vector<ArgType> initArgs;
			funcs[symName] = { type, initArgs };
		} else {
			if (symName == "main") {
				return false;
			}
			globalVars[symName] = { type, 0 };
		}
	} else {
		if (curVars.find(symName) != curVars.end()) {
			return false;
		}
		curVars[symName] = { type, 0 };
	}
	return true;
}

IdenType SymbolTable::find(Token& tk) {
	auto symName = tk.getValue();
	if (!curName.empty()) {
		auto iter = curVars.find(symName);
		if (iter != curVars.end()) {
			return iter->second.type;
		}
	}
	auto vIter = globalVars.find(symName);
	if (vIter != globalVars.end()) {
		return vIter->second.type;
	}
	auto fIter = funcs.find(symName);
	if (fIter != funcs.end()) {
		return fIter->second.type;
	}
	return IdenType::NONE;
}

void SymbolTable::addArgs(Token& tk, std::vector<ArgType>& args) {
	funcs[tk.getValue()].args = args;
}

void SymbolTable::addDim(Token& tk, int dim) {
	if (curName.empty()) {
		globalVars[tk.getValue()].dim = dim;
	} else {
		curVars[tk.getValue()].dim = dim;
	}
}

std::vector<ArgType> SymbolTable::getArgs(Token& tk) {
	std::vector<ArgType> r;
	auto iter = funcs.find(tk.getValue());
	if (iter == funcs.end()) {
		return r;
	} else {
		return iter->second.args;
	}
}

void SymbolTable::newSub(std::string funcName) {
	curName = funcName;
}

void SymbolTable::exitSub() {
	localVars[curName] = curVars;
	curVars.clear();
	curName = "";
}

std::map<const std::string, varInfo> SymbolTable::getGlobals() {
	return globalVars;
}

std::map<const std::string, varInfo> SymbolTable::getLocals(std::string funcName) {
	std::map<const std::string, varInfo> nmap;
	auto iter = localVars.find(funcName);
	if (iter != localVars.end()) {
		return iter->second;
	} else {
		return nmap;
	}
}

void SymbolTable::addStr(const std::string& str) {
	strs[std::string({ C_DQUOTE }) + str + std::string({ C_DQUOTE })] = "_str_" + std::to_string(strCount++);
}

std::map<const std::string, std::string> SymbolTable::getStrs() {
	return strs;
}

std::string SymbolTable::getStr(const std::string & str) {
	return strs[str];
}