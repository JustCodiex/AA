#include "AAC.h"
#include "AAB2F.h"
#include "AAVal.h"
#include <stack>

void AAC::SetupCompiler() {

	// Create instance of class compiler
	m_classCompiler = new AAClassCompiler;

	// Reset internals
	this->ResetCompilerInternals();

}

void AAC::ResetCompilerInternals() {

	// Reset current procedure ID
	m_currentProcID = 0;

}

AAC_CompileResult AAC::CompileFromAbstractSyntaxTrees(std::vector<AA_AST*> trees) {

	// Compile Result container
	AAC_CompileResult result;
	AAC_CompileErrorMessage err;

	// Reset the internals of the compiler
	this->ResetCompilerInternals();

	// Collapse global tree sections
	this->CollapseGlobalScope(trees);

	// Run the static checkers
	CompiledStaticChecks staticChecks = this->NewStaticCheck();
	if (COMPILE_ERROR(err = this->RunStaticOperations(trees, staticChecks, L""))) {
		result.firstMsg = err;
		result.success = false;
		return result;
	}

	// Compiled procedure results
	std::vector<AAC::CompiledProcedure> compileResults;

	// Compile all the trees into individual procedures
	for (size_t i = 0; i < trees.size(); i++) {

		// If it's a class, we'll have to compile it just a bit differently
		if (trees[i]->GetRoot()->type == AA_AST_NODE_TYPE::classdecl) {

			// For all sub-elements of class decl
			for (size_t j = 0; j < trees[i]->GetRoot()->expressions[0]->expressions.size(); j++) {

				// Make sure it's a function decleration
				if (trees[i]->GetRoot()->expressions[0]->expressions[j]->type == AA_AST_NODE_TYPE::fundecl) {

					// Compiled AST
					compileResults.push_back(this->CompileProcedureFromASTNode(trees[i]->GetRoot()->expressions[0]->expressions[j], staticChecks));

				}

			}

		} else {

			// Generated bytecode
			compileResults.push_back(CompileProcedureFromAST(trees[i], staticChecks));

		}

	}

	// Map the procedures to their respective functions
	//staticChecks.exportSignatures = this->MapProcedureToSignature(staticChecks, compileResults);

	// Compile all procedures into bytecode
	result.result = CompileFromProcedures(compileResults, staticChecks, 0);

	// Write operations out in a readable format
	if (m_outfile != L"") {
		aa::dump_instructions(m_outfile, compileResults);
	}

	// Set success flag to true
	result.success = true;

	// Aslo return bytecode (so we can execute it directly)
	return result;

}

AAC::CompiledProcedure AAC::CompileProcedureFromAST(AA_AST* pAbstractTree, CompiledStaticChecks staticData) {
	return this->CompileProcedureFromASTNode(pAbstractTree->GetRoot(), staticData);
}

AAC::CompiledProcedure AAC::CompileProcedureFromASTNode(AA_AST_NODE* pASTNode, CompiledStaticChecks staticData) {

	// Procedure for the abstract tree
	CompiledProcedure proc;
	proc.node = pASTNode;

	// Compile the execution stack
	proc.procOperations = CompileAST(pASTNode, proc.procEnvironment, staticData);

	// Return the procedure
	return proc;

}

void AAC::CollapseGlobalScope(std::vector<AA_AST*>& trees) {

	size_t i = 0;
	AA_AST_NODE* pGlobalScope = new AA_AST_NODE(L"<GlobalScope>", AA_AST_NODE_TYPE::block, AACodePosition(0, 0));

	while (i < trees.size()) {

		AA_AST_NODE_TYPE nodeType = trees[i]->GetRoot()->type;

		if (nodeType != AA_AST_NODE_TYPE::fundecl && nodeType != AA_AST_NODE_TYPE::classdecl && 
			nodeType != AA_AST_NODE_TYPE::usingspecificstatement && nodeType != AA_AST_NODE_TYPE::usingstatement && // Using statements need to preserve their position in code
			nodeType != AA_AST_NODE_TYPE::name_space) { // namespaces are per definition not part of the global namespace/scope

			pGlobalScope->expressions.push_back(trees[i]->GetRoot());
			trees.erase(trees.begin() + i);

		} else {
			i++;
		}

	}

	if (pGlobalScope->expressions.size() > 0) {
		trees.insert(trees.begin(), new AA_AST(pGlobalScope));
	}

}

AAC::CompiledStaticChecks AAC::NewStaticCheck() {

	// Static check obj
	CompiledStaticChecks staticChecks;

	// Static function checks
	staticChecks.registeredTypes = AATypeChecker::DefaultTypeEnv;

	// For all classes
	for (size_t i = 0; i < this->m_preregisteredClasses.size(); i++) {

		// Add type name
		staticChecks.registeredTypes.Add(this->m_preregisteredClasses[i].name);

		// Add the class
		staticChecks.registeredClasses.Add(this->m_preregisteredClasses[i]);

	}

	// For all functions
	for (size_t i = 0; i < this->m_preregisteredFunctions.size(); i++) {

		// Add function
		staticChecks.registeredFunctions.Add(this->m_preregisteredFunctions[i]);

	}

	// Return static checks obj
	return staticChecks;

}

