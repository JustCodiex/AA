#include "AAC.h"
#include "AATypeChecker.h"
#include <stack>

void AAC::ResetCompilerInternals() {

	// Reset current procedure ID
	m_currentProcID = 0;

}

AAC_Out AAC::CompileFromAbstractSyntaxTrees(std::vector<AA_AST*> trees) {

	// Reset the internals of the compiler
	this->ResetCompilerInternals();

	// Collapse global tree sections
	this->CollapseGlobalScope(trees);

	// Run the static checkers
	CompiledStaticChecks staticChecks = this->RunStaticOperations(trees);

	// Compiled procedure results
	std::vector<AAC::CompiledProcedure> compileResults;

	// Compile all the trees into individual procedures
	for (size_t i = 0; i < trees.size(); i++) {

		// Generated bytecode
		compileResults.push_back(CompileProcedureFromAST(trees[i], staticChecks));

	}

	// Map the procedures to their respective functions
	staticChecks.exportSignatures = this->MapProcedureToSignature(staticChecks, compileResults);

	// Compile all procedures into bytecode
	AAC_Out bytecode = CompileFromProcedures(compileResults, staticChecks, 0);

	// Aslo return bytecode (so we can execute it directly)
	return bytecode;

}

AAC::CompiledProcedure AAC::CompileProcedureFromAST(AA_AST* pAbstractTree, CompiledStaticChecks staticData) {

	// Procedure for the abstract tree
	CompiledProcedure proc;
	proc.node = pAbstractTree->GetRoot();

	// Compile the execution stack
	proc.procOperations = CompileAST(pAbstractTree->GetRoot(), proc.procEnvironment, staticData);

	// Return the procedure
	return proc;

}

void AAC::CollapseGlobalScope(std::vector<AA_AST*>& trees) {

	size_t i = 0;
	AA_AST_NODE* pGlobalScope = new AA_AST_NODE(L"", AA_AST_NODE_TYPE::block, AACodePosition(0, 0));

	while (i < trees.size()) {

		if (trees[i]->GetRoot()->type != AA_AST_NODE_TYPE::fundecl) {

			pGlobalScope->expressions.push_back(trees[i]->GetRoot());
			trees.erase(trees.begin() + i);

		} else {
			i++;
		}

	}

	trees.insert(trees.begin(), new AA_AST(pGlobalScope));

}

AAC::CompiledStaticChecks AAC::RunStaticOperations(std::vector<AA_AST*> trees) {

	// Static function checks
	CompiledStaticChecks staticChecks;

	// For all input trees, register possible functions
	for (size_t i = 0; i < trees.size(); i++) {

		// Register functions
		staticChecks.registeredFunctions.Add(RegisterFunctions(trees[i]->GetRoot(), trees[i]->GetRoot()));

	}

	// For all input trees, run static type checker
	for (size_t i = 0; i < trees.size(); i++) {

		// Register functions
		this->TypecheckAST(trees[i]);

	}

	// For all input trees, optimize
	for (size_t i = 0; i < trees.size(); i++) {

		// Simplify the abstract tree
		trees[i]->Simplify();

	}

	// Return result of static function check
	return staticChecks;

}

void AAC::TypecheckAST(AA_AST* pTree) {

	// Currently, we just run a simple type check
	AATypeChecker checker = AATypeChecker(pTree);
	checker.TypeCheck();

}

aa::list<AAC::CompiledStaticChecks::SigPointer> AAC::RegisterFunctions(AA_AST_NODE* pNode, AA_AST_NODE* pSource) {

	aa::list<AAC::CompiledStaticChecks::SigPointer> signatures;

	if (pNode->type == AA_AST_NODE_TYPE::classdecl) { // TODO: change to classdecl

		// stuff

	} else if (pNode->type == AA_AST_NODE_TYPE::fundecl) {
		signatures.Add(this->RegisterFunction(pNode));
	}

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

	pNode->tags["funcsignature"] = (int)&sig; // BAD
	pNode->tags["returncount"] = this->GetReturnCount(&sig);

	return sP;

}

