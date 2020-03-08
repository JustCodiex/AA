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
	AAVal(std::wstring ws) {
		litVal.lit.s.len = ws.length();
		litVal.lit.s.val = new wchar_t[ws.length()+1];
		wmemset(litVal.lit.s.val, '\0', ws.length() + 1);
		wcscpy(litVal.lit.s.val, ws.c_str());
		litVal.tp = AALiteralType::String;
	}
	std::wstring ToString();

	static AAVal Null;

};
