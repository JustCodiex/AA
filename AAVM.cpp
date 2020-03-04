#include "AAVM.h"
#include "AAB2F.h"
#include <ctime>

AAVM* AAVM::CreateNewVM(bool logExecuteTime, bool logCompiler) {
	AAVM* vm = new AAVM();
	vm->m_logCompileMessages = logCompiler;
	vm->m_logExecTime = logExecuteTime;
	return vm;
}

AAVM::AAVM() {

	m_compiler = new AAC;
	m_parser = new AAP;
	m_outStream = 0;

	m_logCompileMessages = false;
	m_logExecTime = false;

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

AAC_Out AAVM::CompileExpressionToFile(std::wstring input, std::wstring outputfile) {

	// Generated AST from input
	std::vector<AA_AST*> trees = m_parser->Parse(input);

	// Set compiler output file
	m_compiler->SetOpListFile(outputfile + L"op.txt");

	// Compile all procedures into bytecode
	AAC_Out bytecode = m_compiler->CompileFromAbstractSyntaxTrees(trees);

	// Dump bytecode
	aa::dump_bytecode(outputfile, bytecode);

	// Aslo return bytecode (so we can execute it directly)
	return bytecode;

}

AAC_Out AAVM::CompileFileToFile(std::wstring fileIn, std::wstring fileOut) { // TODO: Implement

	// Generated AST from input
	//AA_AST* tree = m_parser->Parse(L"");

	// Generated bytecode
	AAC_Out bytecode; //= m_compiler->CompileFromAST(0);

	return bytecode;

}

void AAVM::RunFile(std::wstring file) {



}

void AAVM::RunExpression(std::wstring input) {

	// Generated AST from input
	//AA_AST* tree = m_parser->Parse(input);

	// Generated bytecode
	AAC_Out bytecode;// = m_compiler->CompileFromAST(0);

	// Execute the bytecode
	Execute(bytecode.bytes, bytecode.length);

}

void AAVM::Execute(AAC_Out bytecode) {
	this->Execute(bytecode.bytes, bytecode.length);
}

void AAVM::Execute(unsigned char* bytes, unsigned long long len) {

	// Load program so we can execute it
	AAProgram* pProg = new AAProgram;
	if (pProg->LoadProgram(bytes, len)) {

		// Run the program
		this->Run(pProg);

	}

}

void AAVM::Run(AAProgram* pProg) {

	// Get entry point
	int entryPoint = pProg->GetEntryPoint();

	// Log start time
	clock_t s = clock();

	// Run the program from entry point
	this->Run(pProg->m_procedures, entryPoint);

	// Should we log execution?
	if (m_logExecTime) {

		// Print execution time
		printf("Execute time: %fs\n", (float)(clock() - s) / CLOCKS_PER_SEC);

	}

}

#define AAVM_GetOperation(i) procedure[procPointer].opSequence[i].op
#define AAVM_GetArgument(i) procedure[procPointer].opSequence[opPointer].args[i]

void AAVM::Run(AAProgram::Procedure* procedure, int entry) {

	int opPointer = 0;
	int procPointer = entry;
	aa::stack<AAVal> stack;

	while (opPointer < procedure[procPointer].opCount) {

		switch (AAVM_GetOperation(opPointer)) {
		case AAByteCode::PUSHC:
			stack.Push(procedure[procPointer].constTable[AAVM_GetArgument(0)]);
			opPointer++;
			break;
		case AAByteCode::ADD: {
			AA_Literal rhs = stack.Pop().litVal;
			AA_Literal lhs = stack.Pop().litVal;
			AA_Literal r = lhs + rhs;
			stack.Push(r);
			opPointer++;
			break;
		}
		case AAByteCode::SUB: {
			AA_Literal rhs = stack.Pop().litVal;
			AA_Literal lhs = stack.Pop().litVal;
			AA_Literal r = lhs - rhs;
			stack.Push(r);
			opPointer++;
			break;
		}
		case AAByteCode::MUL: {
			AA_Literal rhs = stack.Pop().litVal;
			AA_Literal lhs = stack.Pop().litVal;
			AA_Literal r = lhs * rhs;
			stack.Push(r);
			opPointer++;
			break;
		}
		case AAByteCode::DIV: {
			AA_Literal rhs = stack.Pop().litVal;
			AA_Literal lhs = stack.Pop().litVal;
			stack.Push(lhs / rhs);
			opPointer++;
			break;
		}
		case AAByteCode::MOD: {
			AA_Literal rhs = stack.Pop().litVal;
			AA_Literal lhs = stack.Pop().litVal;
			stack.Push(lhs % rhs);
			opPointer++;
			break;
		}
		case AAByteCode::NNEG: {
			stack.Push(-stack.Pop().litVal);
			opPointer++;
			break;
		}
		case AAByteCode::LNEG: {
			stack.Push(!stack.Pop().litVal);
			opPointer++;
			break;
		}
		case AAByteCode::GETVAR: {
			stack.Push(procedure[procPointer].venv->GetVariable(AAVM_GetArgument(0)));
			opPointer++;
			break;
		}
		case AAByteCode::SETVAR: {
			AA_Literal rhs = stack.Pop().litVal;
			procedure[procPointer].venv->SetVariable(AAVM_GetArgument(0), rhs);
			opPointer++;
			break;
		}
		case AAByteCode::CALL: {

			int callProc = AAVM_GetArgument(0);
			int argCount = AAVM_GetArgument(1);

			aa::stack<AAVal> args;
			for (int i = 0; i < argCount; i++) {
				args.Push(stack.Pop());
			}

			stack.Push(AAVal(procPointer)); // Push pointer to current proc
			stack.Push(AAVal(opPointer + 1)); // push pointer to next operation

			procPointer = callProc;
			opPointer = 0;

			while (args.Size() > 0) {
				stack.Push(args.Pop());
			}

			break;
		}
		case AAByteCode::RET: {
			int retCount = AAVM_GetArgument(0);
			aa::stack<AAVal> returnValues;
			for (int i = 0; i < retCount; i++) {
				returnValues.Push(stack.Pop());
			}
			opPointer = stack.Pop().litVal.lit.i.val;
			procPointer = stack.Pop().litVal.lit.i.val;
			for (int i = 0; i < retCount; i++) {
				stack.Push(returnValues.Pop());
			}
			break;
		}
		case AAByteCode::NOP:
		default:
			opPointer++;
			break;
		}

	}

	this->ReportStack(stack);

}

void AAVM::ReportStack(aa::stack<AAVal> stack) {

	if (stack.Size() == 1) {
		if (m_outStream) {
			AA_Literal litVal = stack.Pop().litVal;
			if (litVal.tp == AALiteralType::Int) {
				m_outStream->operator<<(litVal.lit.i.val);
			} else if (litVal.tp == AALiteralType::Float) {
				std::string fs = std::to_string(litVal.lit.f.val);
				m_outStream->write(fs.c_str(), fs.length());
			} else if (litVal.tp == AALiteralType::Boolean) {
				if (litVal.lit.b.val) {
					m_outStream->write("true", 4);
				} else {
					m_outStream->write("false", 5);
				}
			}
			m_outStream->write("\n", 1);
		}
	}

}
