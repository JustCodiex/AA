#include "AAC.h"
#include <stack>

AAC_Out AAC::CompileFromAST(AA_AST* pAbstractTree) {

	// Store resulting bytecode
	AAC_Out result = AAC_Out();

	// Simplify the abstract tree
	pAbstractTree->Simplify();

	// Constant Value table container
	CompiledEnviornmentTable consTable;

	// Compile the execution stack
	std::vector<CompiledAbstractExpression> opList = CompileAST(pAbstractTree->GetRoot(), consTable);

	// Combine it all to bytecode string
	this->ToByteCode(opList, consTable, result);

	// return bytecode
	return result;

}

std::vector<AAC::CompiledAbstractExpression> AAC::CompileAST(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable) {

	// Stack
	std::vector<CompiledAbstractExpression> executionStack;

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::binop: {
		executionStack = Merge(executionStack, CompileBinaryOperation(pNode, cTable));
		break;
	}
	case AA_AST_NODE_TYPE::unop: {
		executionStack = Merge(executionStack, CompileUnaryOperation(pNode, cTable));
		break;
	}
	case AA_AST_NODE_TYPE::block: {
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			executionStack = Merge(executionStack, this->CompileAST(pNode->expressions[i], cTable));
		}
		break;
	}
	// Implicitt return
	case AA_AST_NODE_TYPE::variable:
	case AA_AST_NODE_TYPE::intliteral:
	case AA_AST_NODE_TYPE::floatliteral:
	case AA_AST_NODE_TYPE::charliteral:
	case AA_AST_NODE_TYPE::stringliteral:
	{
		executionStack = Merge(executionStack, (HandleStackPush(cTable, pNode)));
		break;
	}
	default:
		break;
	}

	return executionStack;

}

std::vector<AAC::CompiledAbstractExpression> AAC::CompileBinaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable) {

	std::vector<CompiledAbstractExpression> opList;

	CompiledAbstractExpression binopCAE;
	binopCAE.argCount = 0;
	binopCAE.bc = GetBytecodeFromBinaryOperator(pNode->content);

	if (binopCAE.bc == AAByteCode::SETVAR) {

		binopCAE.argCount = 1;
		binopCAE.argValues[0] = HandleDecl(cTable, pNode->expressions[0]);
		opList = Merge(opList, (HandleStackPush(cTable, pNode->expressions[1])));

	} else {

		opList = Merge(opList, (HandleStackPush(cTable, pNode->expressions[0])));
		opList = Merge(opList, (HandleStackPush(cTable, pNode->expressions[1])));

	}

	opList.push_back(binopCAE);

	return opList;

}

std::vector<AAC::CompiledAbstractExpression> AAC::CompileUnaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable) {

	std::vector<CompiledAbstractExpression> opList;

	CompiledAbstractExpression unopCAE;
	unopCAE.argCount = 0;
	unopCAE.bc = GetBytecodeFromUnaryOperator(pNode->content);

	opList = Merge(opList, (HandleStackPush(cTable, pNode->expressions[0])));

	opList.push_back(unopCAE);

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
	return type == AA_AST_NODE_TYPE::intliteral;
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

std::vector<AAC::CompiledAbstractExpression> AAC::HandleStackPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode) {

	std::vector<CompiledAbstractExpression> opList;

	if (IsConstant(pNode->type)) {
		opList.push_back(this->HandleConstPush(cTable, pNode));
	} else if (IsVariable(pNode->type)) {
		opList.push_back(this->HandleVarPush(cTable, pNode));
	} else {
		opList = Merge(opList, CompileAST(pNode, cTable));
	}

	return opList;

}

void AAC::ToByteCode(std::vector<CompiledAbstractExpression> bytecodes, CompiledEnviornmentTable constTable, AAC_Out& result) {

	// Byte stream
	aa::bstream bis;

	// Convert constants table to bytecode
	ConstTableToByteCode(constTable, bis);

	// Write amount of operations
	bis << (int)bytecodes.size();

	// Write all expressions in their compiled formats
	for (size_t i = 0; i < bytecodes.size(); i++) {
		this->ConvertToBytes(bytecodes[i], bis);
	}

	// Get size and allocate memory buffer
	result.length = bis.length();
	result.bytes = new unsigned char[(int)result.length];
	
	// Copy stream into array
	memcpy(result.bytes, bis.array(), (int)result.length);

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
		default:
			break;
		}

	}

	wss << (int)constTable.identifiers.Size();

	for (size_t i = 0; i < constTable.identifiers.Size(); i++) {

		//std::wstring identifier = constTable.identifiers.At(i);

		//wss << (int)identifier.length();
		//wss << identifier;

		wss << (int)i;

	}

}

void AAC::ConvertToBytes(CompiledAbstractExpression expr, aa::bstream& bis) {

	bis << (unsigned char)expr.bc;

	for (int i = 0; i < expr.argCount; i++) {
		bis << expr.argValues[i];
	}

}
