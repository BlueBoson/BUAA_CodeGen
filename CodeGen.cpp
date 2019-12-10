#include <iostream>
#include <fstream>
#include "GrammaticalAnalyser.h"
#include "MidCode.h"
#include "MipsGen.h"
#include "Optim.h"

constexpr auto SRC_FILE = "testfile.txt";
constexpr auto OUTPUT_FILE = "output.txt";
constexpr auto ERROR_FILE = "error.txt";
constexpr auto LOG_FILE = "log.txt";
constexpr auto MID_FILE = "17231066_李瑞康_优化前中间代码.txt";
constexpr auto OPT_FILE = "17231066_李瑞康_优化后中间代码.txt";
/*
constexpr auto MID_FILE = "17231066_LiRuikang_BeforeOptim.txt";
constexpr auto OPT_FILE = "17231066_LiRuikang_AfterOptim.txt";
*/
constexpr auto OBJ_FILE = "mips.txt";

int main(int agrc, char* argv[]) {
	using namespace std;
	ifstream ifs(SRC_FILE, ios::in);
	if (!ifs) {
		cout << "Cannot Open Source File " << SRC_FILE << endl;
		return 0;
	}
	ofstream ofs(OUTPUT_FILE, ios::out);
	if (!ofs) {
		cout << "Cannot Open Output File " << OUTPUT_FILE << endl;
		return 0;
	}
	ofstream efs(ERROR_FILE, ios::out);
	if (!efs) {
		cout << "Cannot Open Error File " << ERROR_FILE << endl;
		return 0;
	}
	ofstream lfs(LOG_FILE, ios::out);
	if (!lfs) {
		cout << "Cannot Open Log File " << LOG_FILE << endl;
		return 0;
	}
	ofstream mfs(MID_FILE, ios::out);
	if (!mfs) {
		cout << "Cannot Open Midcode File " << MID_FILE << endl;
		return 0;
	}
	ofstream ops(OPT_FILE, ios::out);
	if (!ops) {
		cout << "Cannot Open Optim File " << OPT_FILE << endl;
		return 0;
	}
	ofstream mips(OBJ_FILE, ios::out);
	if (!mips) {
		cout << "Cannot Open Mips FIle" << OBJ_FILE << endl;
	}
	Error::setErrorFile(efs);
	Error::setLogFile(lfs);
	GrammaticalAnalyser& grammaticalAnalyser = GrammaticalAnalyser::getInstance(ifs, ofs);
	SymbolTable table = SymbolTable::getInstance();
	grammaticalAnalyser.analyse(mfs, table);
	auto mids = MidCode::getVec();
	Optim optimizer;
	optimizer.feed(mids);
	optimizer.func_inline();
	optimizer.unused_tv();
	mids = optimizer.result();
	for (auto mid : mids) {
		ops << mid.toString() << endl;
	}
	MipsGen& genor = MipsGen::getInstance(mids, mips, table);
	genor.gen();
	SymbolTable::deleteInstance();
	GrammaticalAnalyser::deleteInstance();
	MipsGen::deleteInstance();
	Error::cancelErrorFile();
	Error::cancelLogFile();
	ifs.close();
	ofs.close();
	efs.close();
	lfs.close();
	mfs.close();
	ops.close();
	mips.close();
	return 0;
}