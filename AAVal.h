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
	AAVal(int i) {
		litVal.lit.i = AA_IntLiteral(i);
		litVal.tp = AALiteralType::Int;
	}
	AAVal(bool b) {
		litVal.lit.b = AA_BoolLiteral(b);
		litVal.tp = AALiteralType::Boolean;
	}
};
