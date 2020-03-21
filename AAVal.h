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
		litVal.tp = AALiteralType::Int;
		litVal.lit.i = AA_IntLiteral(i);
		obj = 0;
	}

	AAVal(float f) {
		litVal.tp = AALiteralType::Float;
		litVal.lit.f = AA_FloatLiteral(f);
		obj = 0;
	}
	
	AAVal(bool b) {
		litVal.tp = AALiteralType::Boolean;
		litVal.lit.b = AA_BoolLiteral(b);
		obj = 0;
	}
	
	AAVal(std::wstring ws) {
		litVal.tp = AALiteralType::String;
		litVal.lit.s.len = ws.length();
		litVal.lit.s.val = new wchar_t[ws.length()+1];
		wmemset(litVal.lit.s.val, '\0', ws.length() + 1);
		wcscpy(litVal.lit.s.val, ws.c_str());		
		obj = 0;
	}

	AAVal(wchar_t* str, size_t l) {
		litVal.tp = AALiteralType::String;
		litVal.lit.s.len = l;
		litVal.lit.s.val = str;
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
	int valCount;
	AAObject() {
		values = 0;
		valCount = 0;
	}
};

AAObject* AllocAAO(size_t sz);
