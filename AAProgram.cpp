#include "AAProgram.h"
#include "AAByteType.h"

AAProgram::AAProgram() {
	m_entryPoint = -1;
	m_procedureCount = -1;
	m_procedures = 0;
	m_signatureCount = -1;
	m_types = 0;
}

bool AAProgram::LoadProgram(unsigned char* bytes, unsigned long long length) {

	aa::bwalker bw = aa::bwalker(bytes, length);

	// Read version program was compiled with
	bw.read_bytes(m_compiledWithVersion, 10);

	// TODO: Add version compare to make sure we can run it

	// Fetch procedure count and entry point
	bw >> m_procedureCount;
	bw >> m_entryPoint;

	// Allocate procedure array
	m_procedures = new Procedure[m_procedureCount];

	// Load procedures
	for (int i = 0; i < m_procedureCount; i++) {

		// Load constants for procedure
		LoadConstants(m_procedures[i], bw);

		// Load operations for procedure
		LoadOperations(m_procedures[i], bw);

	}

	// Amount of types used by program
	int typecount;

	// Load type count
	bw >> typecount;

	// Allocate type environment
	m_types = new AAStaticTypeEnvironment(typecount);

	// Load types
	for (int i = 0; i < typecount; i++) {

		// Load type
		LoadType(bw);

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
		case AAByteCode::PUSHWS:
		case AAByteCode::GETVAR:
		case AAByteCode::JMP:
		case AAByteCode::JMPF:
		case AAByteCode::JMPT:
		case AAByteCode::ALLOC:
		case AAByteCode::ADD:
		case AAByteCode::DIV:
		case AAByteCode::GE:
		case AAByteCode::GEQ:
		case AAByteCode::GETELEM:
		case AAByteCode::SETELEM:
		case AAByteCode::LE:
		case AAByteCode::LEQ:
		case AAByteCode::MOD:
		case AAByteCode::MUL:
		case AAByteCode::NNEG:
		case AAByteCode::LNEG:
		case AAByteCode::POP:
		case AAByteCode::SUB:
		case AAByteCode::WRAP:
		case AAByteCode::CMPE:
		case AAByteCode::CMPNE:
		case AAByteCode::TUPLEGET:
			proc.opSequence[i].args = new int[1];
			bw >> proc.opSequence[i].args[0];
			break;
		case AAByteCode::GETFIELD:
		case AAByteCode::SETFIELD:
		case AAByteCode::SETVAR:
		case AAByteCode::CALL:
		case AAByteCode::XCALL:
		case AAByteCode::VCALL:
		case AAByteCode::ALLOCARRAY:
			proc.opSequence[i].args = new int[2];
			bw >> proc.opSequence[i].args[0];
			bw >> proc.opSequence[i].args[1];
			break;
		case AAByteCode::CTOR:
			proc.opSequence[i].args = new int[3];
			bw >> proc.opSequence[i].args[0];
			bw >> proc.opSequence[i].args[1];
			bw >> proc.opSequence[i].args[2];
			break;
		/*case AAByteCode::BCKM:
			proc.opSequence[i].args = new int[4];
			bw >> proc.opSequence[i].args[0];
			bw >> proc.opSequence[i].args[1];
			bw >> proc.opSequence[i].args[2];
			bw >> proc.opSequence[i].args[3];
			break;*/
		case AAByteCode::TUPLENEW: {
			int tupleSize = 0;
			bw >> tupleSize;
			proc.opSequence[i].args = new int[tupleSize + 1];
			proc.opSequence[i].args[0] = tupleSize;
			for (int j = 0; j < tupleSize; j++) {
				bw >> proc.opSequence[i].args[j + 1];
			}
			break;
		}
		default:
			proc.opSequence[i].args = 0;
			break;
		}

	}

}

void AAProgram::LoadType(aa::bwalker& bw) {

	std::wstring typeName = bw.read_cstring();
	uint16_t basePtr = UINT16_MAX, typesize = 0;
	unsigned char typeVariant = 0, extendCount = 0, hasVtable = 0;
	uint16_t* extendPtrs = 0;
	uint32_t constUID;

	// Read in base data
	bw >> typeVariant;
	bw >> typesize;
	bw >> basePtr;
	bw >> constUID;
	bw >> extendCount;

	// Alloc type
	AAObjectType* pType = new AAObjectType(typeName, typesize, constUID, basePtr);

	// Allocate memory for additional extensions
	extendPtrs = new uint16_t[extendCount];
	
	// Loop through all extensions
	for (size_t i = 0; i < extendCount; i++) {
		bw >> extendPtrs[i];
	}

	// Add inheritances
	pType->SetInheritance(extendCount, extendPtrs);

	// If reference type
	if ((AAByteTypeVariant)typeVariant == AAByteTypeVariant::Ref) {

		// Read if we have a vtable
		bw >> hasVtable;

		// Read vtable if found
		if (hasVtable) {

			// Create VTable
			AAVTable* pTypeVTable = new AAVTable;

			uint16_t vtablelen, vtablefunc, vtablefunclen, vtablesub; // VTable length (virtual functions), VTable function index, vTable function substitute
			uint32_t vtablecptr; // VTable class pointer

			// Read VTable length
			bw >> vtablelen;

			// For all elements in VTable
			for (uint16_t i = 0; i < vtablelen; i++) {

				// Load through all vtables
				bw >> vtablefunc;
				bw >> vtablefunclen;

				// Loop through all potential overrides
				for (uint16_t j = 0; j < vtablefunclen; j++) {

					// Read tuple
					bw >> vtablecptr;
					bw >> vtablesub;

					// Register
					pTypeVTable->SetFunctionPtr(vtablefunc, vtablecptr, vtablesub);

				}

			}

			// Set the VTable
			pType->SetVTable(pTypeVTable);

		}

	}

	// Insert type
	m_types->InsertType(constUID, pType);

}

AAStaticTypeEnvironment* AAProgram::GetTypeEnvironment() {
	return m_types;
}
