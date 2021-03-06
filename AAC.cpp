#include "AAC.h"
#include "AAB2F.h"
#include "AAVal.h"
#include <stack>
#include "AAVM.h"
#include "AAUnparser.h"

typedef CompiledAbstractExpression Instruction;
typedef aa::list<Instruction> Instructions;

AAC::AAC(AAVM* pVM) {

	// Set VM
	m_pAAVM = pVM;

	// Set support classes to null
	m_classCompiler = 0;
	m_staticAnalyser = 0;

	// Set current procID to 0
	m_currentProcID = 0;

	// The found entry point
	m_foundEntryPoint = 0;

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

	// Reset found entry point
	m_foundEntryPoint = 0;

	// Reset registered types and other static configurations
	m_staticAnalyser->Reset(m_preregisteredFunctions, m_preregisteredClasses, m_preregisteredNamespaces);

	// Reset registered compile types
	m_byteTypes.Clear();

}

AAC_CompileResult AAC::CompileFromAbstractSyntaxTrees(aa::list<AA_AST*> trees) {

	// Compile Result container
	AAC_CompileResult result;
	AAC_CompileErrorMessage err;
	AAStaticEnvironment senv;

	// Reset the internals of the compiler
	this->ResetCompilerInternals();

	// Expand ASTs
	for (size_t i = 0; i < trees.Size(); i++) {
		
		// Expand the tree
		trees.At(i)->Expand();

	}

	// Run the static checkers
	if (COMPILE_ERROR(err = this->RunStaticOperations(trees, senv))) {
		result.firstMsg = err;
		result.success = false;
		return result;
	}

	// Compile types
	m_byteTypes = this->CompileTypedata(senv);

	// The unparser
	AAUnparser unparser;
	unparser.Open(m_unparsefile);

	// Compile all the trees into individual procedures
	for (size_t i = 0; i < trees.Size(); i++) {

		// Unparse
		unparser.Unparse(trees.At(i));

		// Get the node type
		AA_AST_NODE_TYPE tp = trees.At(i)->GetRoot()->type;

		// Escape clause (For stuff we dont compile)
		if (tp == AA_AST_NODE_TYPE::usingspecificstatement || tp == AA_AST_NODE_TYPE::usingstatement) {
			continue;
		}

		// Compile the procedures and add to result
		result.compileResults.Add(this->CompileProcedureFromASTRootNode(trees.At(i)->GetRoot(), senv));

	}

	// Close the unparser
	unparser.Close();

	// Compile all procedures into bytecode
	result.result = CompileFromProcedures(result.compileResults, senv, m_foundEntryPoint);

	// Write operations out in a readable format
	if (m_outfile != L"") {
		aa::dump_instructions(m_outfile, result.compileResults.Vector(), m_byteTypes.Vector(), m_pAAVM);
	}

	// Set success flag to true
	result.success = true;

	// Reset override entry point
	m_overrideEntryPoint = L"";

	// Aslo return bytecode (so we can execute it directly)
	return result;

}

CompiledProcedure AAC::CompileProcedureFromASTNode(AA_AST_NODE* pASTNode, AAStaticEnvironment staticData) {

	// Procedure for the abstract tree
	CompiledProcedure proc;
	proc.name = pASTNode->content;

	// Compile the execution stack
	proc.procOperations = CompileAST(pASTNode, proc.procEnvironment, staticData);

	// Return the procedure
	return proc;

}

