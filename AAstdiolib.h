#pragma once
#include "AAVM.h"
#include "AAMemoryStore.h"

#pragma region std

#pragma region io

#include "AAFileStream.h"

#pragma region filestream 

void AAFileStream_Open(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {

	// Convert argument stack into a list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	// Get the self argument
	AAObject* self = pAAVm->GetHeap()->operator[](argl.First().ptr);

	// Did we manage to get pointer to ourselves?
	if (self) {

		self->values[0].litVal.tp = AALiteralType::IntPtr;
		self->values[0].litVal.lit.ptr.ptr = 0;

		// Open file
		AAFileStream* fptr = new AAFileStream();
		fptr->OpenWrite(argl.Last().litVal.lit.s.val);

		// Did we manage to open the file?
		if (fptr) {

			// Assign pointer
			self->values[0].litVal.lit.ptr.ptr = (void*)fptr;

			// Push (the pointer to) self (this) unto the stack (because this acts as the .ctor
			stack.Push(argl.First().ptr);

		} else {

			// Throw runtime error
			pAAVm->ThrowExternalError(AAVM_RuntimeError("IOException", "Failed to open file"));

			return;

		}

	} else {

		// Throw runtime error
		pAAVm->ThrowExternalError(AAVM_RuntimeError("NullPointerException", ("Null value at address '" + std::to_string(argl.First().ptr.val) + "'").c_str()));

		return;

	}

}

void AAFileStream_Close(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {

	// Convert argument stack into a list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	// Get the self argument
	AAObject* self = pAAVm->GetHeap()->operator[](argl.First().ptr);

	// Did we manage to get pointer to ourselves?
	if (self) {

		// Close the file
		int r = ((AAFileStream*)self->values[0].litVal.lit.ptr.ptr)->CloseStream();

		// Did we somehow fail to close?
		if (r != 0) {

			// Throw error
			pAAVm->ThrowExternalError(AAVM_RuntimeError("IOException", "Failed to close file"));

			// Return
			return;

		}

	} else {

		// Throw runtime error
		pAAVm->ThrowExternalError(AAVM_RuntimeError("NullPointerException", ("Null value at address '" + std::to_string(argl.First().ptr.val) + "'").c_str()));

		return;

	}

}

void AAFileStream_Write(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {

	// Convert argument stack into a list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	// Get the self argument
	AAObject* self = pAAVm->GetHeap()->operator[](argl.First().ptr);

	// Did we manage to get pointer to ourselves?
	if (self) {

		// Get the file pointer
		AAFileStream* fPtr = (AAFileStream*)self->values[0].litVal.lit.ptr.ptr;

		// Make sure the file is valid
		if (fPtr) {

			// Write msg
			fPtr->WriteString(argl.Last().ToString());

			// Push (the pointer to) ourselves back onto the stack
			stack.Push(argl.First().ptr);

		} else {

			// Throw error
			pAAVm->ThrowExternalError(AAVM_RuntimeError("IOException", "File not open"));

			// Return
			return;

		}

	} else {

		// Throw runtime error
		pAAVm->ThrowExternalError(AAVM_RuntimeError("NullPointerException", ("Null value at address '" + std::to_string(argl.First().ptr.val) + "'").c_str()));

		return;

	}

}

#pragma endregion
#pragma endregion
#pragma endregion
