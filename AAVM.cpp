#include "AAVM.h"
#include "stack.h"

AAVM* AANewVM() {
	return new AAVM;
}

AAVM::AAVM() {

	m_compiler = new AAC;
	m_parser = new AAP;

}

void AAVM::Release() {

	// Delete compiler
	delete m_compiler;
	m_compiler = 0;

	// Delete parser
	m_parser->Release();
	delete m_parser;
	m_parser = 0;

}

void AAVM::Execute(std::wstring file) {



}

void AAVM::Execute(unsigned char* bytes, unsigned long long len) {

	// The operation count
	int opCount;

	// Operation list and constants table
	AAO* ops = 0; AA_Literal* constants = 0;

	// Create the execution environment
	CreateExecutionEnvironment(bytes, len, constants, ops, opCount);

	// Run
	Run(constants, ops, opCount);

}

void AAVM::Run(AA_Literal* lit, AAO* ops, int opCount) {

	size_t opPointer = 0;
	aa::stack<AA_Literal> stack;

	while (opPointer < opCount) {

		switch (ops[opPointer].op) {
		case AAByteCode::PUSH_CONST:
			stack.Push(lit[ops[opPointer].args[0]]);
			opPointer++;
			break;
		case AAByteCode::ADD: {
			AA_Literal r = stack.At(ops[opPointer].args[0]) + stack.At(ops[opPointer].args[1]);
			stack.Pop();
			stack.Pop();
			stack.Push(r);
			opPointer++;
			break;
		}
		case AAByteCode::NOP:
		default:
			opPointer++;
			break;
		}

	}

	if (stack.Size() == 1) {
		printf(std::to_string(stack.Pop().lit.i.val).c_str());
	}

}

void AAVM::CreateExecutionEnvironment(unsigned char* bytes, unsigned long long len, AA_Literal*& constants, AAO*& oplist, int& opCount) {

	unsigned int opOffset;
	constants = LoadConstsTable(bytes, len, opOffset);
	oplist = LoadOpSequence(bytes + opOffset, len, opCount);

}

AA_Literal* AAVM::LoadConstsTable(unsigned char* bytes, unsigned long long len, unsigned int& offset) {

	int count = 0;
	memcpy(&count, bytes, 4);

	offset = 4;
	AA_Literal* table = new AA_Literal[count];

	for (int i = 0; i < count; i++) {

		unsigned char type;
		memcpy(&type, bytes + offset, 1);

		offset++;
		table[i].tp = (AALiteralType)type;

		switch (table[i].tp) {
		case AALiteralType::Int:
			memcpy(&table[i].lit.i.val, bytes + offset, 4);
			offset += 4;
			break;
		case AALiteralType::String:
			break;
		case AALiteralType::Float:
			break;
		case AALiteralType::Boolean:
			break;
		default:
			break;
		}

	}

	return table;

}

AAO* AAVM::LoadOpSequence(unsigned char* bytes, unsigned long long len, int& count) {

	unsigned long long offset = 0;

	count = 0;
	memcpy(&count, bytes, 4);
	offset += 4;

	AAO* opSequence = new AAO[count];

	for (int i = 0; i < count; i++) {

		unsigned char opchar;
		memcpy(&opchar, bytes + offset, 1);
		offset += 1;

		opSequence[i].op = (AAByteCode)opchar;

		switch (opSequence[i].op) {
		case AAByteCode::NOP:
			opSequence[i].args = 0;
			break;
		case AAByteCode::PUSH_CONST:
			opSequence[i].args = new int[1];
			memcpy(opSequence[i].args, bytes + offset, 4);
			offset += 4;
			break;
		case AAByteCode::ADD:
			opSequence[i].args = new int[3];
			memcpy(opSequence[i].args, bytes + offset, 4 * 3);
			offset += 4 * 3;
			break;
		default:
			break;
		}

	}

	return opSequence;

}
