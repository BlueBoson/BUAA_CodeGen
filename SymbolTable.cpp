#include "SymbolTable.h"

SymbolTable* SymbolTable::instance = NULL;

SymbolTable& SymbolTable::getInstance() {
	if (instance == NULL) {
		instance = new SymbolTable;
	}
	return *instance;
}

void SymbolTable::deleteInstance() {
	delete instance;
	instance = NULL;
}

bool SymbolTable::push(Token& tk, IdenType type) {
	auto symName = tk.getValue();
	int last = tables.size() - 1;
	auto iter = tables[last].find(symName);
	if (iter != tables[last].end()) {
		return false;
	} else {
		tables[last][symName] = type;
		return true;
	}
}

IdenType SymbolTable::find(Token& tk) {
	auto symName = tk.getValue();
	for (int i = tables.size() - 1; i >= 0; i--) {
		auto iter = tables[i].find(symName);
		if (iter != tables[i].end()) {
			return iter->second;
		}
	}
	return IdenType::NONE;
}

void SymbolTable::addArgs(Token& tk, std::vector<ArgType>& args) {
	auto iter = funcArgs.find(tk.getValue());
	if (iter == funcArgs.end()) {
		funcArgs[tk.getValue()] = args;
	}
}

std::vector<ArgType> SymbolTable::getArgs(Token& tk) {
	std::vector<ArgType> r;
	auto iter = funcArgs.find(tk.getValue());
	if (iter == funcArgs.end()) {
		return r;
	} else {
		return iter->second;
	}
}

void SymbolTable::newSub() {
	using namespace std;
	map<const std::string, IdenType> sub;
	tables.push_back(sub);
}

void SymbolTable::exitSub() {
	tables.pop_back();
}
