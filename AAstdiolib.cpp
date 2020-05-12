#include "AAstdiolib.h"
#include "AAMemoryStore.h"
#include "AAExecute.h"
#include "AAIntrPtr.h"

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

		// Convert path to string
		std::wstring ws = pAAVm->GetHeap()->String(aa::vm::PopSomething(AAPrimitiveType::refptr, stack).to_cpp<AAMemoryPtr>())->ToString();

		// Open file
		AAFileStream* fptr = new AAFileStream();
		fptr->OpenWrite(ws);

		// Did we manage to open the file?
		if (fptr) {

			// Get pointer to internal file stream
			AAIntPtr iPtr = AAIntPtr((void*)fptr);

			// Assign pointer
			aa::SetVal(self, 0, iPtr);

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
		int r = aa::GetVal<AAIntPtr>(self, 0).GetPtr<AAFileStream>()->CloseStream();

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

void AAFileStream_Write(AAVM* pAAVm, any_stack& stack) { // write string --> Need variant for char and so forth

	// Pop top element (we know this is a stack value because our argument is 'Any'
	AAStackValue top = stack.Pop<AAStackValue>();

	// Get pointer to object
	AAMemoryPtr ptr = stack.Pop<AAMemoryPtr>();

	// Get the self argument
	const AAObject* self = ptr.get_object();

	// Did we manage to get pointer to ourselves?
	if (self) {

		AAIntPtr iPtr = aa::GetVal<AAIntPtr>(self, 0);

		// Get the file pointer
		AAFileStream* fPtr = iPtr.GetPtr<AAFileStream>();

		// Make sure the file is valid
		if (fPtr) {

			// Write whatever was on top of the stack
			switch (top.get_type()) {
			case AAPrimitiveType::refptr:
				fPtr->WriteString(top.to_cpp<AAMemoryPtr>().get_object()->ToString());
				break;
			case AAPrimitiveType::wchar: {
				fPtr->WriteWchar(top.to_cpp<wchar_t>());
				break;
			}
			default:
				break;
			}

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
