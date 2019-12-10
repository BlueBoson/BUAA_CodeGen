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
	if (!mids.empty()) {
		origin = mids;
		mids.clear();
	}
}