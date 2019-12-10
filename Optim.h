#pragma once
#include <vector>
#include "MidCode.h"

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

