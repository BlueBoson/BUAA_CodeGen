#pragma once
#include <vector>
#include <map>
#include "MidCode.h"
#include "symbols.h"

class Optim {
private:
	std::vector<MidCode> origin;
	std::vector<MidCode> mids;
public:
	void feed(std::vector<MidCode> origin);
	std::vector<MidCode> result();
	void func_inline();
	void unused_tv();
};

