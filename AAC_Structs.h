#pragma once
#include "AACodePosition.h"

// Compiled output
struct AAC_Out {
	unsigned long long length;
	unsigned char* bytes;
	AAC_Out() {
		length = 0;
		bytes = 0;
	}
};

// Error message
struct AAC_CompileErrorMessage {
	int errorType;
	AACodePosition errorSource;
	const char* errorMsg;
	AAC_CompileErrorMessage() {
		errorType = INT_MAX;
		errorSource = AACodePosition(0, 0);
		errorMsg = 0;
	}
	AAC_CompileErrorMessage(int eType, const char* eMsg) {
		errorMsg = eMsg;
		errorType = eType;
		errorSource = AACodePosition(0, 0);
	}
	AAC_CompileErrorMessage(int eType, const char* eMsg, AACodePosition eSrc) {
		errorMsg = eMsg;
		errorType = eType;
		errorSource = eSrc;
	}
};

// The compiler result
struct AAC_CompileResult {
	AAC_Out result;
	bool success;
	AAC_CompileErrorMessage firstMsg;
	AAC_CompileResult() {
		success = false;
		result = AAC_Out();
		firstMsg = AAC_CompileErrorMessage();
	}
};

// Shortcut for no compile error
#define NO_COMPILE_ERROR_MESSAGE AAC_CompileErrorMessage(-1, "Compiled Sucessfully")

// Returns true if there was an error message
#define COMPILE_ERROR(a) (a).errorType != -1

// Did the compiler compile the code sucessfully?
#define COMPILE_SUCESS(a) a.success

// Did the compiler failed to compile the code sucessfully?
#define COMPILE_FAILED(a) !COMPILED_SUCESS(a)
