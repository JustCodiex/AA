#pragma once
#include "AAVM.h"

void AAString_Ctor(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {
	
	// Convert argument stack into a list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	// Update internal _str field to be correctly assigned
	argl.At(0).obj->values[0] = argl.At(1);

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
