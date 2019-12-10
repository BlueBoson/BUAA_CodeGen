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
			for (; iter->getType() != MidType::END_FUNC; ++iter) {
				if (iter->getType() == MidType::VAR || iter->getType() == MidType::PARAM || iter->getType() == MidType::CONST) {
					++ivNum;
					if (iter->getType() == MidType::VAR && !iter->getOp2().empty()) {
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
	/*
	auto iter = origin.begin();
	while (iter != origin.end()) {
		if (iter->getType() != MidType::FUNC || iter->getOp2() == "main") {
			mids.push_back(*(iter++));
			continue;
		}
		auto func_begin = iter;
		int ivNum = 0;
		for (; iter->getType() != MidType::END_FUNC; ++iter) {
			if (iter->getType() == MidType::VAR || iter->getType() == MidType::PARAM || iter->getType() == MidType::CONST) {
				++ivNum;
				if (iter->getType() == MidType::VAR && !iter->getOp2().empty()) {
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
			continue;
		}
		map<string, string> ivMap;
		for (iter = func_begin; iter->getType() != MidType::END_FUNC; ++iter) {
			if (iter->getType() == MidType::VAR || iter->getType() == MidType::PARAM || iter->getType() == MidType::CONST) {
				ivMap[iter->getResOp()] = MidCode::genIv();
			} else {
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
				mids.push_back()
			}
		}
	}
	*/
}