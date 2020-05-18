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

#pragma region AAC Compile Error Types

namespace aa {
	namespace compiler_err {

		/// <summary>
		/// The error type is currently not classified (in the type checker)
		/// </summary>
		const int C_Unclassified_Type = 42;

		/// <summary>
		/// Invalid Type Error
		/// (Not valid in context)
		/// </summary>
		const int C_Invalid_Type = 100;

		/// <summary>
		/// Invalid Type Error
		/// (Not valid in condition)
		/// </summary>
		const int C_Invalid_Condition_Type = 101;

		/// <summary>
		/// Type is not defined
		/// (Not Found within scope)
		/// </summary>
		const int C_Undefined_Type = 102;

		/// <summary>
		/// Type is not defined
		/// (Not Found within scope)
		/// </summary>
		const int C_Undefined_Class = 103;

		/// <summary>
		/// Types are not matching
		/// </summary>
		const int C_Mismatching_Types = 200;

		/// <summary>
		/// Type doesn't match unary operation
		/// </summary>
		const int C_Msimatching_Unary_Type = 201;

		/// <summary>
		/// Types don't have a compatible operation
		/// </summary>
		const int C_Invalid_Binary_Operator = 202;

		/// <summary>
		/// The tuple index is out of bounds (ie. (0,1,2)._5) 
		/// </summary>
		const int C_Invalid_Tuple_Index = 211;

		/// <summary>
		/// The function being called is undefined
		/// </summary>
		const int C_Undefined_Function = 311;

		/// <summary>
		/// The constructor being called is undefined
		/// </summary>
		const int C_Undefined_Constructor = 312;

		/// <summary>
		/// There exists no function overload matching argument list
		/// </summary>
		const int C_Undefined_Function_Overload = 321;

		/// <summary>
		/// There exists no constructor overload matching argument list
		/// </summary>
		const int C_Undefined_Constructor_Overload = 322;

		/// <summary>
		/// Very fatal compiler error, when compiling a tagged class
		/// </summary>
		const int C_Compiler_TaggedClass_Generator_Error = -101;

	}
}

#pragma endregion

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

// Returns true if the was no error message
#define COMPILE_OK(a) !(COMPILE_ERROR(a))

// Did the compiler compile the code sucessfully?
#define COMPILE_SUCESS(a) a.success

// Did the compiler failed to compile the code sucessfully?
#define COMPILE_FAILED(a) !COMPILED_SUCESS(a)
