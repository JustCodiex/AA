#include "AAC.h"
#include "AAB2F.h"
#include "AAVal.h"
#include <stack>
#include "AAVM.h"

AAC::AAC(AAVM* pVM) {

	// Set VM
	m_pAAVM = pVM;

	// Set support classes to null
	m_classCompiler = 0;
	m_staticAnalyser = 0;

	// Set current procID to 0
	m_currentProcID = 0;

}

void AAC::SetupCompiler() {

	// Create instance of class compiler
	m_classCompiler = new AAClassCompiler;

	// Create instance of static analyser
	m_staticAnalyser = new AAStaticAnalysis(this);

	// Reset internals
	this->ResetCompilerInternals();

}

void AAC::ResetCompilerInternals() {

	// Reset current procedure ID
	m_currentProcID = 0;

	// Reset registered types and other static configurations
	m_staticAnalyser->Reset(m_preregisteredFunctions, m_preregisteredClasses, m_preregisteredNamespaces);

}

AAC_CompileResult AAC::CompileFromAbstractSyntaxTrees(std::vector<AA_AST*> trees) {

	// Compile Result container
	AAC_CompileResult result;
	AAC_CompileErrorMessage err;
	AAStaticEnvironment senv;

	// Reset the internals of the compiler
	this->ResetCompilerInternals();

	// Run the static checkers
	if (COMPILE_ERROR(err = this->RunStaticOperations(trees, senv))) {
		result.firstMsg = err;
		result.success = false;
		return result;
	}

	// Compiled procedure results
	aa::list<AAC::CompiledProcedure> compileResults;

	// Compile all the trees into individual procedures
	for (size_t i = 0; i < trees.size(); i++) {

		// Get the node type
		AA_AST_NODE_TYPE tp = trees[i]->GetRoot()->type;

		// Escape clause (For stuff we dont compile)
		if (tp == AA_AST_NODE_TYPE::usingspecificstatement || tp == AA_AST_NODE_TYPE::usingstatement) {
			continue;
		}

		// Compile the procedures and add to result
		compileResults.Add(this->CompileProcedureFromASTRootNode(trees[i]->GetRoot(), senv));

	}

	// Compile all procedures into bytecode
	result.result = CompileFromProcedures(compileResults, senv, 0);

	// Write operations out in a readable format
	if (m_outfile != L"") {
		aa::dump_instructions(m_outfile, compileResults.Vector(), m_pAAVM);
	}

	// Set success flag to true
	result.success = true;

	// Aslo return bytecode (so we can execute it directly)
	return result;

}

AAC::CompiledProcedure AAC::CompileProcedureFromASTNode(AA_AST_NODE* pASTNode, AAStaticEnvironment staticData) {

	// Procedure for the abstract tree
	CompiledProcedure proc;
	proc.node = pASTNode;

	// Compile the execution stack
	proc.procOperations = CompileAST(pASTNode, proc.procEnvironment, staticData);

	// Return the procedure
	return proc;

}

aa::list<AAC::CompiledProcedure> AAC::CompileProcedureFromASTRootNode(AA_AST_NODE* pAstRootNode, AAStaticEnvironment senv) {

	// Compile results
	aa::list<CompiledProcedure> compileResults;

	// Get the node type
	AA_AST_NODE_TYPE tp = pAstRootNode->type;

	// If it's a class, we'll have to compile it just a bit differently
	if (tp == AA_AST_NODE_TYPE::classdecl) {

		// Does the declaration contain a body to compile?
		if (pAstRootNode->expressions.size() >= AA_NODE_CLASSNODE_BODY) {

			// For all sub-elements of class decl
			for (size_t i = 0; i < pAstRootNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions.size(); i++) {

				// Make sure it's a function decleration
				if (pAstRootNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]->type == AA_AST_NODE_TYPE::fundecl) {

					// Compile the AST
					compileResults.Add(this->CompileProcedureFromASTNode(pAstRootNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i], senv));

				}

			}

		}

	} else if (tp == AA_AST_NODE_TYPE::name_space) {

		// For all subelements in namespace
		for (size_t i = 0; i < pAstRootNode->expressions.size(); i++) {

			// Compile code and add to results
			compileResults.Add(this->CompileProcedureFromASTRootNode(pAstRootNode->expressions[i], senv));

		}

	} else if (tp == AA_AST_NODE_TYPE::enumdecleration) {
	
		// For all subelements in enum body
		for (size_t i = 0; i < pAstRootNode->expressions[AA_NODE_ENUMNODE_BODY]->expressions.size(); i++) {

			// Compile code and add to results
			compileResults.Add(this->CompileProcedureFromASTRootNode(pAstRootNode->expressions[AA_NODE_ENUMNODE_BODY]->expressions[i], senv));

		}

	} else {

		// Generated bytecode
		compileResults.Add(CompileProcedureFromASTNode(pAstRootNode, senv));

	}

	// Return results
	return compileResults;

}