AAC_CompileErrorMessage AAC::RunStaticOperations(std::vector<AA_AST*> trees, CompiledStaticChecks& staticChecks, std::wstring currentnamespace) {
	
	// Compile error container
	AAC_CompileErrorMessage err;

	// For all input trees, register namespaces
	for (size_t i = 0; i < trees.size(); i++) {

		// Is name space?
		if (trees[i]->GetRoot()->type == AA_AST_NODE_TYPE::name_space) {

			// Temp abstract syntax trees
			std::vector<AA_AST*> tempASTTrees;

			// Add elements of namespaces
			for (size_t j = 0; j < trees[i]->GetRoot()->expressions.size(); j++) {
				tempASTTrees.push_back(new AA_AST(trees[i]->GetRoot()->expressions[j]));
			}

			// Run operation
			err = this->RunStaticOperations(tempASTTrees, staticChecks, currentnamespace);

			// Delete all the temporary ASTs
			for (size_t j = 0; j < tempASTTrees.size(); j++) {
				delete tempASTTrees[j];
			}

			// Any compile error?
			if (COMPILE_ERROR(err)) {
				return err;
			}

		}

	}

	// For all input trees, register classes
	for (size_t i = 0; i < trees.size(); i++) {

		// Make sure it's a class decleration
		if (trees[i]->GetRoot()->type == AA_AST_NODE_TYPE::classdecl) {

			// Get the class
			CompiledClass cc = this->RegisterClass(trees[i]->GetRoot());

			// Register class
			staticChecks.registeredClasses.Add(cc);

			for (size_t j = 0; j < cc.methods.Size(); j++) {

				AAC::CompiledStaticChecks::SigPointer sP = AAC::CompiledStaticChecks::SigPointer(cc.methods.At(j).sig, cc.methods.At(j).source);
				sP.procID = cc.methods.At(j).procID;

				staticChecks.registeredFunctions.Add(sP);

			}

			// Add class name to registered types as well
			staticChecks.registeredTypes.Add(cc.name);

		}

	}

	// For all input trees, register possible functions
	for (size_t i = 0; i < trees.size(); i++) {

		// Make sure it's a function decleration
		if (trees[i]->GetRoot()->type == AA_AST_NODE_TYPE::fundecl) {

			// Register functions
			staticChecks.registeredFunctions.Add(RegisterFunctions(trees[i]->GetRoot()));

		}

	}

	// For all input trees, run static type checker
	for (size_t i = 0; i < trees.size(); i++) {

		// Any possible type error
		AATypeChecker::Error tErr;

		// Register functions
		if (!this->TypecheckAST(trees[i], staticChecks, tErr)) {

			// Create error message based on type error
			err.errorMsg = tErr.errMsg;
			err.errorSource = tErr.errSrc;
			err.errorType = tErr.errType;

			// Return error message
			return err;

		}

	}

	// For all input trees, optimize
	for (size_t i = 0; i < trees.size(); i++) {

		// Simplify the abstract tree
		trees[i]->Simplify();

	}

	// Return result of static function check
	return NO_COMPILE_ERROR_MESSAGE;

}

bool AAC::TypecheckAST(AA_AST* pTree, CompiledStaticChecks staticData, AATypeChecker::Error& typeError) {

	// Currently, we just run a simple type check
	AATypeChecker checker = AATypeChecker(pTree, staticData.registeredTypes, staticData.GetSignatures(), staticData.registeredClasses);

	// ** Apply special stuff here. ** //

	// Run type checker
	if (!checker.TypeCheck()) {

		// Get error
		typeError = checker.GetErrorMessage();
		
		// Return false => there were errors
		return false;

	} else {

		// Return true => no errors
		return true;

	}

}

