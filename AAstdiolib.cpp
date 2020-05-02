#include "AAstdiolib.h"
#include "AAMemoryStore.h"
#include "AAExecute.h"

#pragma region std

#pragma region io

#include "AAFileStream.h"

#pragma region filestream 

void AAFileStream_Open(AAVM* pAAVm, any_stack& stack) {

	// Get pointer to object
	AAMemoryPtr ptr = stack.Pop<AAMemoryPtr>();

	// Get the self argument
	const AAObject* self = ptr.get_object();

	// Did we manage to get pointer to ourselves?
	if (self) {

		// Open file
		AAFileStream* fptr = new AAFileStream();
		fptr->OpenWrite(aa::vm::PopSomething(AAPrimitiveType::string, stack).ToString());

		// Did we manage to open the file?
		if (fptr) {

			// Assign pointer
			aa::SetVal(self, 0, AAVal(&fptr));

			// Push (the pointer to) self (this) unto the stack (because this acts as the .ctor
			stack.Push(ptr);

		} else {

			// Throw runtime error
			pAAVm->ThrowExternalError(AAVM_RuntimeError("IOException", "Failed to open file"));

		}

	} else {

		// Throw runtime error
		pAAVm->ThrowExternalError(AAVM_RuntimeError("NullPointerException", ("Null value at address '" + ptr.get_memory_address_str() + "'").c_str()));

	}

}

void AAFileStream_Close(AAVM* pAAVm, any_stack& stack) {

	// Get pointer to object
	AAMemoryPtr ptr = stack.Pop<AAMemoryPtr>();

	// Get the self argument
	const AAObject* self = ptr.get_object();

	// Did we manage to get pointer to ourselves?
	if (self) {

		// Close the file
		int r = aa::GetVal<AAVal>(self, 0).ToValue<AAFileStream*>()->CloseStream();

		// Did we somehow fail to close?
		if (r != 0) {

			// Throw error
			pAAVm->ThrowExternalError(AAVM_RuntimeError("IOException", "Failed to close file"));

		}

	} else {

		// Throw runtime error
		pAAVm->ThrowExternalError(AAVM_RuntimeError("NullPointerException", ("Null value at address '" + ptr.get_memory_address_str() + "'").c_str()));

	}

}

void AAFileStream_Write(AAVM* pAAVm, any_stack& stack) {

	// Get pointer to object
	AAMemoryPtr ptr = stack.Pop<AAMemoryPtr>();

	// Get the self argument
	const AAObject* self = ptr.get_object();

	// Did we manage to get pointer to ourselves?
	if (self) {

		// Get the file pointer
		AAFileStream* fPtr = (AAFileStream*)aa::GetVal<AAVal>(self, 0).ToValue<AAFileStream*>();

		// Make sure the file is valid
		if (fPtr) {

			// Write msg
			fPtr->WriteString(aa::vm::PopSomething(AAPrimitiveType::string, stack).ToString());

			// Push (the pointer to) ourselves back onto the stack
			stack.Push(ptr);

		} else {

			// Throw error
			pAAVm->ThrowExternalError(AAVM_RuntimeError("IOException", "File not open"));

		}

	} else {

		// Throw runtime error
		pAAVm->ThrowExternalError(AAVM_RuntimeError("NullPointerException", ("Null value at address '" + ptr.get_memory_address_str() + "'").c_str()));

	}

}

#pragma endregion
#pragma endregion
#pragma endregion