AAC_CompileErrorMessage AAC::RunStaticOperations(std::vector<AA_AST*>& trees, AAStaticEnvironment& staticChecks) {
	
	// Compile error container
	AAC_CompileErrorMessage err;

	// Run static analysis and check for errors
	if (COMPILE_ERROR((err = this->m_staticAnalyser->RunStaticAnalysis(trees)))) {
		return err;
	}

	// Set static environment
	staticChecks = this->m_staticAnalyser->GetResult();

	// Return result of static function check
	return NO_COMPILE_ERROR_MESSAGE;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileAST(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

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
	case AA_AST_NODE_TYPE::enumbody:
	case AA_AST_NODE_TYPE::funcbody:
	case AA_AST_NODE_TYPE::block: {
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			executionStack.Add(this->CompileAST(pNode->expressions[i], cTable, staticData));
			if (i < pNode->expressions.size() - 1) {
				if (pNode->expressions[i]->type == AA_AST_NODE_TYPE::binop && pNode->expressions[i]->content != L"=") {
					executionStack.Add(CompiledAbstractExpression(AAByteCode::POP, 0, 0));
				} else if (pNode->expressions[i]->type == AA_AST_NODE_TYPE::funcall && pNode->expressions[i]->HasTag("returns")) {
					if (pNode->expressions[i]->tags["returns"] != 0) { // Calling a function that returns something (BUT, we're only interested in side-effects, thus, we also can't simplify this away)
						executionStack.Add(CompiledAbstractExpression(AAByteCode::POP, 0, 0));
					}
				}
			}
		}
		break;
	}
	case AA_AST_NODE_TYPE::fundecl: {
		if (pNode->expressions.size() >= AA_NODE_FUNNODE_BODY) { // We've got the actual function body => compile it
			aa::list<CompiledAbstractExpression> args = this->CompileFuncArgs(pNode, cTable, staticData);
			aa::list<CompiledAbstractExpression> body = aa::list<CompiledAbstractExpression>::Merge(args, this->CompileAST(pNode->expressions[AA_NODE_FUNNODE_BODY], cTable, staticData));
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
		executionStack.Add(CompileFunctionCall(pNode, cTable, staticData));
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
	case AA_AST_NODE_TYPE::enumvalueaccess: {
		executionStack.Add(this->CompileEnumAccessorOperation(pNode, cTable, staticData));
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
	case AA_AST_NODE_TYPE::enumidentifier:
	{
		executionStack.Add(HandleStackPush(cTable, pNode, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::index: {
		executionStack.Add(HandleIndexPush(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::matchstatement: {
		executionStack.Add(this->CompilePatternBlock(pNode, cTable, staticData));
		break;
	}
	default:
		break;
	}

	return executionStack;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileBinaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

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

			binopCAE.bc = (pNode->tags["operatorIsVM"]) ? AAByteCode::XCALL : AAByteCode::CALL;
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

aa::list<AAC::CompiledAbstractExpression> AAC::CompileUnaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	aa::list<CompiledAbstractExpression> opList;

	CompiledAbstractExpression unopCAE;
	unopCAE.argCount = 0;
	unopCAE.bc = GetBytecodeFromUnaryOperator(pNode->content);

	opList.Add(HandleStackPush(cTable, pNode->expressions[0], staticData));
	opList.Add(unopCAE);

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileAccessorOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	aa::list<CompiledAbstractExpression> opList;

	opList.Add(this->CompileAST(pNode->expressions[0], cTable, staticData)); // CALL ACCESS NOT HANDLEs => Implement that
	opList.Add(this->CompileAST(pNode->expressions[1], cTable, staticData));

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileEnumAccessorOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Operation opcode list
	aa::list<CompiledAbstractExpression> opList;

	// Is it a enumvalueaccess? (This is checked beforehand, this function might be used for more later)
	if (pNode->type == AA_AST_NODE_TYPE::enumvalueaccess) {

		// Create literal
		AA_Literal lit;
		lit.tp = AALiteralType::Int;
		lit.lit.i.val = pNode->expressions[1]->tags["enumval"];

		// Add the const push
		opList.Add(this->HandleConstPush(cTable, lit));

	}

	// Return list
	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileFunctionCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	aa::list<CompiledAbstractExpression> opList;

	int procID = -1;
	if (pNode->HasTag("calls")) {
		procID = pNode->tags["calls"];
	}

	int args = 0;
	if (pNode->HasTag("args")) {
		args = pNode->tags["args"];
	}

	bool isVmCll = false;
	if (pNode->HasTag("isVM")) {
		isVmCll = pNode->tags["isVM"];
	}

	if (args > 0) {
		for (AA_AST_NODE* pArg : pNode->expressions) {
			opList.Add(this->HandleStackPush(cTable, pArg, staticData));
		}
	}

	CompiledAbstractExpression callCAE;
	callCAE.argCount = 2;
	callCAE.bc = (isVmCll) ? AAByteCode::XCALL : AAByteCode::CALL;
	callCAE.argValues[0] = procID;
	callCAE.argValues[1] = args;

	opList.Add(callCAE);

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileFuncArgs(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Get the argument list
	AA_AST_NODE* pArgList = pNode->expressions[AA_NODE_FUNNODE_ARGLIST];

	aa::list<CompiledAbstractExpression> opList;

	for (size_t i = 0; i < pArgList->expressions.size(); i++) {

		// Get the argument
		std::wstring arg = pArgList->expressions[pArgList->expressions.size() - 1 - i]->content;

		// Add identifier to constants' identifier table
		cTable.identifiers.Add(arg);

		// Create setvar operation
		CompiledAbstractExpression argCAE;
		argCAE.bc = AAByteCode::SETVAR;
		argCAE.argCount = 1;

		// Set the argument of size() - 1 - i (the inverse) becausue of the push-pop mechanism used by the AAVM to pass call arguments
		argCAE.argValues[0] = cTable.identifiers.IndexOf(arg);

		// Add operation
		opList.Add(argCAE);

	}

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::CompileConditionalBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

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

aa::list<AAC::CompiledAbstractExpression> AAC::CompileForBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

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

aa::list<AAC::CompiledAbstractExpression> AAC::CompileWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

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

aa::list<AAC::CompiledAbstractExpression> AAC::CompileDoWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

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

aa::list<AAC::CompiledAbstractExpression> AAC::CompileNewStatement(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

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

aa::list<AAC::CompiledAbstractExpression> AAC::CompilePatternBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Operations list
	aa::list<CompiledAbstractExpression> opList;

	// Compile LHS
	aa::list<CompiledAbstractExpression> matchon = this->CompileAST(pNode->expressions[0], cTable, staticData);

	// Get the list of cases
	std::vector<AA_AST_NODE*> cases = pNode->expressions[1]->expressions;

	// Keep track of conditions
	aa::list<aa::list<CompiledAbstractExpression>> conditions;

	// Keep track of bodies
	aa::list<aa::list<CompiledAbstractExpression>> bodies;

	// For each case in match list
	for (size_t i = 0; i < cases.size(); i++) {

		aa::list<CompiledAbstractExpression> condition;
		condition.Add(matchon);
		condition.Add(this->CompileAST(cases[i]->expressions[0]->expressions[0], cTable, staticData)); // Will throw error later! (TODO: Implement a condition compiler)

		CompiledAbstractExpression eq;
		eq.bc = AAByteCode::CMPE;
		eq.argCount = 0;
		condition.Add(eq);

		CompiledAbstractExpression jmpiftrue;
		jmpiftrue.bc = AAByteCode::JMPT;
		jmpiftrue.argCount = 1;
		jmpiftrue.argValues[0] = -2;
		condition.Add(jmpiftrue);
		conditions.Add(condition);

		aa::list<CompiledAbstractExpression> body = this->CompileAST(cases[i]->expressions[1], cTable, staticData);
		CompiledAbstractExpression jmptoend;
		jmptoend.bc = AAByteCode::JMP;
		jmptoend.argCount = 1;
		jmptoend.argValues[0] = -3;
		body.Add(jmptoend);
		bodies.Add(body);

	}

	// Join it all into one long list
	conditions.ForEach([&opList](auto& cl) { opList.Add(cl); });

	// Add in the bodies one for one
	for (size_t i = 0; i < bodies.Size(); i++) {

		// position to jump to
		size_t jmpt = opList.Size();

		// Find first jmp and correct it (should correspond to the body we're now adding)
		for (size_t j = 0; j < opList.Size(); j++) {
			if (opList.At(j).bc == AAByteCode::JMPT && opList.At(j).argValues[0] == -2) {
				opList.At(j).argValues[0] = (jmpt - j) - 1;
				break;
			}
		}

		// Add the body
		opList.Add(bodies.At(i));

	}

	// Correct jmps in body
	for (size_t i = 0; i < opList.Size(); i++) {

		// Found a jump we should fix?
		if (opList.At(i).bc == AAByteCode::JMP && opList.At(i).argValues[0] == -3) {

			// Fix it
			opList.At(i).argValues[0] = (opList.Size() - i) - 1;

		}

	}

	// Return the list of operations
	return opList;

}

#pragma region Helper Functions

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

bool AAC::IsEnumeration(AA_AST_NODE_TYPE type) {
	return type == AA_AST_NODE_TYPE::enumidentifier;
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

AAC::CompiledAbstractExpression AAC::HandleConstPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode) {

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

	// Return const push
	return this->HandleConstPush(cTable, lit);

}

AAC::CompiledAbstractExpression AAC::HandleConstPush(CompiledEnviornmentTable& cTable, AA_Literal lit) {

	// TODO: Update this to keep track of const counter (so two cases of 5's is added to the const table)
	//		 If a single const is used, we don't want to waste time looking it up

	int i = -1;
	if (cTable.constValues.Contains(lit)) {
		i = cTable.constValues.IndexOf(lit);
	} else {
		i = cTable.constValues.Size();
		cTable.constValues.Add(lit);
	}

	CompiledAbstractExpression pushOp;
	pushOp.bc = AAByteCode::PUSHC;
	pushOp.argCount = 1;
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
		printf("Using a variable before it's declared!\n");
		return pushCAE;
	}

}

AAC::CompiledAbstractExpression AAC::HandleFieldPush(AA_AST_NODE* pNode, AAStaticEnvironment staticData) {

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

aa::list<AAC::CompiledAbstractExpression> AAC::HandleStackPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode, AAStaticEnvironment staticData) {

	aa::list<CompiledAbstractExpression> opList;

	if (IsConstant(pNode->type)) {
		opList.Add(this->HandleConstPush(cTable, pNode));
	} else if (IsVariable(pNode->type)) {
		opList.Add(this->HandleVarPush(cTable, pNode));
	} else if (IsEnumeration(pNode->type)) {
		if (pNode->content.compare(L"_") == 0) {
			AA_Literal lit;
			lit.tp = AALiteralType::Boolean;
			lit.lit.b = true;
			opList.Add(this->HandleConstPush(cTable, lit));
		} else if (pNode->HasTag("enumval")) {
			AA_Literal lit;
			lit.tp = AALiteralType::Int;
			lit.lit.i.val = pNode->tags["enumval"];
			opList.Add(this->HandleConstPush(cTable, lit));
		}
	} else {
		opList.Add(CompileAST(pNode, cTable, staticData));
	}

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::HandleCtorCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& ctable, AAStaticEnvironment staticData) {

	// List containing the operations generated
	aa::list<CompiledAbstractExpression> opList;

	int procID = -1;
	if (pNode->HasTag("calls")) {
		procID = pNode->tags["calls"];
	}

	int args = 0;
	if (pNode->HasTag("args")) {
		args = pNode->tags["args"];
	}

	bool isVmCll = false;
	if (pNode->HasTag("isVM")) {
		isVmCll = pNode->tags["isVM"];
	}

	size_t allocSize = 0;
	if (pNode->HasTag("allocsz")) {
		allocSize = (size_t)pNode->tags["allocsz"]; // Hopefully, a temporary solution
	}

	CompiledAbstractExpression newCAE;
	newCAE.argCount = 1;
	newCAE.argValues[0] = allocSize;
	newCAE.bc = AAByteCode::HALLOC;

	opList.Add(newCAE);

	for (size_t i = 0; i < pNode->expressions.size(); i++) {
		opList.Add(this->CompileAST(pNode->expressions[i], ctable, staticData));
	}

	CompiledAbstractExpression callCAE;
	callCAE.bc = (isVmCll) ? AAByteCode::XCALL : AAByteCode::CALL;
	callCAE.argCount = 2;
	callCAE.argValues[0] = procID;
	callCAE.argValues[1] = args;

	opList.Add(callCAE);

	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::HandleMemberCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// The op list to get from member call
	aa::list<CompiledAbstractExpression> opList;

	// Create a temporary node
	AA_AST_NODE* tempNode = new AA_AST_NODE(pNode->expressions[1]->content, AA_AST_NODE_TYPE::funcall, pNode->expressions[0]->position);
	tempNode->expressions = pNode->expressions[1]->expressions;
	tempNode->expressions.insert(tempNode->expressions.begin(), pNode->expressions[0]);
	tempNode->tags = pNode->expressions[1]->tags;

	// Call member
	opList = this->CompileFunctionCall(tempNode, cTable, staticData);

	// Delete the temporary node again
	delete tempNode;

	// Return the op list
	return opList;

}

aa::list<AAC::CompiledAbstractExpression> AAC::HandleIndexPush(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

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

#pragma endregion

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

AAC_Out AAC::CompileFromProcedures(aa::list<CompiledProcedure> procedures, AAStaticEnvironment staticCompileData, int entryPoint) {

	// Bytecode compilation result
	AAC_Out compileBytecodeResult;

	// Byte stream
	aa::bstream bis;

	// Write procedure count
	bis << (int)procedures.Size();

	// Write entry point (Pointer to the first operation to execute)
	bis << entryPoint;

	// For all procedures
	for (size_t p = 0; p < procedures.Size(); p++) {

		// Convert constants table to bytecode
		ConstTableToByteCode(procedures.At(p).procEnvironment, bis);

		// Write amount of operations
		bis << (int)procedures.At(p).procOperations.Size();

		// Write all expressions in their compiled formats
		for (size_t i = 0; i < procedures.At(p).procOperations.Size(); i++) {
			this->ConvertToBytes(procedures.At(p).procOperations.At(i), bis);
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

#pragma region Stack Verifier (Not used)

int AAC::CalcStackSzAfterOperation(AAC::CompiledAbstractExpression op, AAStaticEnvironment staticData) {
	switch (op.bc) {
	case AAByteCode::PUSHC:
	case AAByteCode::PUSHV:
	case AAByteCode::GETVAR:
	case AAByteCode::HALLOC:
	case AAByteCode::SALLOC:
		return 1; // Increases stack size by one
	case AAByteCode::ADD:
	case AAByteCode::SUB:
	case AAByteCode::MUL:
	case AAByteCode::MOD:
	case AAByteCode::SETVAR:
	case AAByteCode::LEQ:
	case AAByteCode::GEQ:
	case AAByteCode::LE:
	case AAByteCode::GE:
	case AAByteCode::JMPF:
	case AAByteCode::JMPT:
		return -1; // Takes the two elements on top and operates on them, putting the resulting back unto the stack
	case AAByteCode::SETFIELD:
		return -2;
	case AAByteCode::CALL: {
		int callc = (staticData.availableFunctions.FindFirst([op](AAFuncSignature*& sig) { return sig->isVMFunc == false && sig->procID == op.argValues[0]; })->returnType == AACType::Void) ? 0 : 1;
		printf("%i", callc);
		return -op.argValues[1] + callc;
	}
	case AAByteCode::XCALL:
		return -op.argValues[1] + (m_preregisteredFunctions[op.argValues[0]]->returnType == AACType::Void) ? 0 : 1;
	case AAByteCode::JMP:
	case AAByteCode::GETFIELD:
		return 0;
	default: // seperated for debugging purposes (eg. detecting non-checked operations)
		return 0;
	}
}

// Obsolete, please let the static analyser do this!!!!
bool AAC::VerifyFunctionCallstack(aa::list<CompiledAbstractExpression> body, int expected, int args, AAStaticEnvironment staticData) {

	// Stack count
	int stacksz = 0;

	// Run through all operations
	body.ForEach([&stacksz, this, staticData](AAC::CompiledAbstractExpression& c) { stacksz += this->CalcStackSzAfterOperation(c, staticData); });

	// Return true iff stacksz is count
	if (stacksz == expected - args) { // The expected amount (minus args, because setvar subtracts from stack)
		return true;
	} else {
		printf("%i", stacksz);
		return false;
	}

}

#pragma endregion

#pragma region Preregistering

void AAC::AddVMClass(AAClassSignature* cc) {
	m_preregisteredClasses.push_back(cc);
}

void AAC::AddVMFunction(AAFuncSignature* sig) {
	m_preregisteredFunctions.push_back(sig);
}

void AAC::AddVMNamespace(AACNamespace* dom) {
	m_preregisteredNamespaces.push_back(dom);
}

#pragma endregion
