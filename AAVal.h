#pragma once
#include "AA_literals.h"

struct AAVal {
	AA_Literal litVal;
	AAVal() {
		litVal = AA_Literal();
	}
	AAVal(AA_Literal l) {
		litVal = l;
	}
};
