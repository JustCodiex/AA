#pragma once

#include "AAC.h"
#include "AAP.h"
#include "AAO.h"

class AAVM {

public:

	AAVM();

	void Release();

	void Execute(std::wstring file);
	
	void Execute(unsigned char* bytes, unsigned long long len);

	AAP* GetParser() { return m_parser; }

	AAC* GetCompiler() { return m_compiler; }

private:

	AA_Literal* LoadConstsTable(unsigned char* bytes, unsigned long long len, unsigned int& offset);
	AAO* LoadOpSequence(unsigned char* bytes, unsigned long long len, int& count);

	void CreateExecutionEnvironment(unsigned char* bytes, unsigned long long len, AA_Literal*& constants, AAO*& oplist, int& opCount);

	void Run(AA_Literal* lit, AAO* ops, int opCount);

private:

	AAP* m_parser;
	AAC* m_compiler;

};

AAVM* AANewVM();
