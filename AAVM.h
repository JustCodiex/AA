#pragma once

#include "AAC.h"
#include "AAP.h"

class AAVM {

public:

	AAVM();

	void Release();

	//void Execute();
	
	AAP* GetParser() { return m_parser; }

	AAC* GetCompiler() { return m_compiler; }

private:

	AAP* m_parser;
	AAC* m_compiler;

};

AAVM* AANewVM();
