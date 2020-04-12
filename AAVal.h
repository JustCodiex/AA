#pragma once
#include "AA_literals.h"
#include "AAMemory.h"
#include "stack.h"

struct AAObject;

struct AAVal {
	
	AA_Literal litVal;
	AAMemoryPtr ptr;

	AAVal() {
		litVal = AA_Literal();
		ptr = 0;
	}

	AAVal(AA_Literal l) {
		litVal = l;
		ptr = 0;
	}
	
	AAVal(int i) {
		litVal.tp = AALiteralType::Int;
		litVal.lit.i = AA_IntLiteral(i);
		ptr = 0;
	}

	AAVal(float f) {
		litVal.tp = AALiteralType::Float;
		litVal.lit.f = AA_FloatLiteral(f);
		ptr = 0;
	}
	
	AAVal(bool b) {
		litVal.tp = AALiteralType::Boolean;
		litVal.lit.b = AA_BoolLiteral(b);
		ptr = 0;
	}
	
	AAVal(std::wstring ws) {
		litVal.tp = AALiteralType::String;
		litVal.lit.s.len = ws.length();
		litVal.lit.s.val = new wchar_t[ws.length()+1];
		wmemset(litVal.lit.s.val, '\0', ws.length() + 1);
		wcscpy(litVal.lit.s.val, ws.c_str());		
		ptr = 0;
	}

	AAVal(wchar_t* str, size_t l) {
		litVal.tp = AALiteralType::String;
		litVal.lit.s.len = l;
		litVal.lit.s.val = str;
		ptr = 0;
	}

	AAVal(AAMemoryPtr ptr) {
		this->ptr = ptr;
		this->litVal.tp = AALiteralType::Null;
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

class AAVM;
void AAO_ToString(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack);
void AAO_NewObject(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack);
