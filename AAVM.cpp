#include "AAVM.h"
#include "AAB2F.h"
#include "AARuntimeEnvironment.h"
#include <ctime>

AAVM* AAVM::CreateNewVM(bool logExecuteTime, bool logCompiler, bool logTopStack) {
	AAVM* vm = new AAVM();
	vm->m_logCompileMessages = logCompiler;
	vm->m_logExecTime = logExecuteTime;
	vm->m_logTopOfStackAfterExec = logTopStack;
	return vm;
}

AAVM::AAVM() {

	m_compiler = new AAC;

	if (m_compiler) {
		m_compiler->SetupCompiler();
	}

	m_parser = new AAP;
	m_outStream = 0;

	m_logTopOfStackAfterExec = false;
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

AAVal AAVM::CompileAndRunExpression(std::wstring input) {
	return this->CompileAndRunExpression(input, L"", L"");
}

AAVal AAVM::CompileAndRunExpression(std::wstring input, std::wstring binaryoutputfile, std::wstring formattedoutputfile) {

	// Generated AST from input
	std::vector<AA_AST*> trees = m_parser->Parse(input);

	// Compile and run the AST
	return this->CompileAndRun(trees, binaryoutputfile, formattedoutputfile);	

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

AAVal AAVM::CompileAndRunFile(std::wstring sourcefile) {
	return this->CompileAndRunFile(sourcefile, L"", L"");
}

AAVal AAVM::CompileAndRunFile(std::wstring sourcefile, std::wstring binaryoutputfile, std::wstring formattedoutputfile) {

	// Generated AST from input
	std::vector<AA_AST*> trees = m_parser->Parse(std::wifstream(sourcefile));

	// Compile and run the AST
	return this->CompileAndRun(trees, binaryoutputfile, formattedoutputfile);

}

AAVal AAVM::CompileAndRun(std::vector<AA_AST*> trees, std::wstring binaryoutputfile, std::wstring formattedoutputfile) {

	// Set compiler output file
	m_compiler->SetOpListFile(formattedoutputfile);

	// Compile all procedures into bytecode
	AAC_CompileResult compileResult = m_compiler->CompileFromAbstractSyntaxTrees(trees);

	// Did we compile without error?
	if (COMPILE_SUCESS(compileResult)) {

		// Only dump bytecode if a filepath is specified
		if (binaryoutputfile != L"") {

			// Dump bytecode
			aa::dump_bytecode(binaryoutputfile, compileResult.result);

		}

		// Cleanup trees
		m_parser->ClearTrees(trees);

		// Execute the bytecode
		return this->Execute(compileResult.result);

	} else {

		// Write compiler error
		this->WriteCompilerError(compileResult.firstMsg);

		// Return null
		return AAVal::Null;

	}

}

AAVal AAVM::Execute(AAC_Out bytecode) {
	return this->Execute(bytecode.bytes, bytecode.length);
}

AAVal AAVM::Execute(unsigned char* bytes, unsigned long long len) {

	// Load program so we can execute it
	AAProgram* pProg = new AAProgram;
	if (pProg->LoadProgram(bytes, len)) {

		// Run the program
		return this->Run(pProg);

	} else {

		// Return AAVal Error
		return AAVal(L"");

	}

}

AAVal AAVM::Run(AAProgram* pProg) {

	// Get entry point
	int entryPoint = pProg->GetEntryPoint();

	// Log start time
	clock_t s = clock();

	// Run the program from entry point
	AAVal v = this->Run(pProg->m_procedures, entryPoint);

	// Should we log execution?
	if (m_logExecTime) {

		// Print execution time
		printf("Execute time: %fs\n", (float)(clock() - s) / CLOCKS_PER_SEC);

	}

	return v;

}

#define AAVM_VENV execp.venv
#define AAVM_OPI execp.opPointer
#define AAVM_PROC execp.procPointer

#define AAVM_CURRENTOP procedure[AAVM_PROC].opSequence[AAVM_OPI].op
#define AAVM_GetArgument(i) procedure[AAVM_PROC].opSequence[AAVM_OPI].args[i]

AAVal AAVM::Run(AAProgram::Procedure* procedure, int entry) {

	aa::stack<AAVal> stack;
	aa::stack<AARuntimeEnvironment> callstack;

	AARuntimeEnvironment execp;
	execp.opPointer = 0;
	execp.procPointer = entry;
	execp.venv = procedure[entry].venv->CloneSelf();

	while (AAVM_OPI < procedure[AAVM_PROC].opCount) {
		switch (AAVM_CURRENTOP) {
		case AAByteCode::ADD:
		case AAByteCode::CMPE:
		case AAByteCode::CMPNE:
		case AAByteCode::DIV:
		case AAByteCode::GE:
		case AAByteCode::GEQ:
		case AAByteCode::LE:
		case AAByteCode::LEQ:
		case AAByteCode::MOD:
		case AAByteCode::MUL:
		case AAByteCode::SUB:
		{
			AA_Literal rhs = stack.Pop().litVal;
			AA_Literal lhs = stack.Pop().litVal;
			stack.Push(BinaryOperation(AAVM_CURRENTOP, lhs, rhs));
			AAVM_OPI++;
			break;
		}
		case AAByteCode::PUSHC: {
			int p = AAVM_GetArgument(0);
			stack.Push(procedure[AAVM_PROC].constTable[p]);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::NNEG: {
			stack.Push(-stack.Pop().litVal);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::LNEG: {
			stack.Push(!stack.Pop().litVal);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::GETVAR: {
			stack.Push(AAVM_VENV->GetVariable(AAVM_GetArgument(0)));
			AAVM_OPI++;
			break;
		}
		case AAByteCode::SETVAR: {
			AAVal rhs = stack.Pop();
			AAVM_VENV->SetVariable(AAVM_GetArgument(0), rhs);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::CALL: {

			int callProc = AAVM_GetArgument(0);
			int argCount = AAVM_GetArgument(1);

			aa::stack<AAVal> args;
			for (int i = 0; i < argCount; i++) {
				args.Push(stack.Pop());
			}

			AAVM_OPI++;
			callstack.Push(execp);

			AAVM_VENV = procedure[callProc].venv->CloneSelf();
			AAVM_PROC = callProc;
			AAVM_OPI = 0;

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

			delete execp.venv;
			execp = callstack.Pop();

			for (int i = 0; i < retCount; i++) {
				stack.Push(returnValues.Pop());
			}
			break;
		}
		case AAByteCode::JMPF: {
			if (stack.Pop().litVal.lit.b.val) {
				AAVM_OPI++;
			} else {
				AAVM_OPI += 1 + AAVM_GetArgument(0); // jump to next (if-else or else) or next statement after block
			}
			break;
		}
		case AAByteCode::JMPT: {
			if (!stack.Pop().litVal.lit.b.val) {
				AAVM_OPI++;
			} else {
				AAVM_OPI += 1 + AAVM_GetArgument(0); // jump to next (if-else or else) or next statement after block
			}
			break;
		}
		case AAByteCode::JMP: {
			AAVM_OPI += 1 + AAVM_GetArgument(0);
			break;
		}
		case AAByteCode::ALLOC: {
			int allocsz = AAVM_GetArgument(0);
			AAVal allocobj = AAVal(AllocAAO((size_t)allocsz));
			stack.Push(allocobj);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::GETFIELD: {
			AAVal o = stack.Pop();
			stack.Push(o.obj->values[AAVM_GetArgument(0)]);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::SETFIELD: {
			AAVal rhs = stack.Pop();
			AAVal o = stack.Pop();
			o.obj->values[AAVM_GetArgument(0)] = rhs;
			AAVM_OPI++;
			break;
		}
		case AAByteCode::NOP:
		default:
			AAVM_OPI++;
			break;
		}

	}

	// Return whatever's on top of the stack
	return this->ReportStack(stack);

}

AAVal AAVM::BinaryOperation(AAByteCode op, AA_Literal lhs, AA_Literal rhs) {
	switch (op) {
	case AAByteCode::ADD: {
		return lhs + rhs;
	}
	case AAByteCode::SUB: {
		return lhs - rhs;
	}
	case AAByteCode::MUL: {
		return lhs * rhs;
	}
	case AAByteCode::DIV: {
		return lhs / rhs;
	}
	case AAByteCode::MOD: {
		return lhs % rhs;
	}
	case AAByteCode::CMPE: {
		return lhs == rhs;
	}
	case AAByteCode::CMPNE: {
		return lhs != rhs;
	}
	case AAByteCode::LE: {
		return lhs < rhs;
	}
	case AAByteCode::LEQ: {
		return lhs <= rhs;
	}
	case AAByteCode::GE: {
		return lhs > rhs;
	}
	case AAByteCode::GEQ: {
		return lhs >= rhs;
	}
	default:
		return AAVal(0);
	}
}

AAVal AAVM::ReportStack(aa::stack<AAVal> stack) {

	if (stack.Size() == 1) {
		AAVal v = stack.Pop();
		if (m_outStream && m_logTopOfStackAfterExec) {
			AA_Literal litVal = v.litVal;
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
		return v;
	} else {
		return AAVal::Null;
	}

}

void AAVM::WriteCompilerError(AAC_CompileErrorMessage errMsg) {

	// Make sure we have an output stream we can write to
	if (m_outStream) {

		// Do we have a valid message?
		if (errMsg.errorMsg) {

			// Format the compile error message
			std::string compileErrMsg = "Failed to compile, [C" + std::to_string(errMsg.errorType) + "] -> '" + std::string(errMsg.errorMsg)
				+ "' on line " + std::to_string(errMsg.errorSource.line) + ", column " + std::to_string(errMsg.errorSource.column) + "\n";

			// Write error message
			m_outStream->write(compileErrMsg.c_str(), compileErrMsg.length());

		} else {

			// something much more awful

		}

	}

}
