#pragma once
#include "AAVM.h"

#pragma region std

#pragma region io

#include <iostream>

#pragma region filestream 

void AAFileStream_Open(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {

	// Convert argument stack into a list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	// Get the self argument
	AAVal self = argl.First();
	self.obj->values[0].litVal.tp = AALiteralType::IntPtr;
	self.obj->values[0].litVal.lit.ptr.ptr = 0;

	// Open file
	FILE* fptr = _wfopen(argl.Last().litVal.lit.s.val, L"w");

	// Did we manage to open the file?
	if (fptr) {

		// Assign pointer
		self.obj->values[0].litVal.lit.ptr.ptr = (void*)fptr;

		// Push self (this)
		stack.Push(self);

	} else {
		
		// Throw runtime error
		pAAVm->ThrowExternalError(AAVM_RuntimeError("IOException", "Failed to open file"));

		return;

	}

}

void AAFileStream_Close(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {

	// Convert argument stack into a list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	// Get the self argument
	AAVal self = argl.First();

	// Close the file
	int r = fclose((FILE*)self.obj->values[0].litVal.lit.ptr.ptr);

	// Did we somehow fail to close?
	if (r != 0) {

		// Throw error
		pAAVm->ThrowExternalError(AAVM_RuntimeError("IOException", "Failed to close file"));

		// Return
		return;

	}

}

void AAFileStream_Write(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {

	// Convert argument stack into a list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	// Get the self argument
	AAVal self = argl.First();

	// Get the file pointer
	FILE* fPtr = (FILE*)self.obj->values[0].litVal.lit.ptr.ptr;

	// Make sure the file is valid
	if (fPtr) {

		// Convert to string
		std::string s = string_cast(argl.Last().ToString().c_str());

		// Write msg
		fwrite(s.c_str(), sizeof(char), s.length(), fPtr);

		// Push ourselves back onto the stack
		stack.Push(self);

	} else {

		// Throw error
		pAAVm->ThrowExternalError(AAVM_RuntimeError("IOException", "File not open"));

		// Return
		return;

	}

}

#pragma endregion
#pragma endregion
#pragma endregion
