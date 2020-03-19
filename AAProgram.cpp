#include "AAProgram.h"

AAProgram::AAProgram() {
	m_entryPoint = -1;
	m_procedureCount = -1;
	m_procedures = 0;
	m_signatureCount = -1;
	m_exportedSignatures = 0;
}

bool AAProgram::LoadProgram(unsigned char* bytes, unsigned long long length) {

	aa::bwalker bw = aa::bwalker(bytes, length);

	bw >> m_signatureCount;

	m_exportedSignatures = new ExportSignature[m_signatureCount];

	for (int i = 0; i < m_signatureCount; i++) {

		bw >> m_exportedSignatures[i].name;
		bw >> m_exportedSignatures[i].procID;

	}

	bw >> m_procedureCount;
	bw >> m_entryPoint;

	m_procedures = new Procedure[m_procedureCount];

	for (int i = 0; i < m_procedureCount; i++) {

		// Load constants for procedure
		LoadConstants(m_procedures[i], bw);

		// Load operations for procedure
		LoadOperations(m_procedures[i], bw);

	}

	return true;

}

void AAProgram::LoadConstants(Procedure& proc, aa::bwalker& bw) {

	int litCount;
	bw >> litCount;

	proc.constTable = new AA_Literal[litCount];

	for (int i = 0; i < litCount; i++) {

		unsigned char charType;
		bw >> charType;

		proc.constTable[i].tp = (AALiteralType)charType;

		switch (proc.constTable[i].tp) {
		case AALiteralType::Int:
			bw >> proc.constTable[i].lit.i.val;
			break;
		case AALiteralType::String: {
			std::wstring ws;
			bw >> ws;
			proc.constTable[i].lit.s.len = ws.length();
			proc.constTable[i].lit.s.val = new wchar_t[proc.constTable[i].lit.s.len];
			wcscpy(proc.constTable[i].lit.s.val, ws.c_str());			
			break;
		}
		case AALiteralType::Float:
			bw >> proc.constTable[i].lit.f.val;
			break;
		case AALiteralType::Boolean:
			bw >> proc.constTable[i].lit.b.val;
			break;
		case AALiteralType::Char:
			bw >> proc.constTable[i].lit.c.val;
			break;
		default:
			break;
		}

	}

	int varCount;
	bw >> varCount;

	proc.venv = new AAVarEnv;

	for (int i = 0; i < varCount; i++) {

		int varID;
		bw >> varID;

		proc.venv->DeclareVariable(varID);

	}

}

void AAProgram::LoadOperations(Procedure& proc, aa::bwalker& bw) {

	bw >> proc.opCount;

	proc.opSequence = new AAO[proc.opCount];

	for (int i = 0; i < proc.opCount; i++) {

		unsigned char opCode;
		bw >> opCode;

		proc.opSequence[i].op = (AAByteCode)opCode;

		switch (proc.opSequence[i].op) {
		case AAByteCode::PUSHC:
		case AAByteCode::GETVAR:
		case AAByteCode::SETVAR:
		case AAByteCode::RET:
		case AAByteCode::JMP:
		case AAByteCode::JMPF:
		case AAByteCode::JMPT:
		case AAByteCode::HALLOC:
		case AAByteCode::SALLOC:
		case AAByteCode::GETFIELD:
		case AAByteCode::SETFIELD:
			proc.opSequence[i].args = new int[1];
			bw >> proc.opSequence[i].args[0];
			break;
		case AAByteCode::CALL:
		case AAByteCode::VMCALL:
			proc.opSequence[i].args = new int[2];
			bw >> proc.opSequence[i].args[0];
			bw >> proc.opSequence[i].args[1];
			break;
		default:
			proc.opSequence[i].args = 0;
			break;
		}

	}

}
