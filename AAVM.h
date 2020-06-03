#pragma once
#include "AAC.h"
#include "AAP.h"
#include "AAO.h"
#include "AAVarEnv.h"
#include "AAProgram.h"
#include "AARuntimeEnvironment.h"
#include "AACClass.h"
#include "AAStackValue.h"
#include "stack.h"
#include "any_stack.h"
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

	AAStackValue Execute(AAC_Out bytecode);
	AAStackValue Execute(unsigned char* bytes, unsigned long long len);

	AAStackValue CompileAndRunFile(std::wstring sourcefile);
	AAStackValue CompileAndRunFile(std::wstring sourcefile, std::wstring binaryoutputfile);
	AAStackValue CompileAndRunFile(std::wstring sourcefile, std::wstring binaryoutputfile, std::wstring formattedoutputfile);
	AAStackValue CompileAndRunFile(std::wstring sourcefile, std::wstring binaryoutputfile, std::wstring formattedoutputfile, std::wstring unparsefile);

	AAStackValue CompileAndRunExpression(std::wstring input);
	AAStackValue CompileAndRunExpression(std::wstring input, std::wstring binaryoutputfile);
	AAStackValue CompileAndRunExpression(std::wstring input, std::wstring binaryoutputfile, std::wstring formattedoutputfile);
	AAStackValue CompileAndRunExpression(std::wstring input, std::wstring binaryoutputfile, std::wstring formattedoutputfile, std::wstring unparsefile);

	AAP* GetParser() { return m_parser; }
	AAC* GetCompiler() { return m_compiler; }

	void SetOutput(std::ostream* stream) {
		m_outStream = stream;
	}

	std::ostream* GetOutput() {
		return m_outStream;
	}

	void EnableCompilerLog(bool enable) { m_logCompileMessages = enable; }
	void EnableExecutionTiming(bool enable) { m_logExecTime = enable; }
	void EnableTopStackLogging(bool enable) { m_logTopOfStackAfterExec = enable; }
	void EnableTraceLogging(bool enable) { m_logTrace = enable; }

	bool IsCompilerLoggingEnabled() { return m_logCompileMessages; }
	bool IsExecutionTimeLoggingEnabled() { return m_logExecTime; }
	bool IsTopStackLoggingEnabled() { return m_logTopOfStackAfterExec; }
	bool IsTraceLoggingEnabled() { return m_logTrace; }

	int RegisterFunction(AACSingleFunction funcPtr);
	int RegisterFunction(AACSingleFunction funcPtr, AACNamespace* domain);
	AAClassSignature* RegisterClass(std::wstring typeName, AACClass cClass);

	bool HasSyntaxError() { return m_lastSyntaxError.errorType != -1; }
	bool HasCompileError() { return !m_lastCompileResult.success; }
	bool HasRuntimeError() { return m_hasRuntimeError; }

	AAC_CompileErrorMessage GetCompileError() { return m_lastCompileResult.firstMsg; }
	AAVM_RuntimeError GetRuntimeError() { return m_lastRuntimeError; }

	void ThrowExternalError(AAVM_RuntimeError externalErr);

	const AACSingleFunction GetBuiltinFuncByIndex(const size_t i) { return m_cppfunctions.Apply(i); }

	/// <summary>
	/// Get the current instance of the heap (Note: This is only valid while running a program)
	/// </summary>
	/// <returns>The currently used heap</returns>
	AAMemoryStore* GetHeap() { return m_heapMemory; }

	static AAVM* CreateNewVM(bool logExecuteTime, bool logCompiler, bool logTopStack);

private:

	AAStackValue CompileAndRun(AAP_ParseResult input, std::wstring binaryoutputfile, std::wstring formattedoutputfile, std::wstring unparsefile);

	AAStackValue Run(AAProgram::Procedure* procedures, AAStaticTypeEnvironment* staticProgramTypeEnvironment, int entry);

	void exec(AAProgram::Procedure* procedures, aa::stack<AARuntimeEnvironment>& callstack, any_stack& opstack, AARuntimeEnvironment& execp);

	AAStackValue Run(AAProgram* pProg);

	//AAStackValue ReportStack(aa::stack<AAStackValue> stack);
	AAStackValue ReportStack(any_stack& stack);

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

	inline void Trace(const char* msg, ...);

private:

	AAP* m_parser;
	AAC* m_compiler;

	std::ostream* m_outStream;

	bool m_logExecTime;
	bool m_logCompileMessages;
	bool m_logTopOfStackAfterExec;
	bool m_logTrace;

	aa::set<AACSingleFunction> m_cppfunctions;

	AAMemoryStore* m_heapMemory;
	AAStaticTypeEnvironment* m_staticTypeEnvironment;

	bool m_hasRuntimeError;
	AAVM_RuntimeError m_lastRuntimeError;

	AAC_CompileResult m_lastCompileResult;
	AAP_SyntaxErrorMessage m_lastSyntaxError;

	clock_t m_startCompile;

};
