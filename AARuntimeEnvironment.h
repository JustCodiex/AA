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

	/// <summary>
	/// Creates an instance of a runtime environment
	/// </summary>
	/// <returns></returns>
	AARuntimeEnvironment();

	/// <summary>
	/// Pops the environment and the local variable scopes
	/// </summary>
	void PopEnvironment(bool self);

};
