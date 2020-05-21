#include "AAVM.h"
#include "AAB2F.h"
#include "astring.h"
#include "AAString.h"
#include "AAConsole.h"
#include "AAstdiolib.h"
#include "AAMemoryStore.h"
#include "AAExecute.h"

AAVM* AAVM::CreateNewVM(bool logExecuteTime, bool logCompiler, bool logTopStack) {
	AAVM* vm = new AAVM();
	vm->m_logCompileMessages = logCompiler;
	vm->m_logExecTime = logExecuteTime;
	vm->m_logTopOfStackAfterExec = logTopStack;
	vm->LoadStandardLibrary();
	return vm;
}

AAVM::AAVM() {

	m_compiler = new AAC(this);

	if (m_compiler) {
		m_compiler->SetupCompiler();
	}

	m_parser = new AAP;
	m_outStream = 0;

	m_startCompile = 0;

	m_heapMemory = 0;
	m_staticTypeEnvironment = 0;

	m_logTopOfStackAfterExec = false;
	m_logCompileMessages = false;
	m_logExecTime = false;
	m_logTrace = false;

	// Nothing has been run yet, thus we couldn't have a runtime error
	m_hasRuntimeError = false;

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

AAStackValue AAVM::CompileAndRunExpression(std::wstring input) {
	return this->CompileAndRunExpression(input, L"", L"", L"");
}

AAStackValue AAVM::CompileAndRunExpression(std::wstring input, std::wstring binaryoutputfile) {
	return this->CompileAndRunExpression(input, binaryoutputfile, L"", L"");
}

AAStackValue AAVM::CompileAndRunExpression(std::wstring input, std::wstring binaryoutputfile, std::wstring formattedoutputfile) {
	return this->CompileAndRunExpression(input, binaryoutputfile, formattedoutputfile, L"");
}

AAStackValue AAVM::CompileAndRunExpression(std::wstring input, std::wstring binaryoutputfile, std::wstring formattedoutputfile, std::wstring unparsefile) {

	// Start compile clock
	m_startCompile = clock();

	// Parse the input
	AAP_ParseResult result = m_parser->Parse(input);

	// Return full result of compile and run
	return this->CompileAndRun(result, binaryoutputfile, formattedoutputfile, unparsefile);

}

AAStackValue AAVM::CompileAndRunFile(std::wstring sourcefile, std::wstring binaryoutputfile) {
	return this->CompileAndRunFile(sourcefile, binaryoutputfile, L"", L"");
}

AAStackValue AAVM::CompileAndRunFile(std::wstring sourcefile, std::wstring binaryoutputfile, std::wstring formattedoutputfile) {
	return this->CompileAndRunFile(sourcefile, binaryoutputfile, formattedoutputfile, L"");
}

AAStackValue AAVM::CompileAndRunFile(std::wstring sourcefile, std::wstring binaryoutputfile, std::wstring formattedoutputfile, std::wstring unparsefile) {
	
	// Start compile clock
	m_startCompile = clock();

	// Parse the input
	AAP_ParseResult result = m_parser->Parse(std::wifstream(sourcefile));

	// Return full result of compile and run
	return this->CompileAndRun(result, binaryoutputfile, formattedoutputfile, unparsefile);

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

AAStackValue AAVM::CompileAndRunFile(std::wstring sourcefile) {
	return this->CompileAndRunFile(sourcefile, L"", L"");
}

AAStackValue AAVM::CompileAndRun(AAP_ParseResult result, std::wstring binaryoutputfile, std::wstring formattedoutputfile, std::wstring unparsefile) {

	// The the parser succeed?
	if (result.success) {

		// Set compiler output files
		m_compiler->SetOpListFile(formattedoutputfile);
		m_compiler->SetUnparseFile(unparsefile);

		// Compile all procedures into bytecode
		AAC_CompileResult compileResult = m_compiler->CompileFromAbstractSyntaxTrees(result.result);

		// Log compile time and other messages
		this->StopAndLogCompile();

		// Did we compile without error?
		if (COMPILE_SUCESS(compileResult)) {

			// Only dump bytecode if a filepath is specified
			if (binaryoutputfile != L"") {

				// Dump bytecode
				aa::dump_bytecode(binaryoutputfile, compileResult.result);

			}

			// Cleanup trees
			m_parser->ClearTrees(result.result);

			// Update last compile result
			m_lastCompileResult = compileResult;

			// Execute the bytecode
			return this->Execute(compileResult.result);

		} else {

			// Write compiler error
			this->WriteCompilerError(compileResult.firstMsg);

			// Return nothing
			return AAStackValue::None;

		}

	} else {

		// Write syntax error
		this->WriteSyntaxError(result.firstMsg);

		// Clear the errors
		m_parser->ClearError();

		// Return nothing
		return AAStackValue::None;

	}

}

AAStackValue AAVM::Execute(AAC_Out bytecode) {
	return this->Execute(bytecode.bytes, bytecode.length);
}

AAStackValue AAVM::Execute(unsigned char* bytes, unsigned long long len) {

	// Load program so we can execute it
	AAProgram* pProg = new AAProgram;
	if (pProg->LoadProgram(bytes, len)) {

		// Run the program
		return this->Run(pProg);

	} else {

		// Return AAVal Error
		return AAStackValue::None;

	}

}

AAStackValue AAVM::Run(AAProgram* pProg) {

	// Clear last runtime flag
	m_hasRuntimeError = false;

	// Get entry point
	int entryPoint = pProg->GetEntryPoint();

	// Log start time
	clock_t s = clock();

	// Run the program from entry point
	AAStackValue v = this->Run(pProg->m_procedures, pProg->GetTypeEnvironment(), entryPoint);

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

#define AAVM_ThrowRuntimeErr(exc, msg) this->WriteRuntimeError(AAVM_RuntimeError(exc, (msg).c_str(), execp, callstack)); return /*AAStackValue::None*/;

AAStackValue AAVM::Run(AAProgram::Procedure* procedure, AAStaticTypeEnvironment* staticProgramTypeEnvironment, int entry) {

	any_stack stack = any_stack(1024); // Stack with 1 Kb storage --> Will have to be modular
	aa::stack<AARuntimeEnvironment> callstack;

	// Set static type environment
	m_staticTypeEnvironment = staticProgramTypeEnvironment;

	// Represents the VM's heap memory (We allocate this on the heap as well). We do this every time a program is executed
	m_heapMemory = new AAMemoryStore(256);
	m_heapMemory->SetStaticTypeEnvironment(m_staticTypeEnvironment);

	// Create reuntime environment from entry point
	AARuntimeEnvironment execp;
	execp.opPointer = 0;
	execp.procPointer = entry;
	execp.venv = procedure[entry].venv->CloneSelf();

	// Execute code
	this->exec(procedure, callstack, stack, execp);

	// Report the stack
	AAStackValue val = this->ReportStack(stack);

	// Release all allocated memory (No longer used)
	m_heapMemory->Release();

	// Return whatever's on top of the stack
	return val;

}

void AAVM::exec(AAProgram::Procedure* procedure, aa::stack<AARuntimeEnvironment>& callstack, any_stack& stack, AARuntimeEnvironment& execp) {

	while (AAVM_OPI < procedure[AAVM_PROC].opCount) {
		switch (AAVM_CURRENTOP) {
		case AAByteCode::ADD:
		case AAByteCode::DIV:
		case AAByteCode::MUL:
		case AAByteCode::SUB:
		{
			AAPrimitiveType pt = (AAPrimitiveType)AAVM_GetArgument(0);
			if (pt == AAPrimitiveType::int16) {
				stack.Push<int16_t>(aa::vm::Arithmetic_BinaryOperation(AAVM_CURRENTOP, stack.Pop<int16_t>(), stack.Pop<int16_t>()));
			} else if (pt == AAPrimitiveType::int32) {
				stack.Push<int32_t>(aa::vm::Arithmetic_BinaryOperation(AAVM_CURRENTOP, stack.Pop<int32_t>(), stack.Pop<int32_t>()));
			} else if (pt == AAPrimitiveType::int64) {
				stack.Push<int64_t>(aa::vm::Arithmetic_BinaryOperation(AAVM_CURRENTOP, stack.Pop<int64_t>(), stack.Pop<int64_t>()));
			} else if (pt == AAPrimitiveType::real32) {
				stack.Push<float_t>(aa::vm::Arithmetic_BinaryOperation(AAVM_CURRENTOP, stack.Pop<float_t>(), stack.Pop<float_t>()));
			} else if (pt == AAPrimitiveType::real64) {
				stack.Push<double_t>(aa::vm::Arithmetic_BinaryOperation(AAVM_CURRENTOP, stack.Pop<double_t>(), stack.Pop<double_t>()));
			}
			AAVM_OPI++;
			break;
		}
		case AAByteCode::MOD: { // can't compile if we let double and floats be executed with this
			AAPrimitiveType pt = (AAPrimitiveType)AAVM_GetArgument(0);
			if (pt == AAPrimitiveType::int16) {
				stack.Push<int16_t>(aa::vm::Mod_BinaryOperation(stack.Pop<int16_t>(), stack.Pop<int16_t>()));
			} else if (pt == AAPrimitiveType::int32) {
				stack.Push<int32_t>(aa::vm::Mod_BinaryOperation(stack.Pop<int32_t>(), stack.Pop<int32_t>()));
			} else if (pt == AAPrimitiveType::int64) {
				stack.Push<int64_t>(aa::vm::Mod_BinaryOperation(stack.Pop<int64_t>(), stack.Pop<int64_t>()));
			}
			AAVM_OPI++;
			break;
		}
		case AAByteCode::CONCAT: {
			std::wstring rhs = m_heapMemory->String(stack.Pop<AAMemoryPtr>())->ToString();
			std::wstring lhs = m_heapMemory->String(stack.Pop<AAMemoryPtr>())->ToString();
			stack.Push(m_heapMemory->AllocString(lhs + rhs));
			AAVM_OPI++;
			break;
		}
		case AAByteCode::CMPE:
		case AAByteCode::CMPNE:
		case AAByteCode::GE:
		case AAByteCode::GEQ:
		case AAByteCode::LE:
		case AAByteCode::LEQ: {
			AAPrimitiveType pt = (AAPrimitiveType)AAVM_GetArgument(0);
			if (pt == AAPrimitiveType::int16) {
				stack.Push<bool>(aa::vm::Logic_BinaryOperation(AAVM_CURRENTOP, stack.Pop<int16_t>(), stack.Pop<int16_t>()));
			} else if (pt == AAPrimitiveType::int32) {
				stack.Push<bool>(aa::vm::Logic_BinaryOperation(AAVM_CURRENTOP, stack.Pop<int32_t>(), stack.Pop<int32_t>()));
			} else if (pt == AAPrimitiveType::int64) {
				stack.Push<bool>(aa::vm::Logic_BinaryOperation(AAVM_CURRENTOP, stack.Pop<int64_t>(), stack.Pop<int64_t>()));
			} else if (pt == AAPrimitiveType::real32) {
				stack.Push<bool>(aa::vm::Logic_BinaryOperation(AAVM_CURRENTOP, stack.Pop<float_t>(), stack.Pop<float_t>()));
			} else if (pt == AAPrimitiveType::real64) {
				stack.Push<bool>(aa::vm::Logic_BinaryOperation(AAVM_CURRENTOP, stack.Pop<double_t>(), stack.Pop<double_t>()));
			}
			AAVM_OPI++;
			break;
		}
		case AAByteCode::NNEG: {
			AAPrimitiveType pt = (AAPrimitiveType)AAVM_GetArgument(0);
			if (pt == AAPrimitiveType::int16) {
				stack.Push<int16_t>(aa::vm::Arithmetic_UnaryOperation(AAVM_CURRENTOP, stack.Pop<int16_t>()));
			} else if (pt == AAPrimitiveType::int32) {
				stack.Push<int32_t>(aa::vm::Arithmetic_UnaryOperation(AAVM_CURRENTOP, stack.Pop<int32_t>()));
			} else if (pt == AAPrimitiveType::int64) {
				stack.Push<int64_t>(aa::vm::Arithmetic_UnaryOperation(AAVM_CURRENTOP, stack.Pop<int64_t>()));
			} else if (pt == AAPrimitiveType::real32) {
				stack.Push<float_t>(aa::vm::Arithmetic_UnaryOperation(AAVM_CURRENTOP, stack.Pop<float_t>()));
			} else if (pt == AAPrimitiveType::real64) {
				stack.Push<double_t>(aa::vm::Arithmetic_UnaryOperation(AAVM_CURRENTOP, stack.Pop<double_t>()));
			}
			AAVM_OPI++;
			break;
		}
		case AAByteCode::LNEG: {
			stack.Push(!stack.Pop<bool>()); // Logical negation => Can only be done on bools
			AAVM_OPI++;
			break;
		}
		case AAByteCode::PUSHC: {
			aa::vm::PushConstant(procedure[AAVM_PROC].constTable[AAVM_GetArgument(0)], stack);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::PUSHN: {
			aa::vm::PushSomething(AAStackValue::Null, stack);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::PUSHWS: {
			AA_Literal sLit = procedure[AAVM_PROC].constTable[AAVM_GetArgument(0)];
			std::wstring wcs = sLit.lit.s.val;
			aa::vm::PushSomething(AAStackValue(m_heapMemory->AllocString(wcs)), stack);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::GETVAR: {
			AAStackValue val = AAVM_VENV->GetVariable(AAVM_GetArgument(0));
			aa::vm::PushSomething(val, stack);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::SETVAR: {
			AAVM_VENV->SetVariable(AAVM_GetArgument(0), aa::vm::PopSomething((AAPrimitiveType)AAVM_GetArgument(1), stack));
			AAVM_OPI++;
			break;
		}
		case AAByteCode::CALL: {

			int callProc = AAVM_GetArgument(0);
			int argCount = AAVM_GetArgument(1);

			if (callProc == -1) {
				AAVM_ThrowRuntimeErr("FatalCompileError", "Call index " + std::to_string(callProc) + " is out of range!");
			}

			AAVM_OPI++;
			callstack.Push(execp);

			AAVM_VENV = procedure[callProc].venv->CloneSelf();
			AAVM_PROC = callProc;
			AAVM_OPI = 0;

			break;
		}
		case AAByteCode::VCALL: {

			// Get object from stack
			AAMemoryPtr ptr = aa::vm::PopSomething(AAPrimitiveType::refptr, stack).to_cpp<AAMemoryPtr>();
			AAObject* pCaller = ptr.get_object();
			
			// Put back on stack
			stack.Push(ptr);

			// Fetch actual procedure to call
			int callProc = pCaller->GetVirtualFunctionPtr(AAVM_GetArgument(0));

			// Make sure it's a valid procedure
			if (callProc == -1) {
				AAVM_ThrowRuntimeErr("FatalCompileError", "Call index " + std::to_string(callProc) + " is out of range!");
			}

			AAVM_OPI++;
			callstack.Push(execp);

			AAVM_VENV = procedure[callProc].venv->CloneSelf();
			AAVM_PROC = callProc;
			AAVM_OPI = 0;

			break;
		}
		case AAByteCode::XCALL: {

			// Get function
			int callProc = AAVM_GetArgument(0);
			int argCount = AAVM_GetArgument(1);

			Trace("AAVM_OPI: %i\n", AAVM_OPI);
			Trace(".xcall before %i\n", (int)stack.get_pointer());

			// Call the native function (The native function is also responsible for handling the return
			this->m_cppfunctions.Apply(callProc).fPtr(this, stack);

			Trace(".xcall before %i\n", (int)stack.get_pointer());

			// Did the call cause a runtime error?
			if (this->m_hasRuntimeError) {

				// Update runtime environment
				m_lastRuntimeError.errEnv = execp;

				// Update callstack
				m_lastRuntimeError.callStack = callstack;

				// Write error
				this->WriteRuntimeError(m_lastRuntimeError);

				// Stop execution
				return;

			}

			// Goto next cmd
			AAVM_OPI++;

			break;
		}
		case AAByteCode::RET: {

			if (callstack.Size() > 0) {
				execp.PopEnvironment(false);
				execp = callstack.Pop();
			} else {
				AAVM_ThrowRuntimeErr("CallstackCorrupted", std::string("The callstack was unexpectedly corrupted"));
			}

			break;
		}
		case AAByteCode::JMPF: {
			if (stack.Pop<bool>()) {
				AAVM_OPI++;
			} else {
				AAVM_OPI += 1 + AAVM_GetArgument(0); // jump to next (if-else or else) or next statement after block
			}
			break;
		}
		case AAByteCode::JMPT: {
			if (!stack.Pop<bool>()) {
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
			stack.Push(m_heapMemory->Alloc(allocsz));
			AAVM_OPI++;
			break;
		}
		case AAByteCode::CTOR: { // TODO: Make call functionality better so we have less duplicate code

			// Collect arguments
			uint32_t typeID = (uint32_t)AAVM_GetArgument(0);
			int callProc = AAVM_GetArgument(1);
			bool isVM = AAVM_GetArgument(2);
			size_t allocsize = 0;//AAVM_GetArgument(3);

			// Lookup the type we're creating
			AAObjectType* pObjectType = this->m_staticTypeEnvironment->LookupType(typeID);
			allocsize = pObjectType->GetSize();

			// Allocate the memory
			AAMemoryPtr ptr = m_heapMemory->Alloc(allocsize);

			// Setup object
			AAObject* obj = ptr.get_object();
			obj->SetType(pObjectType);
			obj->CreateInheritance(m_staticTypeEnvironment);

			// Push as last argument
			stack.Push(ptr);

			if (isVM) {

				Trace(".ctor before %i\n", (int)stack.get_pointer());

				// Call and forget
				this->m_cppfunctions.Apply(callProc).fPtr(this, stack);

				Trace(".ctor after %i\n", (int)stack.get_pointer());

				// Did the call cause a runtime error?
				if (this->m_hasRuntimeError) {

					// Update runtime environment
					m_lastRuntimeError.errEnv = execp;

					// Update callstack
					m_lastRuntimeError.callStack = callstack;

					// Write error
					this->WriteRuntimeError(m_lastRuntimeError);

					// Stop execution
					return /*AAStackValue::None*/;

				}

				AAVM_OPI++;

			} else {

				if (callProc == -1) {
					AAVM_ThrowRuntimeErr("FatalCompileError", "Call index " + std::to_string(callProc) + " is out of range!");
				}

				AAVM_OPI++; // Goto next step in current execution context
				callstack.Push(execp);

				AAVM_VENV = procedure[callProc].venv->CloneSelf();
				AAVM_PROC = callProc;
				AAVM_OPI = 0;

			}

			break;
		}
		case AAByteCode::ALLOCARRAY: {
			AAPrimitiveType t = (AAPrimitiveType)AAVM_GetArgument(0);
			int dimCount = AAVM_GetArgument(1);
			uint32_t* dimLengths = new uint32_t[dimCount];
			for (int i = 0; i < dimCount; i++) {
				dimLengths[0] = stack.Pop<uint32_t>(); // NOTE!: We'll always pop 4 bytes from stack
			}
			AAMemoryPtr ptr = m_heapMemory->AllocArray(t, dimCount, dimLengths);
			stack.Push(ptr);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::GETFIELD: {
			AAMemoryPtr oPtr = stack.Pop< AAMemoryPtr>();
			AAObject* o = m_heapMemory->Object(oPtr);
			if (o) {
				aa::vm::PushSomething(aa::GetValue(o, (AAPrimitiveType)AAVM_GetArgument(1), AAVM_GetArgument(0)), stack);
			} else {
				AAVM_ThrowRuntimeErr("NullPointerException", "Null value at address '" + oPtr.get_memory_address_str() + "'");
			}
			AAVM_OPI++;
			break;
		}
		case AAByteCode::SETFIELD: {
			AAStackValue rhs = aa::vm::PopSomething((AAPrimitiveType)AAVM_GetArgument(1), stack);
			AAMemoryPtr oPtr = stack.Pop< AAMemoryPtr>();
			AAObject* o = m_heapMemory->Object(oPtr);
			if (o) {
				aa::SetValue(o, AAVM_GetArgument(0), rhs);
			} else {
				AAVM_ThrowRuntimeErr("NullPointerException", "Null value at address '" + oPtr.get_memory_address_str() + "'");
			}
			AAVM_OPI++;
			break;
		}
		case AAByteCode::GETELEM: {
			int32_t i = stack.Pop<int32_t>(); // index
			AAMemoryPtr arrayPtr = stack.Pop< AAMemoryPtr>(); // Ptr to array
			AAArray* arrObj = m_heapMemory->Array(arrayPtr); // Actual array object
			if (arrObj) {
				if (i >= 0 && i < (int32_t)arrObj->get_length()) {
					aa::vm::PushSomething(arrObj->get_value(i, AAVM_GetArgument(0)), stack);
				} else {
					AAVM_ThrowRuntimeErr("IndexOutOfRange", "Index " + std::to_string(i) + " is out of range!");
				}
			} else {
				AAVM_ThrowRuntimeErr("NullPointerException", "Null value at address '" + arrayPtr.get_memory_address_str() + "'");
			}
			AAVM_OPI++;
			break;
		}
		case AAByteCode::SETELEM: {
			AAMemoryPtr arrayPtr = stack.Pop<AAMemoryPtr>(); // Ptr to array
			AAArray* arrObj = m_heapMemory->Array(arrayPtr); // Actual array object
			if (arrObj) {
				int32_t i = stack.Pop<int32_t>(); // index
				if (i >= 0 && i < (int32_t)arrObj->get_length()) {
					arrObj->set_value(i, AAVM_GetArgument(0), aa::vm::PopSomething(arrObj->get_type(), stack));
				} else {
					AAVM_ThrowRuntimeErr("IndexOutOfRange", "Index " + std::to_string(i) + " is out of range!");
				}
			} else {
				AAVM_ThrowRuntimeErr("NullPointerException", "Null value at address '" + arrayPtr.get_memory_address_str() + "'");
			}
			AAVM_OPI++;
			break;
		}
		case AAByteCode::BDOP: {
			aa::list<AAStackValue> v = this->BreakdownObject(stack.Pop<AAMemoryPtr>());
			v.ForEach([&stack](AAStackValue& val) { stack.Push(val); });
			AAVM_OPI++;
			break;
		}
		case AAByteCode::BCKM:
			stack.Push(this->BackwardsPatternmatch(AAVM_GetArgument(0), AAVM_GetArgument(1), AAVM_GetArgument(2), AAVM_GetArgument(3), stack));
			AAVM_OPI++;
			break;
		case AAByteCode::WRAP: {
			//Trace("Before %i\n", (int)stack.get_pointer());
			AAStackValue top = aa::vm::PopSomething((AAPrimitiveType)AAVM_GetArgument(0), stack);
			//Trace("Pop %i\n", (int)stack.get_pointer());
			stack.Push(top); // Push the popped stackvalue directly back unto the stack
			//Trace("Push %i\n", (int)stack.get_pointer());
			AAVM_OPI++;
			break;
		}
		case AAByteCode::UNWRAP:
			aa::vm::PushSomething(stack.Pop<AAStackValue>(), stack); // Pop as stack value and push unto stack
			AAVM_OPI++;
			break;
		case AAByteCode::POP:
			stack.Pop(AAVM_GetArgument(0));
			AAVM_OPI++;
			break;
		case AAByteCode::TUPLENEW: {
			int count = AAVM_GetArgument(0);
			aa::array<AAPrimitiveType> types = aa::array<AAPrimitiveType>(count);
			aa::array<AAVal> values = aa::array<AAVal>(count);
			for (int i = 0; i < count; i++) {
				types[count - i - 1] = (AAPrimitiveType)AAVM_GetArgument(i + 1);
				values[count - i - 1] = aa::vm::PopSomething(types[count - i - 1], stack).as_val();
			}
			aa::vm::PushSomething(AAStackValue(AATuple(types, values)), stack);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::TUPLEGET: {
			AATuple tuple = aa::vm::PopSomething(AAPrimitiveType::tuple, stack).to_cpp<AATuple>();
			int index = AAVM_GetArgument(0);
			AAPrimitiveType pushType = tuple.TypeAt(index);
			AAVal pushVal = tuple.ValueAt(index);
			aa::vm::PushSomething(pushType, pushVal, stack);
			AAVM_OPI++;
			break;
		}
		case AAByteCode::NOP:
			AAVM_OPI++;
			break;
		default:
			AAVM_ThrowRuntimeErr("UnknownBytecodeError", "Used unknown bytecode operation @" + std::to_string(AAVM_OPI));
			AAVM_OPI++;
			break;
		}

	}

}

aa::list<AAStackValue> AAVM::BreakdownObject(AAStackValue top) {

	aa::list<AAStackValue> ls;
	AAMemoryPtr ptr = top.to_cpp<AAMemoryPtr>();

	if (ptr > 0) {

		AAObject* pObject = m_heapMemory->Object(ptr);

		printf("");

	} else {

		printf("Something very fatal!");

	}

	return ls;

}

AAStackValue AAVM::BackwardsPatternmatch(int op, int vm, int args, int get, const any_stack& stack) {

	AAStackValue v = false;

	return v;

}

template<typename T>
inline T __writesck(any_stack stack, std::ostream* pOutstream, bool logTopOfStack) {
	T t = stack.Pop<T>();
	if (pOutstream && logTopOfStack) {
		std::wstring ws = std::to_wstring(t);
		pOutstream->write(string_cast(ws).c_str(), ws.length());
		pOutstream->write("", 1);
		pOutstream->write("\n", 1);
	}
	return t;
}

AAStackValue AAVM::ReportStack(any_stack& stack) {

	if (stack.is_empty()) {
		stack.Release(false);
		return AAStackValue::None;
	} else {
		AAStackValue val = AAStackValue::None;
		if (stack.get_pointer() == 8) {
			val = AAStackValue(__writesck<int64_t>(stack, m_outStream, m_logTopOfStackAfterExec));
		} else if (stack.get_pointer() == 4) {
			val = AAStackValue(__writesck<int32_t>(stack, m_outStream, m_logTopOfStackAfterExec));
		} else if (stack.get_pointer() == 2) {
			val = AAStackValue(__writesck<int16_t>(stack, m_outStream, m_logTopOfStackAfterExec));
		} else if (stack.get_pointer() == 1) {
			val = AAStackValue(__writesck<unsigned char>(stack, m_outStream, m_logTopOfStackAfterExec));
		} else if (stack.get_pointer() == sizeof(AAMemoryPtr)) {
			AAMemoryPtr ptr = stack.Pop<AAMemoryPtr>();
			if (ptr.ptrType == 'S') {
				std::wstring str = m_heapMemory->String(ptr)->ToString();
				if (m_logTopOfStackAfterExec) {
					m_outStream->write(string_cast(str).c_str(), str.length());
					m_outStream->write("", 1);
					m_outStream->write("\n", 1);
				}
				val = AAStackValue(str);
			} else {
				std::wstring str = ptr.get_object()->ToString();
				if (m_logTopOfStackAfterExec) {
					m_outStream->write(string_cast(str).c_str(), str.length());
					m_outStream->write("", 1);
					m_outStream->write("\n", 1);
				}
				val = AAStackValue(ptr);
			}
		} else if (stack.get_pointer() != 0) {
			std::string msg = "<Warning!> " + std::to_string(stack.get_pointer()) + " bytes remained on the stack!\n";
			m_outStream->write(msg.c_str(), msg.length());
		}
		stack.Release(false);
		return val;
	}

}

void AAVM::WriteCompilerError(AAC_CompileErrorMessage errMsg) {

	// Do we have a valid message?
	if (errMsg.errorMsg) {

		// Format the compile error message
		std::string compileErrMsg = "Failed to compile, [C" + std::to_string(errMsg.errorType) + "] -> '" + std::string(errMsg.errorMsg)
			+ "' on line " + std::to_string(errMsg.errorSource.line) + ", column " + std::to_string(errMsg.errorSource.column) + "\n";

		// Write error message
		this->WriteMsg(compileErrMsg.c_str());

	}

	// Update result
	this->m_lastCompileResult.success = false;
	this->m_lastCompileResult.firstMsg = errMsg;

}

void AAVM::WriteSyntaxError(AAP_SyntaxErrorMessage errMsg) {

	// Do we have a valid message?
	if (errMsg.errorMsg) {

		// String stream
		std::stringstream ss;

		// Format messages
		ss << "Syntax error [S" << errMsg.errorType << "] on line " << errMsg.errorSource.line << ", column " << errMsg.errorSource.column << ": ";
		ss << errMsg.errorMsg;
		ss << "\n";

		// Write syntax message
		this->WriteMsg(ss.str().c_str());

	}

}

void AAVM::WriteRuntimeError(AAVM_RuntimeError err) {

	// Make sure there's a valid error message
	if (err.errMsg) {

		// Format runtime message
		std::string runtimeErrMsg = "Runtime exception, [" + std::string(err.errName) + "] -> '" + std::string(err.errMsg) + "'\n";

		// Write message
		this->WriteMsg(runtimeErrMsg.c_str());

	}

	// Update runtime error flag
	m_hasRuntimeError = true;

	// Set last runtime error
	m_lastRuntimeError = err;

}

void AAVM::ThrowExternalError(AAVM_RuntimeError externalErr) {

	// Set the runtime error flag
	this->m_hasRuntimeError = true;

	// Update last runtime error
	this->m_lastRuntimeError = externalErr;

}

void AAVM::WriteMsg(const char* msg) {

	// Make sure there's a valid stream to write to
	if (m_outStream) {

		// Write error message
		m_outStream->write(msg, strlen(msg));

	}

}

void AAO_ToString(AAVM* pAAVm, any_stack& stack);
void AAO_NewObject(AAVM* pAAVm, any_stack& stack);

int AAVM::RegisterFunction(AACSingleFunction funcPtr) {
	return this->RegisterFunction(funcPtr, NULL);
}

int AAVM::RegisterFunction(AACSingleFunction funcPtr, AACNamespace* domain) {
	AAFuncSignature* dcSig;
	return this->RegisterFunction(funcPtr, dcSig, domain);
}

int AAVM::RegisterFunction(AACSingleFunction funcPtr, AAFuncSignature*& funcSig, AACNamespace* domain, bool isClassMethod) {

	// Create the new function signature
	funcSig = new AAFuncSignature;

	// Is it a new function?
	bool isNewFunction = false;

	int procID = 0;
	if (!m_cppfunctions.FindFirstIndex([funcPtr](AACSingleFunction& func) { return func.equals(funcPtr); }, procID)) {

		// Fetch next proc ID
		procID = (int)m_cppfunctions.Size();
		isNewFunction = true;

	}

	// Create function signature
	funcSig->name = funcPtr.name;
	funcSig->returnType = funcPtr.returnType;
	funcSig->parameters = funcPtr.params;
	funcSig->isVMFunc = true;
	funcSig->isClassMethod = isClassMethod;
	funcSig->accessModifier = AAAccessModifier::PUBLIC;
	funcSig->procID = procID;

	// Add only if new function
	if (isNewFunction) {

		// Push functions
		m_cppfunctions.Add(funcPtr);

	}

	// Do we have a domain to add this function to?
	if (domain) {

		// Add to domain
		domain->AddFunction(funcSig);

	} else {

		// Add VM function so the compiler can recognize it
		m_compiler->AddVMFunction(funcSig);

	}

	// Return the proc ID
	return procID;

}

AAClassSignature* AAVM::RegisterClass(std::wstring typeName, AACClass cClass) {

	// Is string defined?
	if (AACTypeDef::String != 0) {

		bool any = true;

		for (auto& method : cClass.classMethods) {
			if (method.name == L"ToString" && method.params.size() == 0) {
				any = false;
			}
		}

		if (any) {
			cClass.classMethods.push_back(AACSingleFunction(L"ToString", &AAO_ToString, AACTypeDef::String, 0));
		}
		
	}

	// Class signature
	AAClassSignature* cc = new AAClassSignature(typeName);

	// Mark type as VM type
	cc->type->isVMType = true;

	// For each class method
	for (auto& func : cClass.classMethods) {

		// Check if it's a constructor
		bool isCtor = func.name == L".ctor";
		bool isDtor = func.name == L".dtor";

		// Not the constructor
		if (!isCtor && !isDtor) {

			// Update function name
			func.name = cc->name + L"::" + func.name;

		} else {

			// Update constructor name
			func.name = cc->name + L"::" + cc->name;

		}

		// Function signature
		AAFuncSignature* sig;

		// Add 'this' into the argument list
		if (isCtor) {

			// Because it's a class constrcutor we push it into the back
			func.params.push_back(AAFuncParam(cc->type, L"this"));

		} else {

			// Because it's a class method we always push the 'this' identifier -> Note, should not be the case if static (but not implemented yet)
			func.params.insert(func.params.begin(), AAFuncParam(cc->type, L"this"));

		}

		// Register the funcion and get the VMCall procID
		this->RegisterFunction(func, sig, cClass.domain, true);
		sig->isClassCtor = isCtor;
		sig->isClassDtor = isDtor;

		// Add method to class
		cc->methods.Add(sig);

	}

	// For each operator overload
	for (auto& op : cClass.classOperators) {

		// Get the actual function
		AACSingleFunction func = op.funcPtr;

		// Update function name
		func.name = cc->name + L"::" + func.name;

		// Function signature
		AAFuncSignature* sig;

		// Because it's a class method we always push the 'this' identifier -> Note, should not be the case if static (but not implemented yet)
		func.params.insert(func.params.begin(), AAFuncParam(cc->type, L"this"));

		// Register the funcion and get the VMCall procID
		int procID = this->RegisterFunction(func, sig, cClass.domain, true);

		// Add method to operators list
		cc->operators.Add(AAClassOperatorSignature(op.operatorToOverride, sig));

	}

	// For each class field
	for (auto& field : cClass.classFields) {

		AAClassFieldSignature ccf;
		ccf.fieldID = (int)cc->fields.Size();
		ccf.accessModifier = AAAccessModifier::PUBLIC;
		ccf.type = field.fieldtype;
		ccf.name = field.fieldname;

		cc->fields.Add(ccf);

	}

	// Update class size
	cc->classByteSz = m_compiler->GetClassCompilerInstance()->CalculateMemoryUse(cc);

	// Fix all references to ourselves
	this->FixSelfReferences(cc);

	// Do we have domain to work with?
	if (cClass.domain) {
		
		// Add class to domain
		cClass.domain->AddClass(cc);

	} else {

		// Add the VM class
		m_compiler->AddVMClass(cc);

	}

	// Return the class signature
	return cc;

}

void AAVM::LoadStandardLibrary() {

	// Register println
	this->RegisterFunction(AACSingleFunction(L"println", &AAConsole_PrintLn, AACType::Void, 1, AAFuncParam(AACType::Any, L"obj")));

	// Create string class
	AACClass stringClass;
	stringClass.classMethods.push_back(AACSingleFunction(L"length", &AAString_Length, AACTypeDef::Int32, 0));

	// Register string class and type
	AAClassSignature* strCls = this->RegisterClass(L"string", stringClass);
	AACTypeDef::String = strCls->type;

	// Create object class
	AACClass objectClass;
	objectClass.classMethods.push_back(AACSingleFunction(L".ctor", &AAO_NewObject, AACType::Void, 0));
	this->RegisterClass(L"object", objectClass);

	// Create standard namespaces
	AACNamespace* __std = new AACNamespace(L"std", NULL);
	AACNamespace* __stdio = new AACNamespace(L"io", __std);

	// Create filestream class
	AACClass stdio_filestream;
	stdio_filestream.domain = __stdio;
	stdio_filestream.classMethods.push_back(AACSingleFunction(L".ctor", &AAFileStream_Open, AACType::ExportReferenceType, 1, AAFuncParam(AACTypeDef::String, L"_filepath")));
	stdio_filestream.classMethods.push_back(AACSingleFunction(L"close", &AAFileStream_Close, AACType::Void, 0));
	stdio_filestream.classOperators.push_back(AACClassOperator(L"<<", AACSingleFunction(L"writetofile", &AAFileStream_Write, AACType::ExportReferenceType, 1, AAFuncParam(AACType::Any, L"_content"))));
	stdio_filestream.classFields.push_back(AACClassField(AACTypeDef::IntPtr, L"_selfptr"));

	// Register the filestream class
	this->RegisterClass(L"FileStream", stdio_filestream);

	// Add more classes here

	// Register std::io in std
	__std->childspaces.Add(__stdio);

	// Add namespace
	this->m_compiler->AddVMNamespace(__std);

}

void AAVM::FixSelfReferences(AAClassSignature* signature) {

	// Fix all method references
	signature->methods.ForEach(
		[signature]( AAFuncSignature*& sig ) {
			if (sig->returnType == AACType::ExportReferenceType) {
				sig->returnType = signature->type;
			}
			for (auto& param : sig->parameters) {
				if (param.type == AACType::ExportReferenceType) {
					param.type = signature->type;
				}
			}
		}
	);

	// Fix all field references
	signature->fields.ForEach(
		[signature](AAClassFieldSignature& field) {
			if (field.type == AACType::ExportReferenceType) {
				field.type = signature->type;
			}
		}
	);

	// Fix all operator references
	signature->operators.ForEach(
		[signature](AAClassOperatorSignature& op) {
			if (op.method->returnType == AACType::ExportReferenceType) {
				op.method->returnType = signature->type;
			}
			for (auto& param : op.method->parameters) {
				if (param.type == AACType::ExportReferenceType) {
					param.type = signature->type;
				}
			}
		}
	);

}

void AAVM::StopAndLogCompile() {

	// If we should log stuff, we do it now
	if (m_logCompileMessages) {
		printf("Compile time: %fs\n", (float)(clock() - m_startCompile) / CLOCKS_PER_SEC);
	}

}

void AAVM::Trace(const char* msg, ...) {
#if _DEBUG
	if (m_logTrace) {
		va_list arglist;
		va_start(arglist, msg);
		vprintf(msg, arglist);
		va_end(arglist);
	}
#endif
}

void AAO_ToString(AAVM* pAAVm, any_stack& stack) {
	
	// Get the address and create new string for it
	AAMemoryPtr hexMemAddress = pAAVm->GetHeap()->AllocString(stack.Pop<AAMemoryPtr>().get_memory_address_wstr());

	// Push address
	aa::vm::PushSomething(AAStackValue(hexMemAddress), stack);

}

void AAO_NewObject(AAVM* pAAVm, any_stack& stack) {
	/*
	// Convert to argument list
	aa::list<AAStackValue> argl = aa::ToArgumentList(args);

	// Push string representation of primitive unto stack
	stack.Push(argl.First());
	*/ // We can do nothing here

	// Pop object pointer from stack
	stack.Pop(sizeof(AAMemoryPtr));

}
