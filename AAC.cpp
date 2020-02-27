#include "AAC.h"
#include <stack>

AAC_Out AAC::CompileFromAST(AA_AST* pAbstractTree) {

	// Store resulting bytecode
	AAC_Out result = AAC_Out();

	// Simplify the abstract tree
	pAbstractTree->Simplify();

	// Compile the execution stack
	std::vector<CompiledAbstractExpression> executionStack = CompileAST(pAbstractTree->GetRoot());

	// return bytecode
	return result;

}

std::vector<AAC::CompiledAbstractExpression> AAC::CompileAST(AA_AST_NODE* pNode) {

	// Stack
	std::vector<CompiledAbstractExpression> executionStack;

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::binop: {

		CompiledAbstractExpression binopCAE;
		binopCAE.argCount = 2;
		binopCAE.argValues = new int[2];

		if (pNode->content == L"+") {
			binopCAE.bc = AAByteCode::ADD;
		}

		if (IsConstant(pNode->expressions[0]->type)) {
			binopCAE.argValues[0] = -1;
			executionStack.push_back(CompiledAbstractExpression(AAByteCode::PUSH_CONST, 1, new int[1]{ 0, }));
		} else {
			binopCAE.argValues[0] = -1;
			executionStack = Merge(executionStack, CompileAST(pNode->expressions[0]));
		}

		if (IsConstant(pNode->expressions[1]->type)) {
			binopCAE.argValues[0] = -2;
			executionStack.push_back(CompiledAbstractExpression(AAByteCode::PUSH_CONST, 1, new int[1]{ 0, }));
		} else {
			binopCAE.argValues[0] = -2;
			executionStack = Merge(executionStack, CompileAST(pNode->expressions[1]));
		}

		executionStack.push_back(binopCAE);

		break;
	}
	default:
		break;
	}

	return executionStack;

}

AAC::CompiledAbstractExpression AAC::CompileExpression(AA_AST_NODE* pNode) {

	CompiledAbstractExpression CAE;
	
	switch (pNode->type) {
	case AA_AST_NODE_TYPE::binop:
		if (pNode->content == L"+") {
			CAE.bc = AAByteCode::ADD;
		}
		break;
	default:
		break;
	}

	return CAE;

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