CompiledClass AAC::RegisterClass(AA_AST_NODE* pNode) {

	// Compiled Class data
	CompiledClass cc;
	cc.name = pNode->content;
	cc.classByteSz = 0;

	// Function bodies to correct
	std::vector<AA_AST_NODE*> funcBodyNodes;

	if (pNode->expressions.size() == 1 && pNode->expressions[0]->type == AA_AST_NODE_TYPE::classbody) {

		for (size_t i = 0; i < pNode->expressions[0]->expressions.size(); i++) {

			if (pNode->expressions[0]->expressions[i]->type == AA_AST_NODE_TYPE::fundecl) {

				// We need to push a "this" into the method params list
				m_classCompiler->RedefineFunDecl(cc.name, pNode->expressions[0]->expressions[i]);

				// Register function
				CompiledStaticChecks::SigPointer sig = this->RegisterFunction(pNode->expressions[0]->expressions[i]);
				CompiledClassMethod method;
				method.sig = sig.funcSig;
				method.sig.isClassMethod = true;
				method.source = pNode->expressions[0]->expressions[i];
				method.procID = sig.procID;
				method.isCtor = sig.funcSig.name == (cc.name + L"::" + cc.name);

				// Update return count in case of a constructor
				if (method.isCtor) {
					pNode->expressions[0]->expressions[i]->tags["returncount"] = 1;
				}

				// Add method to class definition
				cc.methods.Add(method);

				if (pNode->expressions[0]->expressions[i]->expressions.size() >= 3) { // make sure it's declared
					funcBodyNodes.push_back(pNode->expressions[0]->expressions[i]->expressions[2]);
				}

			} else if (pNode->expressions[0]->expressions[i]->type == AA_AST_NODE_TYPE::vardecl) {
			
				CompiledClassField field;
				field.fieldID = (int)cc.fields.Size();
				field.name = pNode->expressions[0]->expressions[i]->content;
				field.type = pNode->expressions[0]->expressions[i]->expressions[0]->content;

				cc.fields.Add(field);

			} else {

				printf("Unknown class member type found");

			}

		}

	}

	// Correct incorrect references (eg. field access)
	m_classCompiler->CorrectReferences(&cc, funcBodyNodes);

	// Calculate the class size in memory
	cc.classByteSz = m_classCompiler->CalculateMemoryUse(cc);

	return cc;

}

aa::list<AAC::CompiledStaticChecks::SigPointer> AAC::RegisterFunctions(AA_AST_NODE* pNode) {

	aa::list<AAC::CompiledStaticChecks::SigPointer> signatures;
	signatures.Add(this->RegisterFunction(pNode));

	return signatures;

}

