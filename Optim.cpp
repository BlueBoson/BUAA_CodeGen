#include "Optim.h"

void Optim::feed(std::vector<MidCode> origin) {
	this->origin = origin;
}

std::vector<MidCode> Optim::result() {
	if (mids.empty()) {
		return origin;
	} else {
		return mids;
	}
}

void Optim::func_inline() {
	using namespace std;
	if (!mids.empty()) {
		origin = mids;
		mids.clear();
	}
	map<string, vector<MidCode>> funcMap;
	map<string, vector<string>> paraMap;
	vector<MidCode> paras;
	auto iter = origin.begin();
	while (iter != origin.end()) {
		if (iter->getType() == MidType::PUSH) {
			paras.push_back(*(iter++));
		} else if (iter->getType() == MidType::CALL) {
			string funcName = iter->getResOp();
			if (funcMap.find(funcName) == funcMap.end()) {
				for (auto para : paras) {
					mids.push_back(para);
				}
				paras.clear();
				mids.push_back(*(iter++));
			} else {
				auto paraList = paraMap[funcName];
				for (int i = 0; i < paraList.size(); i++) {
					mids.push_back(MidCode(MidType::ASSIGN, paraList[i], paras[i].getResOp(), ""));
				}
				paras.clear();
				for (auto code : funcMap[funcName]) {
					if (code.getType() == MidType::RETURN) {
						break;
					}
					mids.push_back(code);
				}
				iter++;
			}
		} else if (iter->getType() == MidType::FUNC && iter->getOp2() != "main") {
			string funcType = iter->getOp1();
			string funcName = iter->getOp2();
			auto func_begin = iter;
			int ivNum = 0;
			map<string, int> varSet;
			for (iter = func_begin + 1; iter->getType() != MidType::END_FUNC; ++iter) {
				if (iter->getType() == MidType::VAR || iter->getType() == MidType::PARAM || iter->getType() == MidType::CONST) {
					varSet[iter->getResOp()] = 0;
					++ivNum;
					if (iter->getType() == MidType::VAR && !iter->getOp2().empty()) {
						break;
					}
				} else {
					string op = iter->getOp1();
					if (!op.empty() && isLetter(op[0]) && varSet.find(op) == varSet.end()) {
						break;
					}
					op = iter->getOp2();
					if (!op.empty() && isLetter(op[0]) && varSet.find(op) == varSet.end()) {
						break;
					}
					op = iter->getResOp();
					if (!op.empty() && isLetter(op[0]) && varSet.find(op) == varSet.end()) {
						break;
					}
				}
				if (iter->getType() == MidType::LABEL || iter->getType() == MidType::CALL) {
					break;
				}
			}
			if (ivNum > 4 || iter->getType() != MidType::END_FUNC) {
				iter = func_begin;
				mids.push_back(*(iter++));
			} else {
				vector<MidCode> inline_codes;
				vector<string> funcParas;
				map<string, string> ivMap;
				for (iter = func_begin; iter->getType() != MidType::END_FUNC; ++iter) {
					if (iter->getType() == MidType::VAR || iter->getType() == MidType::PARAM || iter->getType() == MidType::CONST) {
						string ivName = MidCode::genIv();
						ivMap[iter->getResOp()] = ivName;
						if (iter->getType() == MidType::PARAM) {
							funcParas.push_back(ivName);
						} else if (iter->getType() == MidType::CONST) {
							inline_codes.push_back(MidCode(MidType::ASSIGN, ivName, iter->getOp2(), ""));
						}
					} else if (iter->getType() != MidType::FUNC) {
						auto code = *iter;
						if (code.getType() != MidType::FUNC) {
							if (ivMap.find(code.getOp1()) != ivMap.end()) {
								code.setOp1(ivMap[code.getOp1()]);
							}
							if (ivMap.find(code.getOp2()) != ivMap.end()) {
								code.setOp2(ivMap[code.getOp2()]);
							}
							if (ivMap.find(code.getResOp()) != ivMap.end()) {
								code.setResOp(ivMap[code.getResOp()]);
							}
						}
						inline_codes.push_back(code);
					}
				}
				if (funcType == "void") {
					inline_codes.push_back(MidCode(MidType::ASSIGN, RET_TV, ZERO_TV, ""));
				}
				funcMap[funcName] = inline_codes;
				paraMap[funcName] = funcParas;
				++iter;
			}
		} else {
			mids.push_back(*(iter++));
		}
	}
}

bool canRep(MidCode& code) {
	std::string res = code.getResOp();
	std::string src = code.getOp1();
	if (code.getType() == MidType::ASSIGN) {

	}
	return code.getType() == MidType::ASSIGN && res.size() > 2 && res[0] == '$' && res[1] == 't' &&
		src[0] != '+' && src[0] != '-' && !isDigit(src[0]) && src[0] != '\'';
}

void Optim::unused_tv() {
	using namespace std;
	if (!mids.empty()) {
		origin = mids;
		mids.clear();
	}
	vector<MidCode> reverse;
	MidCode top(MidType::NONE);
	for (auto riter = origin.rbegin(); riter != origin.rend(); ++riter) {
		if (canRep(*riter) && !reverse.empty()) {
			auto& top = reverse[reverse.size() - 1];
			if (top.getType() == MidType::PUSH) {
				if (top.getResOp() == riter->getResOp()) {
					top.setResOp(riter->getOp1());
					continue;
				}
			} else if (top.getType() == MidType::BEQ || top.getType() == MidType::BNE || top.getType() == MidType::BGEZ ||
				top.getType() == MidType::BGTZ || top.getType() == MidType::BLEZ || top.getType() == MidType::BLTZ ||
				top.getType() == MidType::MINU || top.getType() == MidType::PLUS || top.getType() == MidType::MUL || 
				top.getType() == MidType::DIV || top.getType() == MidType::ASSIGN) {
				bool skip = false;
				if (top.getOp1() == riter->getResOp()) {
					top.setOp1(riter->getOp1());
					skip = true;
				}
				if (top.getOp2() == riter->getResOp()) {
					top.setOp2(riter->getOp1());
					skip = true;
				}
				if (skip) {
					continue;
				}
			}
		}
		reverse.push_back(*riter);
	}
	for (auto riter = reverse.rbegin(); riter != reverse.rend(); ++riter) {
		mids.push_back(*riter);
	}
}
