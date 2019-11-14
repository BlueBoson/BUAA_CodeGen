#include <iostream>
#include <fstream>
#include "GrammaticalAnalyser.h"

constexpr auto SRC_FILE = "testfile.txt";
constexpr auto OUTPUT_FILE = "output.txt";
constexpr auto ERROR_FILE = "error.txt";
constexpr auto LOG_FILE = "log.txt";
constexpr auto MID_FILE = "midcode_raw.txt";

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
		cout << "Cannot Error Output File " << ERROR_FILE << endl;
		return 0;
	}
	ofstream lfs(LOG_FILE, ios::out);
	if (!lfs) {
		cout << "Cannot Log Output File " << LOG_FILE << endl;
		return 0;
	}
	ofstream mfs(MID_FILE, ios::out);
	if (!mfs) {
		cout << "Cannot Log Midcode File " << MID_FILE << endl;
		return 0;
	}
	Error::setErrorFile(efs);
	Error::setLogFile(lfs);
	GrammaticalAnalyser& grammaticalAnalyser = GrammaticalAnalyser::getInstance(ifs, ofs);
	SymbolTable table = SymbolTable::getInstance();
	grammaticalAnalyser.analyse(mfs, table);
	SymbolTable::deleteInstance();
	GrammaticalAnalyser::deleteInstance();
	Error::cancelErrorFile();
	Error::cancelLogFile();
	ifs.close();
	ofs.close();
	efs.close();
	lfs.close();
	return 0;
}