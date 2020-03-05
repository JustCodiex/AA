#pragma once
#include "AAC.h"
#include "AAP.h"
#include "AAO.h"
#include "AAVarEnv.h"
#include "AAProgram.h"
#include "stack.h"
#include <iostream>

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

	void Execute(AAC_Out bytecode);
	void Execute(unsigned char* bytes, unsigned long long len);

	void CompileAndRunFile(std::wstring sourcefile);
	void CompileAndRunFile(std::wstring sourcefile, std::wstring binaryoutputfile, std::wstring formattedoutputfile);

	void CompileAndRunExpression(std::wstring input);
	void CompileAndRunExpression(std::wstring input, std::wstring binaryoutputfile, std::wstring formattedoutputfile);

	AAP* GetParser() { return m_parser; }
	AAC* GetCompiler() { return m_compiler; }

	void SetOutput(std::ostream* stream) {
		m_outStream = stream;
	}

	static AAVM* CreateNewVM(bool logExecuteTime, bool logCompiler);

private:

	void Run(AAProgram::Procedure* procedures, int entry);

	void Run(AAProgram* pProg);

	void ReportStack(aa::stack<AAVal> stack);

private:

	AAP* m_parser;
	AAC* m_compiler;

	std::ostream* m_outStream;

	bool m_logExecTime;
	bool m_logCompileMessages;

};
