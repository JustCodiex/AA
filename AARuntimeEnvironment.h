#pragma once
#include "AAVarEnv.h"

// The runtime environment
struct AARuntimeEnvironment {

	// Pointer to current procedure
	signed int procPointer;

	// Pointer to current operation to execute
	signed int opPointer;
	
	// Variable environment at runtime
	AAVarEnv* venv;

};
