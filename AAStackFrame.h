#pragma once
#include "AAO.h"
#include "AAVarEnv.h"

class AAStackFrame {
	friend class AAVM;

public:

	AA_Literal* constTable;
	AAO* operations;
	unsigned int opCount;
	signed int opPtr;
	unsigned int callCount;

	AAVarEnv* varEnv;
	AAVarEnv* venv;

public:

	void Load();

	void Exit();

};