std::vector<AAC::CompiledAbstractExpression> AAC::CompileAST(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	// Stack
	std::vector<CompiledAbstractExpression> executionStack;

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::binop: {
		executionStack = Merge(executionStack, CompileBinaryOperation(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::unop: {
		executionStack = Merge(executionStack, CompileUnaryOperation(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::block: {
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			executionStack = Merge(executionStack, this->CompileAST(pNode->expressions[i], cTable, staticData));
		}
		break;
	}
	case AA_AST_NODE_TYPE::funcbody: {
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			
			executionStack = Merge(executionStack, this->CompileAST(pNode->expressions[i], cTable, staticData));
		}
		
		break;
	}
	case AA_AST_NODE_TYPE::fundecl: {
		if (pNode->expressions.size() == 3) { // We've got the actual function body => compile it
			this->CompileAST(pNode->expressions[1], cTable, staticData);
			std::vector<CompiledAbstractExpression> args = this->CompileFuncArgs(pNode, cTable, staticData);
			std::vector<CompiledAbstractExpression> body = Merge(args, this->CompileAST(pNode->expressions[2], cTable, staticData));
			CompiledAbstractExpression retCAE;
			retCAE.bc = AAByteCode::RET;
			retCAE.argCount = 1;
			retCAE.argValues[0] = pNode->tags["returncount"];
			body.push_back(retCAE);
			executionStack = Merge(executionStack, body);
		}
		break;
	}
	case AA_AST_NODE_TYPE::funcall: {
		executionStack = Merge(executionStack, CompileFunctionCall(pNode, cTable, staticData));
		break;
	}
	case AA_AST_NODE_TYPE::funarglist: {
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			cTable.identifiers.Add(pNode->expressions[i]->content);
		}
		break;
	}
	// Implicit return
	case AA_AST_NODE_TYPE::variable:
	case AA_AST_NODE_TYPE::intliteral:
	case AA_AST_NODE_TYPE::floatliteral:
	case AA_AST_NODE_TYPE::charliteral:
	case AA_AST_NODE_TYPE::stringliteral:
	case AA_AST_NODE_TYPE::boolliteral:
	{
		executionStack = Merge(executionStack, (HandleStackPush(cTable, pNode, staticData)));
		break;
	}
	default:
		break;
	}

	return executionStack;

}

std::vector<AAC::CompiledAbstractExpression> AAC::CompileBinaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	std::vector<CompiledAbstractExpression> opList;

	CompiledAbstractExpression binopCAE;
	binopCAE.argCount = 0;
	binopCAE.bc = GetBytecodeFromBinaryOperator(pNode->content);

	if (binopCAE.bc == AAByteCode::SETVAR) {

		binopCAE.argCount = 1;
		binopCAE.argValues[0] = HandleDecl(cTable, pNode->expressions[0]);
		opList = Merge(opList, (HandleStackPush(cTable, pNode->expressions[1], staticData)));

	} else {

		opList = Merge(opList, (HandleStackPush(cTable, pNode->expressions[0], staticData)));
		opList = Merge(opList, (HandleStackPush(cTable, pNode->expressions[1], staticData)));

	}

	opList.push_back(binopCAE);

	return opList;

}

std::vector<AAC::CompiledAbstractExpression> AAC::CompileUnaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	std::vector<CompiledAbstractExpression> opList;

	CompiledAbstractExpression unopCAE;
	unopCAE.argCount = 0;
	unopCAE.bc = GetBytecodeFromUnaryOperator(pNode->content);

	opList = Merge(opList, (HandleStackPush(cTable, pNode->expressions[0], staticData)));

	opList.push_back(unopCAE);

	return opList;

}

std::vector<AAC::CompiledAbstractExpression> AAC::CompileFunctionCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	std::vector<CompiledAbstractExpression> opList;

	int progArgs = 0; // Going real safe here
	int procID = this->FindBestFunctionMatch(staticData, pNode, progArgs);

	if (progArgs > 0) {
		for (AA_AST_NODE* pArg : pNode->expressions) {
			opList = Merge(opList, this->HandleStackPush(cTable, pArg, staticData));
		}
	}

	CompiledAbstractExpression callCAE;
	callCAE.argCount = 2;
	callCAE.bc = AAByteCode::CALL;
	callCAE.argValues[0] = procID;
	callCAE.argValues[1] = progArgs;

	opList.push_back(callCAE);

	return opList;

}

