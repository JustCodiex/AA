#pragma once
#include "bwalker.h"
#include "AAO.h"
#include "AAVarEnv.h"
#include "AARuntimeTypeEnv.h"
#include "AAStackFrame.h"

/// <summary>
/// Represents a Å program containing all the information needed to be executed by the Virtual Machine
/// </summary>
class AAProgram {
	friend class AAVM; // Allow the virtual machine to access private parts of class

public:

	AAProgram();

	/// <summary>
	/// Load a program directly from a byte array
	/// </summary>
	/// <param name="bytes">The byte array containing the binary information</param>
	/// <param name="length">The length of the byte array (To ensure we don't read out of bounds)</param>
	/// <returns></returns>
	bool LoadProgram(unsigned char* bytes, unsigned long long length);

	/// <summary>
	/// Load a program directly from its source file
	/// </summary>
	/// <param name="binarypath">The string path of the program binary to load</param>
	/// <returns></returns>
	bool LoadProgram(std::wstring binarypath);

	/// <summary>
	/// Retrieves the entry point (The procedure to call first)
	/// </summary>
	/// <returns></returns>
	int GetEntryPoint() const { return m_entryPoint; }

	/// <summary>
	/// Retrieve the type environment created by the program
	/// </summary>
	/// <returns></returns>
	AAStaticTypeEnvironment* GetTypeEnvironment();

	/// <summary>
	/// Release the program and all its base resources
	/// </summary>
	void Release();

private:

	void LoadConstants(AAStackFrame& proc, aa::bwalker& bw);
	void LoadOperations(AAStackFrame& proc, aa::bwalker& bw);
	void LoadType(aa::bwalker& bw);

private:

	unsigned char m_compiledWithVersion[10] = {};

	int m_entryPoint;
	int m_procedureCount;
	int m_signatureCount;

	AAStackFrame* m_stackframes;
	AAStaticTypeEnvironment* m_types;

};
