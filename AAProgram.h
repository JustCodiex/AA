#pragma once
#include "bwalker.h"
#include "AAO.h"
#include "AAVarEnv.h"
#include "AARuntimeTypeEnv.h"

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

	int GetEntryPoint() { return m_entryPoint; }

	AAStaticTypeEnvironment* GetTypeEnvironment();

private:

	void LoadConstants(Procedure& proc, aa::bwalker& bw);
	void LoadOperations(Procedure& proc, aa::bwalker& bw);
	void LoadType(aa::bwalker& bw);

private:

	unsigned char m_compiledWithVersion[10] = {};

	int m_entryPoint;
	int m_procedureCount;
	int m_signatureCount;

	Procedure* m_procedures;
	AAStaticTypeEnvironment* m_types;

};