std::vector<AAC::CompiledAbstractExpression> AAC::CompileFuncArgs(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData) {

	std::vector<CompiledAbstractExpression> opList;

	for (size_t i = 0; i < pNode->expressions[1]->expressions.size(); i++) {
		CompiledAbstractExpression argCAE;
		argCAE.bc = AAByteCode::SETVAR;
		argCAE.argCount = 1;

		// Set the argument of size() - 1 - i (the inverse) becausue of the push-pop mechanism used by the AAVM to pass call arguments
		argCAE.argValues[0] = cTable.identifiers.IndexOf(pNode->expressions[1]->expressions[pNode->expressions[1]->expressions.size() - 1 - i]->content);

		// Add operation
		opList.push_back(argCAE);

	}

	return opList;

}

std::vector<AAC::CompiledAbstractExpression> AAC::Merge(std::vector<CompiledAbstractExpression> original, std::vector<CompiledAbstractExpression> add) {

	std::vector<CompiledAbstractExpression> merged = original;

	while (add.size() > 0) {
		merged.push_back(add.front());
		add.erase(add.begin());
	}

	return merged;

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

AAByteCode AAC::GetBytecodeFromBinaryOperator(std::wstring ws) {

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
		return AAByteCode::SETVAR;
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

std::vector<AAC::CompiledAbstractExpression> AAC::HandleStackPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode, CompiledStaticChecks staticData) {

	std::vector<CompiledAbstractExpression> opList;

	if (IsConstant(pNode->type)) {
		opList.push_back(this->HandleConstPush(cTable, pNode));
	} else if (IsVariable(pNode->type)) {
		opList.push_back(this->HandleVarPush(cTable, pNode));
	} else {
		opList = Merge(opList, CompileAST(pNode, cTable, staticData));
	}

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
	bis << (int)staticCompileData.exportSignatures.size();

	// Write the exported signatures
	for (size_t s = 0; s < staticCompileData.exportSignatures.size(); s++) {

		// Write out the export signatures
		bis << (int)staticCompileData.exportSignatures[s].name.length();
		bis << staticCompileData.exportSignatures[s].name;
		bis << staticCompileData.exportSignatures[s].procID;

	}

	// Write procedure count
	bis << (int)procedures.size();

	// Write entry point (Pointer to the first operation to execute)
	bis << entryPoint;

	// For all procedures
	for (size_t p = 0; p < procedures.size(); p++) {

		// Convert constants table to bytecode
		ConstTableToByteCode(procedures[p].procEnvironment, bis);

		// Write amount of operations
		bis << (int)procedures[p].procOperations.size();

		// Write all expressions in their compiled formats
		for (size_t i = 0; i < procedures[p].procOperations.size(); i++) {
			this->ConvertToBytes(procedures[p].procOperations[i], bis);
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

std::vector< AAC::CompiledSignature> AAC::MapProcedureToSignature(CompiledStaticChecks staticCheck, std::vector<AAC::CompiledProcedure> procedureLs) {

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

int AAC::FindBestFunctionMatch(CompiledStaticChecks staticCheck, AA_AST_NODE* pNode, int& argCount) { // TODO: Make the typechecker do this

	std::wstring funcName = pNode->content;

	for (size_t i = 0; i < staticCheck.registeredFunctions.Size(); i++) {
		AAC::CompiledStaticChecks::SigPointer sig = staticCheck.registeredFunctions.At(i);
		if (sig.funcSig.name == funcName) {
			if (pNode->expressions.size() == sig.funcSig.parameters.size()) {
				bool isMatch = true; //false;
				for (AAFuncParam p : sig.funcSig.parameters) {

				}
				if (isMatch) {
					argCount = (int)sig.funcSig.parameters.size();
					return sig.procID;
				}
			}
		}
	}

	wprintf(L"Unknown function! %s", funcName.c_str());

	argCount = 0;
	return 0;

}
