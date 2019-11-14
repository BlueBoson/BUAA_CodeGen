#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <cctype>
#include "Token.h"
#include "symbols.h"

constexpr auto BUFFER_SIZE = 32;

class LexicalAnalyser {
public:
	static LexicalAnalyser& getInstance(std::ifstream& _ifs);
	static void deleteInstance();
	static void reInitial();
	Token getNextToken();

private:
	static LexicalAnalyser* instance;
	char curChar;
	char buffer[BUFFER_SIZE];
	int bufInd;
	int line;
	int column;
	std::string tkStr;
	std::ifstream* ifs;

	LexicalAnalyser(std::ifstream& _ifs);
	~LexicalAnalyser();
	void nextChar();
	void catChar();
	void nextNWChar();
	std::string getStr();
	void clearBuf();
};