aa::list<CompiledProcedure> AAC::CompileProcedureFromASTRootNode(AA_AST_NODE* pAstRootNode, AAStaticEnvironment senv) {

	// Compile results
	aa::list<CompiledProcedure> compileResults;

	// Get the node type
	AA_AST_NODE_TYPE tp = pAstRootNode->type;

	if (tp == AA_AST_NODE_TYPE::compile_unit) {

		// Loop through all the contents of the compile unit
		for (size_t i = 0; i < pAstRootNode->expressions.size(); i++) {

			// Add compile result
			compileResults.Add(this->CompileProcedureFromASTRootNode(pAstRootNode->expressions[i], senv));

		}

	} else if (tp == AA_AST_NODE_TYPE::classdecl) { // If it's a class, we'll have to compile it just a bit differently

		// Does the declaration contain a body to compile?
		if (AA_NODE_CLASSNODE_BODY < pAstRootNode->expressions.size()) {

			// Get compile sources
			auto compileSource = pAstRootNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions;

			// For all sub-elements of class decl
			for (size_t i = 0; i < compileSource.size(); i++) {

				// Make sure it's a function decleration
				if (compileSource[i]->type == AA_AST_NODE_TYPE::fundecl && !compileSource[i]->HasTag("do_not_compile_as_procedure")) {

					// Compile the AST
					compileResults.Add(this->CompileProcedureFromASTNode(compileSource[i], senv));

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

		// Dont compile using statements
		if (tp != AA_AST_NODE_TYPE::usingspecificstatement && tp != AA_AST_NODE_TYPE::usingstatement) {

			// Generated bytecode
			compileResults.Add(CompileProcedureFromASTNode(pAstRootNode, senv));

		}

	}

	// Return results
	return compileResults;

}

AAC_CompileErrorMessage AAC::RunStaticOperations(aa::list<AA_AST*>& trees, AAStaticEnvironment& staticChecks) {
	
	// Compile error container
	AAC_CompileErrorMessage err;

	// Run static analysis and check for errors
	if (COMPILE_ERROR((err = this->m_staticAnalyser->RunStaticAnalysis(trees.Vector())))) {
		return err;
	}

	// Set static environment
	staticChecks = this->m_staticAnalyser->GetResult();

	// Get the entry point
	if (!this->m_staticAnalyser->FindEntryPoint(trees.Vector(), staticChecks, m_overrideEntryPoint, m_foundEntryPoint)) {

		// Setup error
		err.errorMsg = "Failed to find an entry point";
		err.errorSource = AACodePosition::Undetermined;
		err.errorType = 1;

		// Return error
		return err;

	}

	// Return result of static function check
	return NO_COMPILE_ERROR_MESSAGE;

}

Instructions AAC::CompileAST(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Instruction list
	Instructions executionStack;

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::binop: {
		executionStack.Add(CompileBinaryOperation(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::unop_pre:
	case AA_AST_NODE_TYPE::unop_post: {
		executionStack.Add(CompileUnaryOperation(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::enumbody:
	case AA_AST_NODE_TYPE::funcbody:
	case AA_AST_NODE_TYPE::lambdabody:
	case AA_AST_NODE_TYPE::block: {
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			executionStack.Add(this->CompileAST(pNode->expressions[i], cTable, staticData));
			if (i < pNode->expressions.size() - 1) {
				if (pNode->expressions[i]->type == AA_AST_NODE_TYPE::binop && pNode->expressions[i]->content != L"=") {
					//executionStack.Add(CompiledAbstractExpression(AAByteCode::POP, 0, 0));
				} else if (pNode->expressions[i]->type == AA_AST_NODE_TYPE::funcall && pNode->expressions[i]->HasTag("returns")) {
					if (pNode->expressions[i]->tags["returns"] != 0) { // Calling a function that returns something (BUT, we're only interested in side-effects, thus, we also can't simplify this away)
						//executionStack.Add(CompiledAbstractExpression(AAByteCode::POP, 0, 0));
					}
				}
			}
		}
		break;
	}
	case AA_AST_NODE_TYPE::fundecl: {
		if (aa::parsing::Function_HasBody(pNode)) { // We've got the actual function body => compile it
			aa::list<CompiledAbstractExpression> args = this->CompileFuncArgs(pNode, cTable, staticData);
			aa::list<CompiledAbstractExpression> body = aa::list<CompiledAbstractExpression>::Merge(args, this->CompileAST(pNode->expressions[AA_NODE_FUNNODE_BODY], cTable, staticData));
			CompiledAbstractExpression retCAE;
			retCAE.bc = AAByteCode::RET;
			retCAE.argCount = 0;
			//retCAE.argCount = 1;
			//retCAE.argValues[0] = pNode->tags["returncount"];
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
	case AA_AST_NODE_TYPE::index: {
		executionStack.Add(HandleIndexPush(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::matchstatement: {
		executionStack.Add(this->CompilePatternBlock(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::tupleaccess: {
		executionStack.Add(this->CompileTupleAccessorOperation(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::lambdacall: {
		executionStack.Add(this->CompileLambdaCall(pNode, cTable, staticData));
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
	default:
		break;
	}

	return executionStack;

}

Instructions AAC::CompileLambda(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	Instructions opList;

	Instructions body = this->CompileAST(pNode->expressions[1], cTable, staticData);
	body.Add(Instruction(AAByteCode::RET, 0, NULL));

	return opList;

}

Instructions AAC::CompileBinaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	Instructions opList;

	Instruction binInstruction;
	binInstruction.argCount = 0;
	binInstruction.bc = GetBytecodeFromBinaryOperator(pNode->content, pNode->expressions[0]->type);

	if (binInstruction.bc == AAByteCode::SETVAR) {
		
		binInstruction.argCount = 2;
		binInstruction.argValues[0] = HandleDecl(cTable, pNode->expressions[0]);
		binInstruction.argValues[1] = pNode->tags["primitive"];
		opList.Add(HandleStackPush(cTable, pNode->expressions[1], staticData));

	} else if (binInstruction.bc == AAByteCode::SETFIELD) {
	
		opList.Add(HandleVarPush(cTable, pNode->expressions[0]->expressions[0]));
		opList.Add(HandleStackPush(cTable, pNode->expressions[1], staticData));

		binInstruction.argCount = 2;
		binInstruction.argValues[0] = pNode->expressions[0]->expressions[1]->tags["fieldid"];
		binInstruction.argValues[1] = pNode->expressions[0]->expressions[1]->tags["primitive"];

	} else if (binInstruction.bc == AAByteCode::SETELEM) {
	
		opList.Add(HandleStackPush(cTable, pNode->expressions[1], staticData));
		opList.Add(HandleStackPush(cTable, pNode->expressions[0]->expressions[1], staticData));
		opList.Add(HandleVarPush(cTable, pNode->expressions[0]->expressions[0]));
		
		binInstruction.argCount = 1;
		binInstruction.argValues[0] = 0; // TODO: Add support for multi-dimensions

	} else {

		if (binInstruction.bc == AAByteCode::LAND || binInstruction.bc == AAByteCode::LOR) {

			Instructions lhs = HandleStackPush(cTable, pNode->expressions[0], staticData);
			Instructions rhs = HandleStackPush(cTable, pNode->expressions[1], staticData);

			opList.Add(lhs);

			binInstruction.argCount = 1;
			binInstruction.argValues[0] = rhs.Size() + 2; // Skip rest of condition and go directly to end of statement OR jmpf

			opList.Add(binInstruction);

			opList.Add(rhs);

			binInstruction.bc = (binInstruction.bc == AAByteCode::LAND) ? AAByteCode::BINAND : AAByteCode::BINOR;
			binInstruction.argCount = 1;
			binInstruction.argValues[0] = (int)AAPrimitiveType::boolean;

			opList.Add(binInstruction);

			// Pop if told to
			opList.Add(HandlePop(pNode));

			// Return the list of commands now
			return opList;

		} else {


			if (pNode->tags["useCall"]) {

				binInstruction.bc = (pNode->tags["operatorIsVM"]) ? AAByteCode::XCALL : AAByteCode::CALL;
				binInstruction.argCount = 2;
				binInstruction.argValues[0] = pNode->tags["operatorProcID"];
				binInstruction.argValues[1] = 2;

			} else {

				if (pNode->tags["primitive"] == (int)AAPrimitiveType::string) {
					binInstruction.bc = AAByteCode::CONCAT;
				} else {
					binInstruction.argCount = 1;
					binInstruction.argValues[0] = pNode->tags["primitive"];
				}

			}

			opList.Add(HandleStackPush(cTable, pNode->expressions[0], staticData));
			opList.Add(HandleStackPush(cTable, pNode->expressions[1], staticData));

		}

	}

	// Add final instruction
	opList.Add(binInstruction);

	// Pop if told to
	opList.Add(HandlePop(pNode));

	// Return final list
	return opList;

}

Instructions AAC::CompileUnaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	Instructions opList;

	CompiledAbstractExpression unopCAE;
	unopCAE.bc = GetBytecodeFromUnaryOperator(pNode->content);

	if (unopCAE.bc == AAByteCode::INC || unopCAE.bc == AAByteCode::DEC) {

		unopCAE.argCount = 2;
		unopCAE.argValues[0] = pNode->expressions[0]->tags["varsi"];
		unopCAE.argValues[1] = (pNode->type == AA_AST_NODE_TYPE::unop_post) ? 1 : 0;

	} else {

		unopCAE.argCount = 1;
		unopCAE.argValues[0] = pNode->tags["primitive"];

		opList.Add(HandleStackPush(cTable, pNode->expressions[0], staticData));

	}

	opList.Add(unopCAE);

	return opList;

}

Instructions AAC::CompileAccessorOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	Instructions opList;

	opList.Add(this->CompileAST(pNode->expressions[0], cTable, staticData)); // CALL ACCESS NOT HANDLEs => Implement that
	opList.Add(this->CompileAST(pNode->expressions[1], cTable, staticData));

	return opList;

}

Instructions AAC::CompileEnumAccessorOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Operation opcode list
	Instructions opList;

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

Instructions AAC::CompileTupleAccessorOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Operation opcode list
	Instructions opList;

	// Put LHS unto stack
	opList.Add(this->CompileAST(pNode->expressions[0], cTable, staticData));

	// The tuple value fetch instruction
	Instruction tupleAccess;
	tupleAccess.bc = AAByteCode::TUPLEGET;
	tupleAccess.argCount = 1;
	tupleAccess.argValues[0] = pNode->tags["tupleindex"];

	// Add the acces operation to oplist
	opList.Add(tupleAccess);

	// Return instructions
	return opList;

}

Instructions AAC::CompileFunctionCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	Instructions opList;

	int procID = -1;
	if (pNode->HasTag("calls")) {
		procID = pNode->tags["calls"];
	}

	int args = 0;
	if (pNode->HasTag("args")) {
		args = pNode->tags["args"];
	}

	bool isVmCll = (bool)pNode->tags["isVM"];
	bool isVirtualCll = pNode->HasTag("isVirtualCall");

	if (args > 0) {
		for (AA_AST_NODE* pArg : pNode->expressions) {
			
			aa::list<CompiledAbstractExpression> ops = this->HandleStackPush(cTable, pArg, staticData);
			opList.Add(ops);

		}
	}

	AAByteCode callMode = AAByteCode::CALL;
	if (isVmCll) {
		callMode = AAByteCode::XCALL;
	} else if (isVirtualCll) {
		callMode = AAByteCode::VCALL; // TODO: Add a fetch vtable call or something here, because at the moment arguments are NOT supported (VM calls also are not supported)
	}

	CompiledAbstractExpression callCAE;
	callCAE.argCount = 2;
	callCAE.bc = callMode;
	callCAE.argValues[0] = procID;
	callCAE.argValues[1] = args;

	opList.Add(callCAE);

	if (pNode->HasTag("pop_size")) {

		CompiledAbstractExpression popCAE;
		popCAE.argCount = 1;
		popCAE.bc = AAByteCode::POP;
		popCAE.argValues[0] = pNode->tags["pop_size"];

		opList.Add(popCAE);

	}

	return opList;

}

Instructions AAC::CompileFuncArgs(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Get the argument list
	AA_AST_NODE* pArgList = pNode->expressions[AA_NODE_FUNNODE_ARGLIST];

	Instructions opList;

	for (size_t i = 0; i < pArgList->expressions.size(); i++) {

		// Get the argument
		std::wstring arg = pArgList->expressions[pArgList->expressions.size() - 1 - i]->content;

		// Add identifier to constants' identifier table
		cTable.identifiers.Add(arg);

		// Create setvar operation
		CompiledAbstractExpression argCAE;
		argCAE.bc = AAByteCode::SETVAR;
		argCAE.argCount = 2;

		// Set the argument of size() - 1 - i (the inverse) becausue of the push-pop mechanism used by the AAVM to pass call arguments
		argCAE.argValues[0] = cTable.identifiers.IndexOf(arg);
		argCAE.argValues[1] = pArgList->expressions[pArgList->expressions.size() - 1 - i]->tags["primitive"];

		// Add operation
		opList.Add(argCAE);

	}

	return opList;

}

Instructions AAC::CompileLambdaCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	Instructions opList;

	return opList;

}

Instructions AAC::CompileCondition(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Instruction list
	Instructions opList;

	// Run through all conditions
	for (size_t i = 0; i < pNode->expressions.size(); i++) {
		opList.Add(this->CompileAST(pNode->expressions[i], cTable, staticData));
	}

	// Return options
	return opList;

}

Instructions AAC::CompileConditionalBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Operations list
	Instructions opList;

	// Compile condition and body
	Instructions condition = this->CompileCondition(pNode->expressions[0], cTable, staticData);
	Instructions body = this->CompileAST(pNode->expressions[1], cTable, staticData);

	// Jump if condition false
	Instruction jmpF = Instruction(AAByteCode::JMPF, 1, NULL);
	jmpF.argValues[0] = 1 + (int)body.Size();

	// Only add jump instruction if there's an else or else if body to skip
	if (pNode->expressions.size() >= 3) {
		body.Add(Instruction(AAByteCode::JMP, 0, NULL)); // Add jump instruction to end
		jmpF.argValues[0]++; // Consider the jmp instruction added
	}

	// Add condition and body
	opList.Add(condition);
	opList.Add(jmpF);
	opList.Add(body);

	// Run through all the other cases
	for (size_t i = 2; i < pNode->expressions.size(); i++) {
		if (pNode->expressions[i]->expressions.size() == 1) { // else case
			opList.Add(this->CompileAST(pNode->expressions[i]->expressions[0], cTable, staticData));
		} else { // else if case

			// Compile condition and body
			Instructions elifcondition = this->CompileCondition(pNode->expressions[i]->expressions[0], cTable, staticData);
			Instructions elifbody = this->CompileAST(pNode->expressions[i]->expressions[1], cTable, staticData);

			// Jump if condition false
			Instruction elifjmpF = Instruction(AAByteCode::JMPF, 1, NULL);
			elifjmpF.argValues[0] = (int)elifbody.Size() + 1;

			// Add invalid jump instruction (fixed later)
			elifbody.Add(Instruction(AAByteCode::JMP, 0, NULL)); // Add jump instruction to end

			// Add all to oplist
			opList.Add(elifcondition);
			opList.Add(elifjmpF);
			opList.Add(elifbody);

		}
	}

	// Correct jump instructions
	const size_t totalInstructions = opList.Size();
	for (size_t i = 0; i < totalInstructions; i++) {
		if (opList.At(i).bc == AAByteCode::JMP && opList.At(i).argCount == 0) {
			opList.At(i).argCount = 1;
			opList.At(i).argValues[0] = totalInstructions - i - 1;
		}
	}

	// return oplist
	return opList;

}

Instructions AAC::CompileForBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Operations list
	Instructions opList;

	// Compile individual
	Instructions init = CompileAST(pNode->expressions[0]->expressions[0], cTable, staticData);
	Instructions condition = CompileAST(pNode->expressions[1]->expressions[0], cTable, staticData);
	Instructions afterthought = CompileAST(pNode->expressions[2]->expressions[0], cTable, staticData);
	Instructions body = CompileAST(pNode->expressions[3], cTable, staticData);

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
	jmpDntLoopIns.argValues[0] = (int)(body.Size()) + 2;

	// Add jump instructions
	condition.Add(jmpDntLoopIns);
	body.Add(jmpBckIns);

	// Add the condition and body to operations list
	opList.Add(condition);
	opList.Add(body);

	// return oplist
	return opList;

}

Instructions AAC::CompileWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Operations list
	aa::list<CompiledAbstractExpression> opList;

	// Compile individual elements
	aa::list<CompiledAbstractExpression> condition = CompileAST(pNode->expressions[0]->expressions[0], cTable, staticData);
	aa::list<CompiledAbstractExpression> body = CompileAST(pNode->expressions[1], cTable, staticData);

	// The jump if false instruction that will skip the body if the condition no longer holds
	CompiledAbstractExpression jmpDntLoopIns;
	jmpDntLoopIns.bc = AAByteCode::JMPF;
	jmpDntLoopIns.argCount = 1;
	jmpDntLoopIns.argValues[0] = (int)(body.Size()) + 2;

	// Add jump instructions
	condition.Add(jmpDntLoopIns);

	// The jump instruction that jumps back to the top of the condition
	CompiledAbstractExpression jmpBckIns;
	jmpBckIns.bc = AAByteCode::JMP;
	jmpBckIns.argCount = 1;
	jmpBckIns.argValues[0] = -(int)(body.Size() + condition.Size()+1);

	// Push the jmp back
	body.Add(jmpBckIns);

	// Add operations to the op list
	opList.Add(condition);
	opList.Add(body);	

	// return oplist
	return opList;

}

Instructions AAC::CompileDoWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Operations list
	aa::list<CompiledAbstractExpression> opList;

	// Compile individual elements
	aa::list<CompiledAbstractExpression> condition = CompileAST(pNode->expressions[0]->expressions[0], cTable, staticData);
	aa::list<CompiledAbstractExpression> body = CompileAST(pNode->expressions[1], cTable, staticData);

	// The jump if false instruction that will skip the body if the condition no longer holds
	CompiledAbstractExpression jmpDntLoopIns;
	jmpDntLoopIns.bc = AAByteCode::JMPT;
	jmpDntLoopIns.argCount = 1;
	jmpDntLoopIns.argValues[0] = -(int)(body.Size() + condition.Size());

	// Add jump instructions
	condition.Add(jmpDntLoopIns);

	// Add operations to the op list
	opList.Add(body);
	opList.Add(condition);

	// return oplist
	return opList;

}

Instructions AAC::CompileNewStatement(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Operations list
	aa::list<CompiledAbstractExpression> opList;

	// Is it a constructor call?
	if (pNode->expressions[0]->type == AA_AST_NODE_TYPE::classctorcall) {
		
		// Let the specific function handle that
		opList.Add(this->HandleCtorCall(pNode->expressions[0], cTable, staticData));

	} else { // Else it's an array creation

		// Compile the array size
		aa::list<CompiledAbstractExpression> arraySizes = this->CompileAST(pNode->expressions[0]->expressions[1], cTable, staticData);
		opList.Add(arraySizes);

		// Create allocation instruction
		CompiledAbstractExpression allocCAE;
		allocCAE.bc = AAByteCode::ALLOCARRAY;
		allocCAE.argCount = 2;
		allocCAE.argValues[0] = pNode->tags["primitive"];
		allocCAE.argValues[1] = 1; // TODO: Add support for multi-dimensions

		// Add to oplist
		opList.Add(allocCAE);

	}

	// return oplist
	return opList;

}

Instructions AAC::CompilePatternBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Operations list
	Instructions opList;

	// Compile LHS
	Instructions matchon = this->CompileAST(pNode->expressions[0], cTable, staticData);

	// Get the list of cases
	std::vector<AA_AST_NODE*> cases = pNode->expressions[1]->expressions;

	// Keep track of conditions
	aa::list<Instructions> conditions;

	// Keep track of bodies
	aa::list<Instructions> bodies;

	// For each case in match list
	for (size_t i = 0; i < cases.size(); i++) {

		// Condition instruction list
		Instructions condition; 

		// Should we always match this branch?
		if (cases[i]->expressions[0]->HasTag("match_always")) {
			
			// Push true constant on top of stack ==> Because this branch will always be true
			condition.Add(this->HandleConstPush(cTable, true));

		} else if (cases[i]->HasTag("compareTupleWithVariables")) { // It's a comparison with variables

			// Compile the condition (We cannot compile it as usual)
			condition = this->CompilePatternCondition(matchon, cases[i]->tags["scope_to_now"], cases[i]->expressions[0]->expressions[0], cTable, staticData);

			// Prepare compare or set
			Instruction tcmpset;
			tcmpset.bc = AAByteCode::TUPLECMPORSET;
			tcmpset.argCount = (int)cases[i]->expressions[0]->expressions[0]->expressions.size() + 1;
			tcmpset.argValues[0] = tcmpset.argCount - 1;

			// Run through all the expressions
			for (size_t j = 0; j < cases[i]->expressions[0]->expressions[0]->expressions.size(); j++) {

				// Is not a constant?
				if (!this->IsConstant(cases[i]->expressions[0]->expressions[0]->expressions[j]->type)) {

					// Enter scope
					this->EnterScope(cTable, cases[i]->expressions[0]->expressions[0]->expressions[j]);

					// Add var ID
					tcmpset.argValues[j + 1] = cases[i]->expressions[0]->expressions[0]->expressions[j]->tags["varsi"];

				}

			}

			// Add last command
			condition.Add(tcmpset);

		} else { // We have to do a value check...

			// Compile the condition (We cannot compile it as usual)
			condition = this->CompilePatternCondition(matchon, cases[i]->tags["scope_to_now"], cases[i]->expressions[0]->expressions[0], cTable, staticData);

			// Is it a tuple comparrison?
			if (cases[i]->HasTag("compareTuples")) {

				// Add tuple comparrison opcode
				condition.Add(Instruction(AAByteCode::TUPLECMP, 0, NULL));

			} else if (!cases[i]->HasTag("compare_handled")) { // If we dont have this tag (the comparison is not handled in the 'CompilePatternCondition'
			
				// Simple comparrison instruction
				Instruction eq;
				eq.bc = AAByteCode::CMPE;
				eq.argCount = 1;
				eq.argValues[0] = cases[i]->expressions[0]->tags["primitive"];
				condition.Add(eq);

			}

		}

		Instruction jmpiftrue;
		jmpiftrue.bc = AAByteCode::JMPT;
		jmpiftrue.argCount = 1;
		jmpiftrue.argValues[0] = -2;
		condition.Add(jmpiftrue);
		conditions.Add(condition);

		Instructions body = this->CompileAST(cases[i]->expressions[1], cTable, staticData);
		
		// Add jump instruction if NOT the last case (because the last case can just continue
		if (i < cases.size() - 1) {

			Instruction jmptoend;
			jmptoend.bc = AAByteCode::JMP;
			jmptoend.argCount = 1;
			jmptoend.argValues[0] = -3;
			body.Add(jmptoend);

		}

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
				opList.At(j).argValues[0] = (jmpt - j) /*- 1*/;
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

Instructions AAC::CompilePatternCondition(aa::list<CompiledAbstractExpression> match, int scopeStart, AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// Operations list
	Instructions opList;

	if (pNode->type == AA_AST_NODE_TYPE::objdeconstruct) {

		opList.Add(match);
		opList.Add(Instruction(AAByteCode::EXTTAG, 0, NULL));

		Instructions tupleBuild;

		bool hasAssignments = false;
		int assignments[8];
		memset(assignments, -1, sizeof(assignments));

		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			if (pNode->expressions[i]->type == AA_AST_NODE_TYPE::variable) {
				if (pNode->expressions[i]->content.compare(L"_") == 0) {
					tupleBuild.Add(Instruction(AAByteCode::ACCEPT, 0, NULL));
				} else {
					this->EnterScope(cTable, pNode->expressions[i]);
					if (pNode->expressions[i]->tags["varsi"] >= scopeStart) { // Set var
						hasAssignments = true;
						assignments[i] = pNode->expressions[i]->tags["varsi"];
					} else {
						// TODO: Fix scoping.... this is bound to cause an error
						tupleBuild.Add(this->HandleVarPush(cTable, pNode->expressions[i]));
					}
				}
			} // else if constant literal 
			// else ...?

		}

		Instruction dconOp;
		
		if (!hasAssignments) {

			dconOp.bc = AAByteCode::TAGTUPLECMP;
			dconOp.argCount = 1;
			dconOp.argValues[0] = pNode->tags["matchType"];

			opList.Add(tupleBuild);

		} else {

			dconOp.bc = AAByteCode::TAGTUPLECMPORSET;
			dconOp.argValues[0] = (int)pNode->expressions.size();
			dconOp.argValues[1] = pNode->tags["matchType"];
			dconOp.argCount = dconOp.argValues[0]+2;
			memcpy(dconOp.argValues + 2, assignments, sizeof(assignments) - sizeof(int));

		}
		
		opList.Add(dconOp);

	} else if (pNode->type == AA_AST_NODE_TYPE::variable || pNode->type == AA_AST_NODE_TYPE::enumidentifier) {

		// Push the object to match with
		opList.Add(match);

		// Is it the dummy/accept value?
		if (pNode->content.compare(L"_") == 0) {

			// Push accepting state
			opList.Add(Instruction(AAByteCode::ACCEPT, 0, NULL));

		} else {

			// Push whatever this compiles to
			opList.Add(this->CompileAST(pNode, cTable, staticData));

		}

	} else if (pNode->type == AA_AST_NODE_TYPE::tupleval) {

		// Compile var check
		if (pNode->HasTag("has_vars")) {

			// Loop through all values in tuple
			for (size_t i = 0; i < pNode->expressions.size(); i++) {

				// If constant, push it
				if (this->IsConstant(pNode->expressions[pNode->expressions.size() - i - 1]->type)) {

					// Add const
					opList.Add(this->HandleConstPush(cTable, pNode->expressions[pNode->expressions.size() - i - 1]));

				}

			}

			// Push the object to match with (We have to push last - such that types can be read when needed)
			opList.Add(match);

		} else { // Simple tuple comparison

			// Push the object to match with
			opList.Add(match);

			// Push tuple
			opList.Add(this->HandleTuplePush(cTable, pNode, staticData));

		}

	}

	// Return the opList
	return opList;

}

#pragma region Helper Functions

void AAC::EnterScope(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode) {

	std::wstring scopedName = std::to_wstring(pNode->tags["varsi"]) + L"$[" + pNode->content + L"]";

	if (!cTable.identifiers.Contains(scopedName)) {
		cTable.identifiers.Add(scopedName);
	}

}

void AAC::ExitScope(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode) {

}

bool AAC::IsConstant(AA_AST_NODE_TYPE type) {
	return type == AA_AST_NODE_TYPE::intliteral || type == AA_AST_NODE_TYPE::boolliteral || type == AA_AST_NODE_TYPE::charliteral ||
		type == AA_AST_NODE_TYPE::floatliteral || type == AA_AST_NODE_TYPE::stringliteral;
}

bool AAC::IsVariable(AA_AST_NODE_TYPE type) {
	return type == AA_AST_NODE_TYPE::variable;
}

bool AAC::IsDecleration(AA_AST_NODE_TYPE type) {
	return type == AA_AST_NODE_TYPE::vardecl || type == AA_AST_NODE_TYPE::tuplevardecl;
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
	} else if (ws.compare(L"&&") == 0) {
		return AAByteCode::LAND;
	} else if (ws.compare(L"&") == 0) {
		return AAByteCode::BINAND;
	} else if (ws.compare(L"||") == 0) {
		return AAByteCode::LOR;
	} else if (ws.compare(L"|") == 0) {
		return AAByteCode::BINOR;
	} else {
		return AAByteCode::NOP;
	}

}

AAByteCode AAC::GetBytecodeFromUnaryOperator(std::wstring ws) {

	if (ws == L"!") {
		return AAByteCode::LNEG;
	} else if (ws == L"-") {
		return AAByteCode::NNEG;
	} else if (ws == L"++") {
		return AAByteCode::INC;
	} else if (ws == L"--") {
		return AAByteCode::DEC;
	} else {
		return AAByteCode::NOP;
	}

}

Instruction AAC::HandleConstPush(CompiledEnviornmentTable& cTable, bool bLit) {

	// Create the boolean value (This really needs a better way of dealing with literals to const...)
	AA_AnyLiteral aLit = AA_AnyLiteral();
	AALiteralType lType = AALiteralType::Boolean;
	aLit.b.val = bLit;

	// Push boolean
	return this->HandleConstPush(cTable, AA_Literal(aLit, AALiteralType::Boolean));

}

Instruction AAC::HandleConstPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode) {

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

Instruction AAC::HandleConstPush(CompiledEnviornmentTable& cTable, AA_Literal lit) {

	// Get the constant ID to push
	int i = -1;
	if (cTable.constValues.Contains(lit)) {
		i = cTable.constValues.IndexOf(lit);
	} else {
		i = cTable.constValues.Size();
		cTable.constValues.Add(lit);
	}

	if (lit.tp == AALiteralType::String) { // We treat strings a bit different
		
		Instruction pushStringOp;
		pushStringOp.bc = AAByteCode::PUSHWS;
		pushStringOp.argCount = 1;
		pushStringOp.argValues[0] = i;

		return pushStringOp;

	} else {

		Instruction pushOp;
		pushOp.bc = AAByteCode::PUSHC;
		pushOp.argCount = 1;
		pushOp.argValues[0] = i;

		return pushOp;

	}

}

Instruction AAC::HandleVarPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode) {

	CompiledAbstractExpression pushCAE;
	pushCAE.bc = AAByteCode::GETVAR;
	pushCAE.argCount = 1;
	
	if (pNode->HasTag("varsi")) {
		pushCAE.argValues[0] = pNode->tags["varsi"];
		return pushCAE;
	} else {
		pushCAE.argValues[0] = -1;
		printf("[AAC.Cpp@%i] Using a variable before it's declared!\n", __LINE__);
		return pushCAE;
	}

}

Instruction AAC::HandleFieldPush(AA_AST_NODE* pNode, AAStaticEnvironment staticData) {

	CompiledAbstractExpression pushCAE;
	pushCAE.bc = AAByteCode::GETFIELD;
	pushCAE.argCount = 2;
	pushCAE.argValues[0] = pNode->tags["fieldid"];
	pushCAE.argValues[1] = pNode->tags["primitive"];

	return pushCAE;

}

int AAC::HandleDecl(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode) {

	this->EnterScope(cTable, pNode);
	return pNode->tags["varsi"];

}

Instructions AAC::HandleTuplePush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode, AAStaticEnvironment staticData) {

	// Operation list
	Instructions opList;

	// For all tuple elements - push them unto the stack
	for (auto& tupleElement : pNode->expressions) {
		if (tupleElement->type == AA_AST_NODE_TYPE::variable && tupleElement->content.compare(L"_") == 0) {
			opList.Add(Instruction(AAByteCode::ACCEPT, 0, NULL));
		} else {
			opList.Add(this->CompileAST(tupleElement, cTable, staticData));
		}
	}

	// Tuple constructor instruction
	Instruction tupleCtor;
	tupleCtor.bc = AAByteCode::TUPLENEW;
	tupleCtor.argCount = pNode->expressions.size() + 1; // Note the current limitation of 7 tuple values
	tupleCtor.argValues[0] = tupleCtor.argCount - 1;

	// Define type for all tuple elements
	for (int i = 0; i < tupleCtor.argValues[0]; i++) { // TODO: Encode into a single 8 byte (2 ints) numeric, this way we can store *unlimited* tuple sizes
		if (pNode->expressions[i]->type == AA_AST_NODE_TYPE::variable && pNode->expressions[i]->content.compare(L"_") == 0) {
			tupleCtor.argValues[i + 1] = (int)AAPrimitiveType::__TRUEANY;
		} else {
			tupleCtor.argValues[i + 1] = pNode->expressions[i]->tags["primitive"];
		}
	}

	// Add tuple ctor to operation list
	opList.Add(tupleCtor);

	// Return operations list
	return opList;

}

Instructions AAC::HandleStackPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode, AAStaticEnvironment staticData) {

	Instructions opList;

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
	} else if (pNode->type == AA_AST_NODE_TYPE::tupleval) {
		opList.Add(this->HandleTuplePush(cTable, pNode, staticData));
	} else if (pNode->type == AA_AST_NODE_TYPE::lambdaexpression) {
		opList.Add(this->CompileLambda(pNode, cTable, staticData));
	} else {
		opList.Add(CompileAST(pNode, cTable, staticData));
	}

	// Does the argument contain the "wrap" tag -> wrap so 'Any' can accept it
	if (pNode->HasTag("wrap")) {

		// Wrap bytecode instruction
		CompiledAbstractExpression wrapCAE;
		wrapCAE.bc = AAByteCode::WRAP;
		wrapCAE.argCount = 1;
		wrapCAE.argValues[0] = pNode->tags["wrap"];

		// Add to operations list
		opList.Add(wrapCAE);

	}

	return opList;

}

Instructions AAC::HandleCtorCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& ctable, AAStaticEnvironment staticData) {

	// List containing the operations generated
	Instructions opList;

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

	// Push arguments to constructor
	for (size_t i = 0; i < pNode->expressions.size(); i++) {
		opList.Add(this->CompileAST(pNode->expressions[i], ctable, staticData));
	}

	// Add constructor call instruction
	CompiledAbstractExpression callCAE;
	callCAE.bc = AAByteCode::CTOR;
	callCAE.argCount = 3;
	callCAE.argValues[0] = pNode->tags["typeID"];
	callCAE.argValues[1] = procID;
	callCAE.argValues[2] = isVmCll;
	//callCAE.argValues[3] = allocSize;

	opList.Add(callCAE);

	return opList;

}

Instructions AAC::HandleMemberCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// The op list to get from member call
	Instructions opList;

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

Instructions AAC::HandleIndexPush(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData) {

	// The op list to get from member call
	aa::list<CompiledAbstractExpression> opList;

	// Handle var push (eg. actually get the variable we're looking for)
	opList.Add(this->HandleVarPush(cTable, pNode->expressions[0]));

	// Handle field fetch
	opList.Add(this->CompileAST(pNode->expressions[1], cTable, staticData));

	// Create dynamic method of fetching element operation
	CompiledAbstractExpression getElemCAE;
	getElemCAE.bc = AAByteCode::GETELEM;
	getElemCAE.argCount = 1;
	getElemCAE.argValues[0] = 0; // TODO: Add support for multidimension

	// Add to operations list
	opList.Add(getElemCAE);

	// Return the op list
	return opList;

}

Instructions AAC::HandlePop(AA_AST_NODE* pNode) {

	Instructions opList;

	if (pNode->HasTag("pop_size")) {

		Instruction popCAE;
		popCAE.argCount = 1;
		popCAE.bc = AAByteCode::POP;
		popCAE.argValues[0] = pNode->tags["pop_size"];

		opList.Add(popCAE);

	}

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
			wss << (int32_t)lit.lit.s.len;
			wss << std::wstring(lit.lit.s.val);
			break;
		default:
			break;
		}

	}

	wss << (int32_t)constTable.identifiers.Size();

	for (size_t i = 0; i < constTable.identifiers.Size(); i++) {
		wss << (int32_t)i;
	}

}

void AAC::ConvertToBytes(Instruction expr, aa::bstream& bis) {

	bis << (unsigned char)expr.bc;

	for (int i = 0; i < expr.argCount; i++) {
		bis << expr.argValues[i];
	}

}

AAByteVTable* AAC::ConvertClassVTableToBinary(AAClassSignature* pClass) {

	// Fetch the vtable
	AAClassVirtualTable* pVTable = pClass->classVTable;

	// VTable result
	AAByteVTable* vTableOut = new AAByteVTable;
	vTableOut->vtable = _vtable(pVTable->virtualFunctions.size());

	size_t i = 0;
	for (auto& pair : pVTable->virtualFunctions) {

		// The functions map
		aa::array<_vtablefunc> funcs = aa::array<_vtablefunc>(pair.second.Size());

		// counter
		size_t j = 0;

		// For each virtual function
		pair.second.ForEach(
			[&funcs, &j](std::pair<AAClassSignature*, AAFuncSignature*>& v) {  
				funcs[j] = _vtablefunc(v.first->type->constantID, v.second->procID);
				j++;
			}
		);

		// Assign to binary vtable
		vTableOut->vtable[i] = _vtableelem(pClass->FindMethodFromFunctionalSignature(pair.first)->procID, funcs);

		// Goto next vtable entry
		i++;

	}

	// Return the vtable
	return vTableOut;

}

AAByteType AAC::ConvertTypeToBytes(AACType* pType, aa::list<AACType*>& typeList) {

	if (pType) {
		AAByteType outType = AAByteType(pType->GetFullname());
		outType.constID = pType->constantID;
		if (pType->isRefType) {
			
			// Handle other derivations
			this->CompileClassToBytes(outType, pType, typeList);

		}
		return outType;
	} else {
		return AAByteType();
	}

}

void AAC::CompileClassToBytes(AAByteType& outType, AACType* pType, aa::list<AACType*>& typeList) {

	// Get class signature
	AAClassSignature* pClass = pType->classSignature;

	// Make sure pointer is valid
	if (pClass) {

		// Calculate class size
		outType.unmanagedSize = (uint16_t)this->m_classCompiler->CalculateMemoryUse(pClass);

		// Get base pointer
		unsigned int basePtr = pClass->basePtr;
		
		// Handle base inheritance, if any
		if (basePtr != UINT32_MAX) {
			AACType* baseType = pClass->extends.Apply(basePtr)->type;
			if (baseType->name.compare(L"object") == 0) {
				outType.baseTypePtr = AAByteType::_BasePtrObj;
			} else {
				outType.baseTypePtr = (uint16_t)typeList.IndexOf(baseType);
			}
		} else {
			outType.baseTypePtr = AAByteType::_BasePtrObjNone;
		}

		// Compile VTable is class has such a thing
		if (pClass->classVTable != 0) {
			outType.vtable = this->ConvertClassVTableToBinary(pClass);
		}

		// If class is taged
		if (aa::modifiers::ContainsFlag(pClass->storageModifier, AAStorageModifier::TAGGED)) {

			// Tagged fields
			std::vector<AAByteTagField> taggedFields;

			// Loop though all fields
			for (size_t i = 0; i < pClass->fields.Size(); i++) {
				AAClassFieldSignature& field = pClass->fields.Apply(i);
				if (field.tagged) {
					AAByteTagField f;
					f.fieldId = (uint16_t)field.fieldOffset; // We actualy have to use the offset...
					f.ptype = (unsigned char)aa::runtime::runtimetype_from_statictype(field.type);
					if (field.type->isRefType || field.type->isEnum) {
						f.typePtr = (uint32_t)typeList.IndexOf(field.type);
					} // TODO: Handle more cases here...
					taggedFields.push_back(f);
				}
			}

			// Tagged fields
			outType.taggedFields = new AAByteTagFieldList(aa::array<AAByteTagField>(taggedFields));

		}

	} else {
	
		// Something has happened
		printf("-->> Unknown compile error, expected class type, received unknown <<--");
	
	}

}

aa::list<AAByteType> AAC::CompileTypedata(AAStaticEnvironment staticCompileData) {

	// Export type list and add a "null" buffer type
	aa::list<AACType*> exportTypes;
	exportTypes.Add(NULL);

	// Add non-primitive types
	exportTypes.Add(this->FetchTypedata(staticCompileData.globalNamespace));

	// Map AACTypes to the respective AAByte types
	aa::list<AAByteType> byteTypes = exportTypes.Map<AAByteType>(
		[this, &exportTypes](AACType*& type) {
			return this->ConvertTypeToBytes(type, exportTypes);
		}
	);

	// TODO: run through all types and correct pointers

	// Return types
	return byteTypes;

}

aa::list<AACType*> AAC::FetchTypedata(AACNamespace* pNamespace) {

	// The export types
	aa::list<AACType*> exportTypes;

	// For all subspaces
	pNamespace->childspaces.ForEach(
		[&exportTypes, this](AACNamespace*& subSpace) { 
			exportTypes.Add(this->FetchTypedata(subSpace));
		}
	);

	// For all types in namespace
	pNamespace->types.ForEach(
		[&exportTypes, this](AACType*& type) {
			if (!aa::runtime::is_primitive_type(type)) {
				exportTypes.Add(type); // add more checks before actually adding here
			}
		}
	);

	// Return all types to export
	return exportTypes;

}

AAC_Out AAC::CompileFromProcedures(aa::list<CompiledProcedure> procedures, AAStaticEnvironment staticCompileData, int entryPoint) {

	// Bytecode compilation result
	AAC_Out compileBytecodeResult;

	// Byte stream
	aa::bstream bis;

	bis.write_bytes(10, m_version);

	// Write procedure count
	bis << (int32_t)procedures.Size();

	// Write entry point (Pointer to the first operation to execute)
	bis << entryPoint;

	// For all procedures
	for (size_t p = 0; p < procedures.Size(); p++) {

		// Convert constants table to bytecode
		ConstTableToByteCode(procedures.At(p).procEnvironment, bis);

		// Write amount of operations
		bis << (int32_t)procedures.At(p).procOperations.Size();

		// Write all expressions in their compiled formats
		for (size_t i = 0; i < procedures.At(p).procOperations.Size(); i++) {
			this->ConvertToBytes(procedures.At(p).procOperations.At(i), bis);
		}

	}

	if (m_byteTypes.Size() > 1) {

		// Write exported type count
		bis << (int32_t)(m_byteTypes.Size() - 1);

		// Write all types
		m_byteTypes.ForEach(
			[&bis](AAByteType& byteType) {
				if (byteType.typeName) { // Only write it if we have a valid name!
					size_t count;
					unsigned char* bytes = byteType.ToBytes(count);
					bis.write_bytes(count, bytes);
					delete[] bytes;
				}
			}
		);

	} else {

		bis << (int32_t)0;

	}

	// Get size and allocate memory buffer
	compileBytecodeResult.length = bis.length();
	compileBytecodeResult.bytes = new unsigned char[(int)compileBytecodeResult.length];

	// Copy stream into array
	memcpy(compileBytecodeResult.bytes, bis.array(), (int)compileBytecodeResult.length);

	// Return the resulting bytecode
	return compileBytecodeResult;

}

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
