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

	AAC_Out CompileExpressionToFile(std::wstring input, std::wstring outputfile);
	AAC_Out CompileFileToFile(std::wstring fileIn, std::wstring fileOut);

	void Execute(AAC_Out bytecode);
	void Execute(unsigned char* bytes, unsigned long long len);

	AAP* GetParser() { return m_parser; }
	AAC* GetCompiler() { return m_compiler; }

	void SetOutput(std::ostream* stream) {
		m_outStream = stream;
	}

private:

	void Run(AA_Literal* cenv, AAVarEnv* venv, AAO* ops, int opCount);

	void Run(AAProgram* pProg);

	void ReportStack(aa::stack<AAVal> stack);

private:

	AAP* m_parser;
	AAC* m_compiler;

	std::ostream* m_outStream;

};

AAVM* AANewVM();
