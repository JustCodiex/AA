#include "AAVM.h"
#include "AAB2F.h"
#include <ctime>

AAVM* AANewVM() {
	return new AAVM;
}

AAVM::AAVM() {

	m_compiler = new AAC;
	m_parser = new AAP;
	m_outStream = 0;

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
	std::vector<AAC::CompiledProcedure> compileResults;

	// Compile all the trees into individual procedures
	for (size_t i = 0; i < trees.size(); i++) {

		// Generated bytecode
		compileResults.push_back(m_compiler->CompileProcedureFromAST(trees[i]));

	}

	// Compile all procedures into bytecode
	AAC_Out bytecode = m_compiler->CompileFromProcedures(compileResults);

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

	//int opPointer = 0;
	//aa::stack<AAVal> stack;

	//clock_t s = clock();

	this->Run(pProg->m_procedures[0].constTable, pProg->m_procedures[0].venv, pProg->m_procedures[0].opSequence, pProg->m_procedures[0].opCount);


	//printf("Execute time: %fs\n", (float)(clock() - s) / CLOCKS_PER_SEC);

}

void AAVM::Run(AA_Literal* cenv, AAVarEnv* venv, AAO* ops, int opCount) {

	int opPointer = 0;
	aa::stack<AAVal> stack;

	while (opPointer < opCount) {

		switch (ops[opPointer].op) {
		case AAByteCode::PUSHC:
			stack.Push(cenv[ops[opPointer].args[0]]);
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
			stack.Push(venv->GetVariable(ops[opPointer].args[0]));
			opPointer++;
			break;
		}
		case AAByteCode::SETVAR: {
			AA_Literal rhs = stack.Pop().litVal;
			venv->SetVariable(ops[opPointer].args[0], rhs);
			opPointer++;
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
