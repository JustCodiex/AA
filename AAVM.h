#pragma once

#include "AAC.h"
#include "AAP.h"
#include "AAO.h"
#include "AAVarEnv.h"
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

	AA_Literal* LoadConstsTable(unsigned char* bytes, unsigned long long len, unsigned int& offset);
	AAVarEnv* LoadVariableEnviornment(unsigned char* bytes, unsigned long long length, unsigned int& offset);
	AAO* LoadOpSequence(unsigned char* bytes, unsigned long long len, int& count);

	void CreateExecutionEnvironment(unsigned char* bytes, unsigned long long len, AA_Literal*& constants, AAVarEnv*& varEnv, AAO*& oplist, int& opCount);

	void Run(AA_Literal* cenv, AAVarEnv* venv, AAO* ops, int opCount);

private:

	AAP* m_parser;
	AAC* m_compiler;

	std::ostream* m_outStream;

};

AAVM* AANewVM();
