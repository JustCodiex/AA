#pragma once
#include "AAC.h"
#include "AAP.h"
#include "AAO.h"
#include "AAVarEnv.h"
#include "AAProgram.h"
#include "AARuntimeEnvironment.h"
#include "AACClass.h"
#include "stack.h"
#include <iostream>
#include <ctime>

struct AAVM_RuntimeError {
	const char* errMsg;
	const char* errName;
	AARuntimeEnvironment errEnv;
	aa::stack<AARuntimeEnvironment> callStack;
	AAVM_RuntimeError() {
		this->errMsg = 0;
		this->errName = 0;
		this->errEnv = AARuntimeEnvironment();
	}
	AAVM_RuntimeError(const char* eType, const char* eMsg) {
		this->errName = eType;
		this->errMsg = eMsg;
		this->errEnv = AARuntimeEnvironment();
	}
	AAVM_RuntimeError(const char* eType, const char* eMsg, AARuntimeEnvironment env, aa::stack<AARuntimeEnvironment> cstack) {
		this->errName = eType;
		this->errMsg = eMsg;
		this->errEnv = env;
		this->callStack = cstack;
	}
};

class AAMemoryStore;

class AAVM {

public:

	AAVM();

	void Release();

	void RunFile(std::wstring file);
	void RunExpression(std::wstring input);

	/*
	** Compiles the input file and then saves the resulting bytecode in the specified output file
	*/
	AAC_Out CompileFileToFile(std::wstring fileIn, std::wstring fileOut);

	AAVal Execute(AAC_Out bytecode);
	AAVal Execute(unsigned char* bytes, unsigned long long len);

	AAVal CompileAndRunFile(std::wstring sourcefile);
	AAVal CompileAndRunFile(std::wstring sourcefile, std::wstring binaryoutputfile, std::wstring formattedoutputfile);

	AAVal CompileAndRunExpression(std::wstring input);
	AAVal CompileAndRunExpression(std::wstring input, std::wstring binaryoutputfile, std::wstring formattedoutputfile);

	AAP* GetParser() { return m_parser; }
	AAC* GetCompiler() { return m_compiler; }

	void SetOutput(std::ostream* stream) {
		m_outStream = stream;
	}

	void EnableCompilerLog(bool enable) { m_logCompileMessages = enable; }
	void EnableExecutionTiming(bool enable) { m_logExecTime = enable; }
	void EnableTopStackLogging(bool enable) { m_logTopOfStackAfterExec = enable; }

	bool IsCompilerLoggingEnabled() { return m_logCompileMessages; }
	bool IsExecutionTimeLoggingEnabled() { return m_logExecTime; }
	bool IsTopStackLoggingEnabled() { return m_logTopOfStackAfterExec; }

	int RegisterFunction(AACSingleFunction funcPtr);
	int RegisterFunction(AACSingleFunction funcPtr, AACNamespace* domain);
	AAClassSignature* RegisterClass(std::wstring typeName, AACClass cClass);

	bool HasCompileError() { return !m_lastCompileResult.success; }
	bool HasRuntimeError() { return m_hasRuntimeError; }

	AAC_CompileErrorMessage GetCompileError() { return m_lastCompileResult.firstMsg; }
	AAVM_RuntimeError GetRuntimeError() { return m_lastRuntimeError; }

	void ThrowExternalError(AAVM_RuntimeError externalErr);

	/// <summary>
	/// Get the current instance of the heap (Note: This is only valid while running a program)
	/// </summary>
	/// <returns>The currently used heap</returns>
	AAMemoryStore* GetHeap() { return m_heapMemory; }

	static AAVM* CreateNewVM(bool logExecuteTime, bool logCompiler, bool logTopStack);

private:

	AAVal CompileAndRun(AAP_ParseResult input, std::wstring binaryoutputfile, std::wstring formattedoutputfile);

	AAVal Run(AAProgram::Procedure* procedures, int entry);

	AAVal Run(AAProgram* pProg);

	AAVal ReportStack(aa::stack<AAVal> stack);

	inline AAVal BinaryOperation(AAByteCode op, AA_Literal left, AA_Literal right);

	void WriteCompilerError(AAC_CompileErrorMessage errMsg);
	void WriteSyntaxError(AAP_SyntaxErrorMessage errMsg);
	void WriteRuntimeError(AAVM_RuntimeError err);

	void WriteMsg(const char* msg);

	// Register function in the internal register
	int RegisterFunction(AACSingleFunction funcPtr, AAFuncSignature*& funcSig, AACNamespace* domain, bool isClassMethod = false);

	// Fix any parts of the class that references self (typewise)
	void FixSelfReferences(AAClassSignature* signature);

	// Load the standard library
	void LoadStandardLibrary();

	void StopAndLogCompile();

private:

	AAP* m_parser;
	AAC* m_compiler;

	std::ostream* m_outStream;

	bool m_logExecTime;
	bool m_logCompileMessages;
	bool m_logTopOfStackAfterExec;

	std::vector<AACSingleFunction> m_cppfunctions;

	AAMemoryStore* m_heapMemory;

	bool m_hasRuntimeError;
	AAVM_RuntimeError m_lastRuntimeError;

	AAC_CompileResult m_lastCompileResult;

	clock_t m_startCompile;

};