AAC::CompiledStaticChecks::SigPointer AAC::RegisterFunction(AA_AST_NODE* pNode) {

	AAFuncSignature sig;
	sig.name = pNode->content;
	sig.returnType = pNode->expressions[0]->content;

	for (AA_AST_NODE* arg : pNode->expressions[1]->expressions) {

		AAFuncParam param;
		param.identifier = arg->content;
		param.type = arg->expressions[0]->content;

		sig.parameters.push_back(param);

	}

	AAC::CompiledStaticChecks::SigPointer sP = AAC::CompiledStaticChecks::SigPointer(sig, pNode);
	sP.procID = ++m_currentProcID;

	pNode->tags["returncount"] = this->GetReturnCount(&sig);

	return sP;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileAST(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	// Stack
	aa::list<CompiledAbstractExpression> executionStack;

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::binop: {
		executionStack.Add(CompileBinaryOperation(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::unop: {
		executionStack.Add(CompileUnaryOperation(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::block: {
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			executionStack.Add(this->CompileAST(pNode->expressions[i], cTable, staticData));
		}
		break;
	}
	case AA_AST_NODE_TYPE::funcbody: {
		for (size_t i = 0; i < pNode->expressions.size(); i++) {			
			executionStack.Add(this->CompileAST(pNode->expressions[i], cTable, staticData));
		}		
		break;
	}
	case AA_AST_NODE_TYPE::fundecl: {
		if (pNode->expressions.size() == 3) { // We've got the actual function body => compile it
			this->CompileAST(pNode->expressions[1], cTable, staticData);
			aa::list<CompiledAbstractExpression> args = this->CompileFuncArgs(pNode, cTable, staticData);
			aa::list<CompiledAbstractExpression> body = aa::list<CompiledAbstractExpression>::Merge(args, this->CompileAST(pNode->expressions[2], cTable, staticData));
			CompiledAbstractExpression retCAE;
			retCAE.bc = AAByteCode::RET;
			retCAE.argCount = 1;
			retCAE.argValues[0] = pNode->tags["returncount"];
			body.Add(retCAE);
			executionStack.Add(body);
		}
		break;
	}
	case AA_AST_NODE_TYPE::funcall: {
		executionStack .Add(CompileFunctionCall(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::funarglist: {
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			cTable.identifiers.Add(pNode->expressions[i]->content);
		}
		break;
	}
	case AA_AST_NODE_TYPE::ifstatement: {
		executionStack.Add(CompileConditionalBlock(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::elseifstatement: {
		executionStack.Add(CompileConditionalBlock(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::elsestatement: {
		executionStack.Add(this->CompileAST(pNode->expressions[0], cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::forstatement: {
		executionStack.Add(this->CompileForBlock(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::whilestatement: {
		executionStack.Add(this->CompileWhileBlock(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::dowhilestatement: {
		executionStack.Add(this->CompileDoWhileBlock(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::newstatement: {
		executionStack.Add(this->CompileNewStatement(pNode, cTable, staticData));
		
		break;
	}
	case AA_AST_NODE_TYPE::classctorcall: {
		executionStack.Add(this->HandleCtorCall(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::fieldaccess: {
		executionStack.Add(this->CompileAccessorOperation(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::field: {
		executionStack.Add(this->HandleFieldPush(pNode, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::callaccess: {
		executionStack.Add(this->HandleMemberCall(pNode, cTable, staticData));
		break;
	}
	// Implicit return
	case AA_AST_NODE_TYPE::variable:
	case AA_AST_NODE_TYPE::intliteral:
	case AA_AST_NODE_TYPE::floatliteral:
	case AA_AST_NODE_TYPE::charliteral:
	case AA_AST_NODE_TYPE::stringliteral:
	case AA_AST_NODE_TYPE::boolliteral:
	case AA_AST_NODE_TYPE::nullliteral:
	{
		executionStack.Add(HandleStackPush(cTable, pNode, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::index: {
		executionStack.Add(HandleIndexPush(pNode, cTable, staticData));
		break;
	}
	default:
		break;
	}

	return executionStack;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileBinaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	aa::list<CompiledAbstractExpression> opList;

	CompiledAbstractExpression binopCAE;
	binopCAE.argCount = 0;
	binopCAE.bc = GetBytecodeFromBinaryOperator(pNode->content, pNode->expressions[0]->type);

	if (binopCAE.bc == AAByteCode::SETVAR) {

		binopCAE.argCount = 1;
		binopCAE.argValues[0] = HandleDecl(cTable, pNode->expressions[0]);
		opList.Add(HandleStackPush(cTable, pNode->expressions[1], staticData));

	} else if (binopCAE.bc == AAByteCode::SETFIELD) {
	
		opList.Add(HandleVarPush(cTable, pNode->expressions[0]->expressions[0]));
		opList.Add(HandleStackPush(cTable, pNode->expressions[1], staticData));

		binopCAE.argCount = 1;
		binopCAE.argValues[0] = pNode->expressions[0]->expressions[1]->tags["fieldid"];

	} else if (binopCAE.bc == AAByteCode::SETELEM) {
	
		opList.Add(HandleVarPush(cTable, pNode->expressions[0]->expressions[0]));
		opList.Add(HandleStackPush(cTable, pNode->expressions[0]->expressions[1], staticData));
		opList.Add(HandleStackPush(cTable, pNode->expressions[1], staticData));

	} else {

		if (pNode->tags["useCall"]) {

			binopCAE.bc = (pNode->tags["operatorIsVM"]) ? AAByteCode::VMCALL : AAByteCode::CALL;
			binopCAE.argCount = 2;
			binopCAE.argValues[0] = pNode->tags["operatorProcID"];
			binopCAE.argValues[1] = 2;

		}

		opList.Add(HandleStackPush(cTable, pNode->expressions[0], staticData));
		opList.Add(HandleStackPush(cTable, pNode->expressions[1], staticData));

	}

	opList.Add(binopCAE);

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileUnaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	aa::list<CompiledAbstractExpression> opList;

	CompiledAbstractExpression unopCAE;
	unopCAE.argCount = 0;
	unopCAE.bc = GetBytecodeFromUnaryOperator(pNode->content);

	opList.Add(HandleStackPush(cTable, pNode->expressions[0], staticData));
	opList.Add(unopCAE);

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileAccessorOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	aa::list<CompiledAbstractExpression> opList;

	opList.Add(this->CompileAST(pNode->expressions[0], cTable, staticData)); // CALL ACCESS NOT HANDLEs => Implement that
	opList.Add(this->CompileAST(pNode->expressions[1], cTable, staticData));

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileFunctionCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	aa::list<CompiledAbstractExpression> opList;

	int progArgs = 0; // Going real safe here
	bool progIsVmCall = false; // Hopefully we can make a little use of C++ functions as possible, so this should be the default
	int procID = this->FindBestFunctionMatch(staticData, pNode, progArgs, progIsVmCall);

	if (progArgs > 0) {
		for (AA_AST_NODE* pArg : pNode->expressions) {
			opList.Add(this->HandleStackPush(cTable, pArg, staticData));
		}
	}

	CompiledAbstractExpression callCAE;
	callCAE.argCount = 2;
	callCAE.bc = (progIsVmCall) ? AAByteCode::VMCALL : AAByteCode::CALL;
	callCAE.argValues[0] = procID;
	callCAE.argValues[1] = progArgs;

	opList.Add(callCAE);

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileFuncArgs(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	aa::list<CompiledAbstractExpression> opList;

	for (size_t i = 0; i < pNode->expressions[1]->expressions.size(); i++) {

		// Create setvar operation
		CompiledAbstractExpression argCAE;
		argCAE.bc = AAByteCode::SETVAR;
		argCAE.argCount = 1;

		// Set the argument of size() - 1 - i (the inverse) becausue of the push-pop mechanism used by the AAVM to pass call arguments
		argCAE.argValues[0] = cTable.identifiers.IndexOf(pNode->expressions[1]->expressions[pNode->expressions[1]->expressions.size() - 1 - i]->content);

		// Add operation
		opList.Add(argCAE);

	}

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileConditionalBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	// Operations list
	aa::list<CompiledAbstractExpression> opList;

	// Push condition
	opList.Add(this->CompileBinaryOperation(pNode->expressions[0]->expressions[0], cTable, staticData)); // will throw error when multiple conditions become possible

	size_t tOffset = 0;
	std::vector<aa::list<CompiledAbstractExpression>> allBodies;

	for (size_t i = 1; i < pNode->expressions.size(); i++) {
		aa::list<CompiledAbstractExpression> opLst = this->CompileAST(pNode->expressions[i], cTable, staticData);
		tOffset += opLst.Size();
		if (i != pNode->expressions.size() - 1) {
			tOffset++;
		}
		allBodies.push_back(opLst);
	}

	// Create jump if false condition
	CompiledAbstractExpression jmpInstruction;
	jmpInstruction.bc = AAByteCode::JMPF;
	jmpInstruction.argCount = 1;
	jmpInstruction.argValues[0] = (int)allBodies[0].Size() + 1;

	// Add jump instruction after condition
	opList.Add(jmpInstruction);

	size_t cutJmp = 0;

	for (size_t i = 0; i < allBodies.size(); i++) {

		if (i != allBodies.size() - 1) {

			cutJmp += allBodies[i].Size() + 1;

			CompiledAbstractExpression jmpInstruction;
			jmpInstruction.bc = AAByteCode::JMP;
			jmpInstruction.argCount = 1;
			jmpInstruction.argValues[0] = (int)(tOffset - cutJmp);

			allBodies[i].Add(jmpInstruction);

		}

		// Add body
		opList.Add(allBodies[i]);

	}

	// return oplist
	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileForBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	// Operations list
	aa::list<CompiledAbstractExpression> opList;

	// Compile individual
	aa::list<CompiledAbstractExpression> init = CompileAST(pNode->expressions[0], cTable, staticData);
	aa::list<CompiledAbstractExpression> condition = CompileAST(pNode->expressions[1], cTable, staticData);
	aa::list<CompiledAbstractExpression> afterthought = CompileAST(pNode->expressions[2], cTable, staticData);
	aa::list<CompiledAbstractExpression> body = CompileAST(pNode->expressions[3], cTable, staticData);

	// Merge the oplist with the initialisor
	opList.Add(init);

	// Merge the oplist with the afterthought
	body = aa::list<CompiledAbstractExpression>::Merge(body, afterthought);

	// The jump instruction that jumps back to the top of the condition
	CompiledAbstractExpression jmpBckIns;
	jmpBckIns.bc = AAByteCode::JMP;
	jmpBckIns.argCount = 1;
	jmpBckIns.argValues[0] = -(int)(body.Size() + condition.Size() + 2); // (+2 because of the additional jmp instruction and because a jmp instruction adds 1 by itself)

	// The jump if false instruction that will skip the body if the condition no longer holds
	CompiledAbstractExpression jmpDntLoopIns;
	jmpDntLoopIns.bc = AAByteCode::JMPF;
	jmpDntLoopIns.argCount = 1;
	jmpDntLoopIns.argValues[0] = (int)(body.Size() + 1);

	// Add jump instructions
	condition.Add(jmpDntLoopIns);
	body.Add(jmpBckIns);

	// Add the condition and body to operations list
	opList.Add(condition);
	opList.Add(body);

	// return oplist
	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	// Operations list
	aa::list<CompiledAbstractExpression> opList;

	// Compile individual elements
	aa::list<CompiledAbstractExpression> condition = CompileAST(pNode->expressions[0], cTable, staticData);
	aa::list<CompiledAbstractExpression> body = CompileAST(pNode->expressions[1], cTable, staticData);

	// The jump if false instruction that will skip the body if the condition no longer holds
	CompiledAbstractExpression jmpDntLoopIns;
	jmpDntLoopIns.bc = AAByteCode::JMPF;
	jmpDntLoopIns.argCount = 1;
	jmpDntLoopIns.argValues[0] = (int)(body.Size() + 1);

	// Add jump instructions
	condition.Add(jmpDntLoopIns);

	// The jump instruction that jumps back to the top of the condition
	CompiledAbstractExpression jmpBckIns;
	jmpBckIns.bc = AAByteCode::JMP;
	jmpBckIns.argCount = 1;
	jmpBckIns.argValues[0] = -(int)(body.Size() + condition.Size() + 1);

	// Push the jmp back
	body.Add(jmpBckIns);

	// Add operations to the op list
	opList.Add(condition);
	opList.Add(body);	

	// return oplist
	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileDoWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	// Operations list
	aa::list<CompiledAbstractExpression> opList;

	// Compile individual elements
	aa::list<CompiledAbstractExpression> condition = CompileAST(pNode->expressions[1], cTable, staticData);
	aa::list<CompiledAbstractExpression> body = CompileAST(pNode->expressions[0], cTable, staticData);

	// The jump if false instruction that will skip the body if the condition no longer holds
	CompiledAbstractExpression jmpDntLoopIns;
	jmpDntLoopIns.bc = AAByteCode::JMPT;
	jmpDntLoopIns.argCount = 1;
	jmpDntLoopIns.argValues[0] = -(int)(body.Size() + condition.Size() + 1);

	// Add jump instructions
	condition.Add(jmpDntLoopIns);

	// Add operations to the op list
	opList.Add(body);
	opList.Add(condition);

	// return oplist
	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileNewStatement(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	// Operations list
	aa::list<CompiledAbstractExpression> opList;

	// Is it a constructor call?
	if (pNode->expressions[0]->type == AA_AST_NODE_TYPE::classctorcall) {
		
		// Let the specific function handle that
		opList.Add(this->HandleCtorCall(pNode->expressions[0], cTable, staticData));

	} else { // Else it's an array creation

		// Create allocation instruction
		CompiledAbstractExpression allocCAE;
		allocCAE.bc = AAByteCode::HALLOC;
		allocCAE.argCount = 1;
		allocCAE.argValues[0] = std::stoi(pNode->expressions[0]->expressions[1]->content) * sizeof(AAVal);

		// Add to oplist
		opList.Add(allocCAE);

	}

	// return oplist
	return opList;

}

bool AAC::IsConstant(AA_AST_NODE_TYPE type) {
	return type == AA_AST_NODE_TYPE::intliteral || type == AA_AST_NODE_TYPE::boolliteral || type == AA_AST_NODE_TYPE::charliteral ||
		type == AA_AST_NODE_TYPE::floatliteral || type == AA_AST_NODE_TYPE::stringliteral;
}

bool AAC::IsVariable(AA_AST_NODE_TYPE type) {
	return type == AA_AST_NODE_TYPE::variable;
}

bool AAC::IsDecleration(AA_AST_NODE_TYPE type) {
	return type == AA_AST_NODE_TYPE::vardecl;
}

AAByteCode AAC::GetBytecodeFromBinaryOperator(std::wstring ws, AA_AST_NODE_TYPE lhsType) {

	if (ws == L"+") {
		return AAByteCode::ADD;
	} else if (ws == L"-") {
		return AAByteCode::SUB;
	} else if (ws == L"*") {
		return AAByteCode::MUL;
	} else if (ws == L"/") {
		return AAByteCode::DIV;
	} else if (ws == L"%") {
		return AAByteCode::MOD;
	} else if (ws == L"=") {
		if (lhsType == AA_AST_NODE_TYPE::fieldaccess) {
			return AAByteCode::SETFIELD;
		} else if (lhsType == AA_AST_NODE_TYPE::index) {
			return AAByteCode::SETELEM;
		} else {
			return AAByteCode::SETVAR;
		}
	} else if (ws == L"==") {
		return AAByteCode::CMPE;
	} else if (ws == L"!=") {
		return AAByteCode::CMPNE;
	} else if (ws == L"<") {
		return AAByteCode::LE;
	} else if (ws == L">") {
		return AAByteCode::GE;
	} else if (ws == L"<=") {
		return AAByteCode::LEQ;
	} else if (ws == L">=") {
		return AAByteCode::GEQ;
	} else {
		return AAByteCode::NOP;
	}

}

AAByteCode AAC::GetBytecodeFromUnaryOperator(std::wstring ws) {

	if (ws == L"!") {
		return AAByteCode::LNEG;
	} else if (ws == L"-") {
		return AAByteCode::NNEG;
	} else {
		return AAByteCode::NOP;
	}

}

int AAC::GetReturnCount(void* fs) {

	AAFuncSignature funcSig = *(AAFuncSignature*)fs;

	if (funcSig.returnType == L"void") {
		return 0;
	} else {
		return 1;
	}

}

AAC::CompiledAbstractExpression AAC::HandleConstPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode) {

	// TODO: Update this to keep track of const counter (so two cases of 5's is added to the const table)
	//		 If a single const is used, we don't want to waste time looking it up

	CompiledAbstractExpression pushOp;
	pushOp.bc = AAByteCode::PUSHC;
	pushOp.argCount = 1;

	AA_AnyLiteral aLit = AA_AnyLiteral();
	AALiteralType lType = AALiteralType::Int;

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::intliteral:
		aLit.i.val = std::stoi(pNode->content);
		lType = AALiteralType::Int;
		break;
	case AA_AST_NODE_TYPE::boolliteral:
		aLit.b.val = pNode->content == L"true";
		lType = AALiteralType::Boolean;
		break;
	case AA_AST_NODE_TYPE::floatliteral:
		aLit.f.val = std::stof(pNode->content);
		lType = AALiteralType::Float;
		break;
	case AA_AST_NODE_TYPE::charliteral:
		aLit.c.val = pNode->content[0];
		lType = AALiteralType::Char;
		break;
	case AA_AST_NODE_TYPE::stringliteral:
		aLit.s.val = new wchar_t[pNode->content.length()];
		aLit.s.len = pNode->content.length();
		wcscpy(aLit.s.val, pNode->content.c_str());
		lType = AALiteralType::String;
		break;
	case AA_AST_NODE_TYPE::nullliteral:
		aLit.n = AA_NullLiteral();
		lType = AALiteralType::Null;
		break;
	default:
		break;
	}

	AA_Literal lit;
	lit.lit = aLit;
	lit.tp = lType;

	int i = -1;
	if (cTable.constValues.Contains(lit)) {
		i = cTable.constValues.IndexOf(lit);
	} else {
		i = cTable.constValues.Size();
		cTable.constValues.Add(lit);
	}

	pushOp.argValues[0] = i;

	return pushOp;

}

AAC::CompiledAbstractExpression AAC::HandleVarPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode) {

	CompiledAbstractExpression pushCAE;
	pushCAE.bc = AAByteCode::GETVAR;
	pushCAE.argCount = 1;

	if (cTable.identifiers.Contains(pNode->content)) {
		pushCAE.argValues[0] = cTable.identifiers.IndexOf(pNode->content);
		return pushCAE;
	} else {
		printf("Using a variable before it's declared!");
		return pushCAE;
	}

}

AAC::CompiledAbstractExpression AAC::HandleFieldPush(AA_AST_NODE* pNode, CompiledStaticChecks staticData) {

	CompiledAbstractExpression pushCAE;
	pushCAE.bc = AAByteCode::GETFIELD;
	pushCAE.argCount = 1;
	pushCAE.argValues[0] = pNode->tags["fieldid"];

	return pushCAE;

}

int AAC::HandleDecl(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode) {

	int i = -1;
	if (cTable.identifiers.Contains(pNode->content)) {
		i = cTable.identifiers.IndexOf(pNode->content);
	} else {
		i = cTable.identifiers.Size();
		cTable.identifiers.Add(pNode->content);
	}

	return i;

}

aa::list<AAC::CompiledAbstractExpression> AAC::HandleStackPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode, CompiledStaticChecks staticData) {

	aa::list<CompiledAbstractExpression> opList;

	if (IsConstant(pNode->type)) {
		opList.Add(this->HandleConstPush(cTable, pNode));
	} else if (IsVariable(pNode->type)) {
		opList.Add(this->HandleVarPush(cTable, pNode));
	} else {
		opList.Add(CompileAST(pNode, cTable, staticData));
	}

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::HandleCtorCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& ctable, CompiledStaticChecks staticData) {

	aa::list<CompiledAbstractExpression> opList;

	CompiledClass cc = m_classCompiler->FindClassFromCtor(pNode->content, staticData.registeredClasses);
	CompiledClassMethod ctor = m_classCompiler->FindBestCtor(&cc);

	CompiledAbstractExpression newCAE;
	newCAE.argCount = 1;
	newCAE.argValues[0] = cc.classByteSz;
	newCAE.bc = AAByteCode::HALLOC;

	opList.Add(newCAE);

	for (size_t i = 0; i < pNode->expressions.size(); i++) {
		opList.Add(this->CompileAST(pNode->expressions[i], ctable, staticData));
	}

	CompiledAbstractExpression callCAE;
	callCAE.bc = (ctor.sig.isVMFunc) ? AAByteCode::VMCALL : AAByteCode::CALL;
	callCAE.argCount = 2;
	callCAE.argValues[0] = ctor.procID;
	callCAE.argValues[1] = (int)ctor.sig.parameters.size();

	opList.Add(callCAE);

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::HandleMemberCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	// The op list to get from member call
	aa::list<CompiledAbstractExpression> opList;

	// Create a temporary node
	AA_AST_NODE* tempNode = new AA_AST_NODE(pNode->expressions[1]->content, AA_AST_NODE_TYPE::funcall, pNode->expressions[0]->position);
	tempNode->expressions = pNode->expressions[1]->expressions;
	tempNode->expressions.insert(tempNode->expressions.begin(), pNode->expressions[0]);

	// Call member
	opList = this->CompileFunctionCall(tempNode, cTable, staticData);

	// Delete the temporary node again
	delete tempNode;

	// Return the op list
	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::HandleIndexPush(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	// The op list to get from member call
	aa::list<CompiledAbstractExpression> opList;

	// Handle var push (eg. actually get the variable we're looking for)
	opList.Add(this->HandleVarPush(cTable, pNode->expressions[0]));

	// Handle field fetch
	opList.Add(this->CompileAST(pNode->expressions[1], cTable, staticData));

	// Create dynamic method of fetching element operation
	CompiledAbstractExpression getElemCAE;
	getElemCAE.bc = AAByteCode::GETELEM;
	getElemCAE.argCount = 0;

	// Add to operations list
	opList.Add(getElemCAE);

	// Return the op list
	return opList;

}

void AAC::ConstTableToByteCode(CompiledEnviornmentTable constTable, aa::bstream& wss) {

	wss << (int)constTable.constValues.Size();

	for (size_t i = 0; i < constTable.constValues.Size(); i++) {

		AA_Literal lit = constTable.constValues.At(i);

		wss << (unsigned char)lit.tp;
		
		switch (lit.tp) {
		case AALiteralType::Int:
			wss << lit.lit.i.val;
			break;
		case AALiteralType::Boolean:
			wss << lit.lit.b.val;
			break;
		case AALiteralType::Float:
			wss << lit.lit.f.val;
			break;
		case AALiteralType::Char:
			wss << lit.lit.c.val;
			break;
		case AALiteralType::String:
			wss << (int)lit.lit.s.len;
			wss << std::wstring(lit.lit.s.val);
			break;
		default:
			break;
		}

	}

	wss << (int)constTable.identifiers.Size();

	for (size_t i = 0; i < constTable.identifiers.Size(); i++) {
		wss << (int)i;
	}

}

void AAC::ConvertToBytes(CompiledAbstractExpression expr, aa::bstream& bis) {

	bis << (unsigned char)expr.bc;

	for (int i = 0; i < expr.argCount; i++) {
		bis << expr.argValues[i];
	}

}

AAC_Out AAC::CompileFromProcedures(std::vector<CompiledProcedure> procedures, CompiledStaticChecks staticCompileData, int entryPoint) {

	// Bytecode compilation result
	AAC_Out compileBytecodeResult;

	// Byte stream
	aa::bstream bis;

	// Write the exported signature count
	bis << 0; // (int)staticCompileData.exportSignatures.size();
	/*
	// Write the exported signatures
	for (size_t s = 0; s < staticCompileData.exportSignatures.size(); s++) {

		// Write out the export signatures
		bis << (int)staticCompileData.exportSignatures[s].name.length();
		bis << staticCompileData.exportSignatures[s].name;
		bis << staticCompileData.exportSignatures[s].procID;

	}*/

	// Write procedure count
	bis << (int)procedures.size();

	// Write entry point (Pointer to the first operation to execute)
	bis << entryPoint;

	// For all procedures
	for (size_t p = 0; p < procedures.size(); p++) {

		// Convert constants table to bytecode
		ConstTableToByteCode(procedures[p].procEnvironment, bis);

		// Write amount of operations
		bis << (int)procedures[p].procOperations.Size();

		// Write all expressions in their compiled formats
		for (size_t i = 0; i < procedures[p].procOperations.Size(); i++) {
			this->ConvertToBytes(procedures[p].procOperations.At(i), bis);
		}

	}

	// Get size and allocate memory buffer
	compileBytecodeResult.length = bis.length();
	compileBytecodeResult.bytes = new unsigned char[(int)compileBytecodeResult.length];

	// Copy stream into array
	memcpy(compileBytecodeResult.bytes, bis.array(), (int)compileBytecodeResult.length);

	// Return the resulting bytecode
	return compileBytecodeResult;

}
/*
std::vector<CompiledSignature> AAC::MapProcedureToSignature(CompiledStaticChecks staticCheck, std::vector<AAC::CompiledProcedure> procedureLs) {

	std::vector<CompiledSignature> procSigs;

	for (size_t i = 0; i < staticCheck.registeredFunctions.Size(); i++) {

		for (size_t j = 0; j < procedureLs.size(); j++) {

			CompiledStaticChecks::SigPointer funcSig = staticCheck.registeredFunctions.At(i);

			if (funcSig.node == procedureLs[j].node) {

				CompiledSignature sig;
				sig.procID = j;
				sig.name = L"@" + funcSig.funcSig.returnType + L"?" + funcSig.funcSig.name + L"(";

				for (size_t p = 0; p < funcSig.funcSig.parameters.size(); p++) {
					sig.name += L"&" + funcSig.funcSig.parameters[p].type + L"@" + funcSig.funcSig.parameters[p].identifier;
				}

				sig.name += L")";

				procSigs.push_back(sig);

			}

		}

	}

	return procSigs;

}
*/
int AAC::FindBestFunctionMatch(CompiledStaticChecks staticCheck, AA_AST_NODE* pNode, int& argCount, bool& isVMCall) { // TODO: Make the typechecker do this

	std::wstring funcName = pNode->content;

	for (size_t i = 0; i < staticCheck.registeredFunctions.Size(); i++) {
		AAC::CompiledStaticChecks::SigPointer sig = staticCheck.registeredFunctions.At(i);
		if (sig.funcSig.name.compare(funcName) == 0) {
			if (pNode->expressions.size() == sig.funcSig.parameters.size()) {
				bool isMatch = true; //false;
				for (AAFuncParam p : sig.funcSig.parameters) {

				}
				if (isMatch) {
					isVMCall = sig.funcSig.isVMFunc;
					argCount = (int)sig.funcSig.parameters.size();
					return sig.procID;
				}
			}
		}
	}

	//wprintf(L"Unknown function! %s", funcName.c_str());

	argCount = 0;
	isVMCall = false;
	return 0;

}

void AAC::AddVMClass(CompiledClass cc) {
	m_preregisteredClasses.push_back(cc);
}

void AAC::AddVMFunction(AAFuncSignature sig, int procID) {

	CompiledStaticChecks::SigPointer sP = CompiledStaticChecks::SigPointer(sig, NULL);
	sP.procID = procID;
	
	m_preregisteredFunctions.push_back(sP);

}
