#include "AATypeChecker.h"

AATypeChecker::AATypeChecker(AA_AST* pTree) {
	m_currentTree = pTree;
}

void AATypeChecker::TypeCheck() {

	// Type check root
	std::wstring ws = this->TypeCheckNode(m_currentTree->GetRoot());

}

std::wstring AATypeChecker::TypeCheckNode(AA_AST_NODE* node) {

	switch (node->type) {
	case AA_AST_NODE_TYPE::block: {
		std::wstring r = L"";
		for (size_t i = 0; i < node->expressions.size(); i++) {
			r = this->TypeCheckNode(node->expressions[i]);
		}
		return r;
	}
	case AA_AST_NODE_TYPE::binop:
		return this->TypeCheckBinaryOperation(node->expressions[0], node->expressions[1]);
	case AA_AST_NODE_TYPE::unop:
		return this->TypeCheckUnaryOperation(node->expressions[0]);
	case AA_AST_NODE_TYPE::intliteral:
		return L"int";
	case AA_AST_NODE_TYPE::charliteral:
		return L"char";
	case AA_AST_NODE_TYPE::floatliteral:
		return L"float";
	case AA_AST_NODE_TYPE::stringliteral:
		return L"string";
	case AA_AST_NODE_TYPE::boolliteral:
		return L"bool";
	case AA_AST_NODE_TYPE::variable:
		return m_vtenv[node->content];
	default:
		break;
	}

	return L"void";

}

std::wstring AATypeChecker::TypeCheckBinaryOperation(AA_AST_NODE* left, AA_AST_NODE* right) {

	if (left->type == AA_AST_NODE_TYPE::vardecl) {
		
		std::wstring typeRight = this->TypeCheckNode(right);
		if (left->expressions.size() > 0) {
			m_vtenv[left->content] = left->expressions[0]->content;
		} else {
			m_vtenv[left->content] = typeRight;
		}
		
		std::wstring typeLeft = m_vtenv[left->content];
		if (typeLeft != typeRight) {
			printf("Type mismatch");
		}
		
		return typeLeft;

	} else {

		std::wstring typeLeft = this->TypeCheckNode(left);
		std::wstring typeRight = this->TypeCheckNode(right);

		if (typeLeft != typeRight) {
			printf("Incorrect type!");
		}

		return typeLeft;

	}

}

std::wstring AATypeChecker::TypeCheckUnaryOperation(AA_AST_NODE* right) {
	// TODO: Actually do a type check
	return this->TypeCheckNode(right);
}
