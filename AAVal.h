#pragma once
#include "AA_literals.h"

struct AAObject;

struct AAVal {
	
	AA_Literal litVal;
	AAObject* obj;

	AAVal() {
		litVal = AA_Literal();
		obj = 0;
	}

	AAVal(AA_Literal l) {
		litVal = l;
		obj = 0;
	}
	
	AAVal(int i) {
		litVal.lit.i = AA_IntLiteral(i);
		litVal.tp = AALiteralType::Int;
		obj = 0;
	}

	AAVal(float f) {
		litVal.lit.f = AA_FloatLiteral(f);
		litVal.tp = AALiteralType::Float;
		obj = 0;
	}
	
	AAVal(bool b) {
		litVal.lit.b = AA_BoolLiteral(b);
		litVal.tp = AALiteralType::Boolean;
		obj = 0;
	}
	
	AAVal(std::wstring ws) {
		litVal.lit.s.len = ws.length();
		litVal.lit.s.val = new wchar_t[ws.length()+1];
		wmemset(litVal.lit.s.val, '\0', ws.length() + 1);
		wcscpy(litVal.lit.s.val, ws.c_str());
		litVal.tp = AALiteralType::String;
		obj = 0;
	}
	
	AAVal(AAObject* o) {
		obj = o;
	}

	std::wstring ToString();

	static AAVal Null;

};

struct AAObject {
	AAVal* values;
};

AAObject* AllocAAO(size_t sz);
