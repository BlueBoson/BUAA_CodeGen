#include "MipsCode.h"

MipsCode::MipsCode(MipsType type, int rd, int rs, int rt, std::string iorl) : 
	type(type), rd(rd), rs(rs), rt(rt), iorl(iorl) {

}

std::string MipsCode::toString() {
	return "";
}