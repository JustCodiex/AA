#include "AAProgram.h"
#include "AAByteType.h"
#include "list.h"
#include <fstream>

AAProgram::AAProgram() {
	m_entryPoint = -1;
	m_procedureCount = -1;
	this->m_stackframes = 0;
	m_signatureCount = -1;
	m_types = 0;
}

void AAProgram::Release() {

	// Delete self
	delete this;

}

bool AAProgram::LoadProgram(std::wstring path) {

	// Binary stream
	std::ifstream bin = std::ifstream(path, std::ios::in | std::ios::ate);

	// If we failed to open
	if (!bin.is_open()) {
		return false;
	}

	// Read file size
	uint32_t len = (uint32_t)bin.tellg();

	// Allocate memory
	char* binaryContent = new char[len+1];

	// Seek start
	bin.seekg(0, std::ios::beg);

	// Read into buffer
	bin.read(binaryContent, len);

	// Close the file
	bin.close();

	// Return result of parse
	return this->LoadProgram(reinterpret_cast<unsigned char*>(binaryContent), len);

}

bool AAProgram::LoadProgram(unsigned char* bytes, unsigned long long length) {

	// Scoped binary walker
	aa::bwalker bw = aa::bwalker(bytes, length);

	// Read version program was compiled with
	bw.read_bytes(m_compiledWithVersion, 10);

	// TODO: Add version compare to make sure we can run it

	// Fetch procedure count and entry point
	bw >> m_procedureCount;
	bw >> m_entryPoint;

	// Allocate procedure array
	this->m_stackframes = new AAStackFrame[m_procedureCount];

	// Load procedures
	for (int i = 0; i < m_procedureCount; i++) {

		// Load constants for procedure
		LoadConstants(this->m_stackframes[i], bw);

		// Load operations for procedure
		LoadOperations(this->m_stackframes[i], bw);

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

	// Close binary walker and release resources
	bw.close_and_release();

	return true;

}

void AAProgram::LoadConstants(AAStackFrame& proc, aa::bwalker& bw) {

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

	proc.varEnv = new AAVarEnv;

	for (int i = 0; i < varCount; i++) {

		int varID;
		bw >> varID;

		proc.varEnv->DeclareVariable(varID);

	}

}

void AAProgram::LoadOperations(AAStackFrame& proc, aa::bwalker& bw) {

	bw >> proc.opCount;

	proc.operations = new AAO[proc.opCount];

	for (int i = 0; i < proc.opCount; i++) {

		unsigned char opCode;
		bw >> opCode;

		proc.operations[i].op = (AAByteCode)opCode;

		switch (proc.operations[i].op) {
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
		case AAByteCode::TAGTUPLECMP:
		case AAByteCode::LAND:
		case AAByteCode::LOR:
		case AAByteCode::BINAND:
		case AAByteCode::BINOR:
			proc.operations[i].args = new int[1];
			bw >> proc.operations[i].args[0];
			break;
		case AAByteCode::GETFIELD:
		case AAByteCode::SETFIELD:
		case AAByteCode::SETVAR:
		case AAByteCode::CALL:
		case AAByteCode::XCALL:
		case AAByteCode::VCALL:
		case AAByteCode::ALLOCARRAY:
		case AAByteCode::INC:
		case AAByteCode::DEC:
			proc.operations[i].args = new int[2];
			bw >> proc.operations[i].args[0];
			bw >> proc.operations[i].args[1];
			break;
		case AAByteCode::CTOR:
			proc.operations[i].args = new int[3];
			bw >> proc.operations[i].args[0];
			bw >> proc.operations[i].args[1];
			bw >> proc.operations[i].args[2];
			break;
		case AAByteCode::TUPLECMPORSET:
		case AAByteCode::TUPLENEW: { // Read arguments of the form (n, arg_0, ..., arg_n)
			int sz = 0;
			bw >> sz;
			proc.operations[i].args = new int[sz + 1];
			proc.operations[i].args[0] = sz;
			for (int j = 0; j < sz; j++) {
				bw >> proc.operations[i].args[j + 1];
			}
			break;
		}
		case AAByteCode::TAGTUPLECMPORSET: { // Read arguments of the form (n, q, arg_0, ..., arg_n)
			int sz = 0;
			bw >> sz;
			proc.operations[i].args = new int[sz + 2];
			proc.operations[i].args[0] = sz;
			bw >> proc.operations[i].args[1];
			for (int j = 0; j < sz; j++) {
				bw >> proc.operations[i].args[j + 2];
			}
			break;
		}
		default:
			proc.operations[i].args = 0;
			break;
		}

	}

}

void AAProgram::LoadType(aa::bwalker& bw) {

	std::wstring typeName = bw.read_cstring();
	uint16_t basePtr = UINT16_MAX, typesize = 0;
	unsigned char typeVariant = 0, extendCount = 0, hasVtable = 0, hasTagFields = 0;
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

		// Read if there's a field for tagged variables
		bw >> hasTagFields;

		// Do we have any tagged fields?
		if (hasTagFields) {

			// Field id
			uint16_t taggedCount = 0, fieldId = 0;
			unsigned char fieldType = 0;
			uint32_t fieldrefType = 0;

			// Read tag count
			bw >> taggedCount;

			// Update type to tagged count
			pType->SetTaggedCount(taggedCount);

			// Read tagged field count
			for (uint16_t i = 0; i < taggedCount; i++) {

				// Read essentials
				bw >> fieldId;
				bw >> fieldType;

				// If ref type, read pointer as well
				if (fieldType == 14) {
					bw >> fieldrefType;
				}

				// Set tag data
				pType->SetTaggedField(i, fieldId, fieldType, fieldrefType);

			}

		}

	}

	// Insert type
	m_types->InsertType(constUID, pType);

}

AAStaticTypeEnvironment* AAProgram::GetTypeEnvironment() {
	return m_types;
}
