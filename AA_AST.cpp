#include "AA_AST.h"

AA_AST::AA_AST(AA_PT* parseTree) {

	m_root = this->AbstractNode(parseTree->GetRoot());

}

AA_AST_NODE* AA_AST::AbstractNode(AA_PT_NODE* pNode) {

	switch (pNode->nodeType) {
	case AA_PT_NODE_TYPE::block: {

		AA_AST_NODE* blockNode = new AA_AST_NODE(L"", AA_AST_NODE_TYPE::block);

		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			blockNode->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}

		return blockNode;
	}
	case AA_PT_NODE_TYPE::binary_operation: {

		AA_AST_NODE* binaryNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::binop);

		binaryNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		binaryNode->expressions.push_back(this->AbstractNode(pNode->childNodes[1]));

		return binaryNode;

	}
	case AA_PT_NODE_TYPE::unary_operation: {

		AA_AST_NODE* unaryNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::unop);
		unaryNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));

		return unaryNode;

	}
	case AA_PT_NODE_TYPE::decleration: {

		if (pNode->childNodes[0]->content == L"var") {
			return new AA_AST_NODE(pNode->childNodes[1]->content, AA_AST_NODE_TYPE::vardecl);
		} else { // When a specific type is specified

		}

		break;
	}
	case AA_PT_NODE_TYPE::intliteral:
		return new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::intliteral);
	case AA_PT_NODE_TYPE::identifier:
		return new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::variable);
	default:
		break;
	}

	return 0;

}

void AA_AST::Simplify() {

	return;

}
