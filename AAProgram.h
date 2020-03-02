#pragma once
#include "bwalker.h"
#include "AAO.h"
#include "AAVarEnv.h"

class AAProgram {
	friend class AAVM; // Allow the virtual machine to access private parts of class

private:

	struct Procedure {
		AA_Literal* constTable;
		AAVarEnv* venv;
		AAO* opSequence;
		int opCount;
	};

public:

	AAProgram();

	bool LoadProgram(unsigned char* bytes, unsigned long long length);

private:

	void LoadConstants(Procedure& proc, aa::bwalker& bw);
	void LoadOperations(Procedure& proc, aa::bwalker& bw);

private:

	int m_entryPoint;
	int m_procedureCount;
	Procedure* m_procedures;

};
