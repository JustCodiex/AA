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

	struct ExportSignature {
		std::wstring name;
		int procID;
	};

public:

	AAProgram();

	bool LoadProgram(unsigned char* bytes, unsigned long long length);

	int GetEntryPoint() { return m_entryPoint; }

private:

	void LoadConstants(Procedure& proc, aa::bwalker& bw);
	void LoadOperations(Procedure& proc, aa::bwalker& bw);

private:

	int m_entryPoint;
	int m_procedureCount;
	int m_signatureCount;
	Procedure* m_procedures;
	ExportSignature* m_exportedSignatures;

};
