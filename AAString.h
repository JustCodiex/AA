#pragma once
#include "AAVM.h"

void AAString_Ctor(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {
	
	// Convert argument stack into a list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	// Update internal _str field to be correctly assigned
	//argl.At(0).obj->values[0] = argl.At(1);

	// Push object onto stack
	stack.Push(argl.At(0));

	// Exit function
	return;

}

void AAString_Length(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {

	// Convert argument stack into a list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	// Make sure it's a string
	if (argl.At(0).litVal.tp == AALiteralType::String) {

		// Push to stack
		stack.Push(AAVal((int)(argl.At(0).litVal.lit.s.len)));

	} else {
		// throw err
	}

}

void AAString_Concat(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {

	// Convert argument stack into a list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	if (argl.At(0).litVal.tp != AALiteralType::String || argl.At(1).litVal.tp != AALiteralType::String) {
		
		// throw err
		return;

	} else {

		AA_StringLiteral a = argl.At(0).litVal.lit.s;
		AA_StringLiteral b = argl.At(1).litVal.lit.s;

		size_t slen = a.len + b.len;
		wchar_t* str = new wchar_t[slen];
		wcscpy(str, a.val);
		wcscat(str, b.val);

		stack.Push(AAVal(str, slen));

	}

}
