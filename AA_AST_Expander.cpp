#include "AA_AST_Expander.h"
#include "AAAutoCodeGenerator.h"

void AA_AST_Expander::ExpandTree(AA_AST* pTree) {

	// Expand the root of the tree
	this->ExpandNode(pTree->GetRoot());

	// TODO: Report errors

}

void AA_AST_Expander::ExpandNode(AA_AST_NODE* pNode) {

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::block:
	case AA_AST_NODE_TYPE::classbody:
	case AA_AST_NODE_TYPE::funcbody:
	case AA_AST_NODE_TYPE::enumbody:
	case AA_AST_NODE_TYPE::name_space:
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			this->ExpandNode(pNode->expressions[i]);
		}
		break;
	case AA_AST_NODE_TYPE::binop:
		this->ExpandBinaryOperation(pNode);
		break;
	case AA_AST_NODE_TYPE::unop:
		this->ExpandUnaryOperation(pNode);
		break;
	case AA_AST_NODE_TYPE::fundecl:
		if (aa::parsing::Function_HasBody(pNode)) {
			this->ExpandNode(pNode->expressions[AA_NODE_FUNNODE_BODY]);
		}
	case AA_AST_NODE_TYPE::classdecl:
		if (aa::parsing::Class_HasBody(pNode)) {
			this->ExpandNode(pNode->expressions[AA_NODE_CLASSNODE_BODY]);
		}
	case AA_AST_NODE_TYPE::enumdecleration:
		if (aa::parsing::Enum_HasBody(pNode)) {
			this->ExpandNode(pNode->expressions[AA_NODE_ENUMNODE_BODY]);
		}
	default:
		break;
	}

}

void AA_AST_Expander::ExpandBinaryOperation(AA_AST_NODE* pNode) {

	// Expand left and right side of operation
	this->ExpandNode(pNode->expressions[0]);
	this->ExpandNode(pNode->expressions[1]);

	// If compoint assignment, expand it
	if (this->IsCompundAssignment(pNode->type, pNode->content)) {
		this->ExpandCompundAssignment(pNode);
	}

}

void AA_AST_Expander::ExpandUnaryOperation(AA_AST_NODE* pNode) {



}

void AA_AST_Expander::ExpandCompundAssignment(AA_AST_NODE* pNode) {

	// Unwrap the correct operators to use
	std::wstring opRight = pNode->content.substr(0, 1);
	pNode->content = pNode->content[1];

	// Create binary op node
	AA_AST_NODE* pBinOpNode = aa::compiler::generator::expr::BinaryOperationNode(opRight, pNode->expressions[0]->Clone(), pNode->expressions[1]);

	// Assign RHS of operation to new binary operation
	pNode->expressions[1] = pBinOpNode;

}

bool AA_AST_Expander::IsCompundAssignment(AA_AST_NODE_TYPE type, std::wstring ws) {
	if (type == AA_AST_NODE_TYPE::binop) {
		return ws.compare(L"+=") == 0 || ws.compare(L"-=") == 0 || ws.compare(L"*=") == 0 || ws.compare(L"/=") == 0 || ws.compare(L"%=") == 0;
	} else {
		return false;
	}
}
