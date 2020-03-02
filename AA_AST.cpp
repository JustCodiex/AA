#include "AA_AST.h"

AA_AST::AA_AST(AA_PT* parseTree) {

	m_root = this->AbstractNode(parseTree->GetRoot());

}

AA_AST_NODE* AA_AST::AbstractNode(AA_PT_NODE* pNode) {

	switch (pNode->nodeType) {
	case AA_PT_NODE_TYPE::block: {

		AA_AST_NODE* blockNode = new AA_AST_NODE(L"", AA_AST_NODE_TYPE::block, pNode->position);

		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			blockNode->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}

		return blockNode;

	}
	case AA_PT_NODE_TYPE::binary_operation: {

		AA_AST_NODE* binaryNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::binop, pNode->position);

		binaryNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		binaryNode->expressions.push_back(this->AbstractNode(pNode->childNodes[1]));

		return binaryNode;

	}
	case AA_PT_NODE_TYPE::unary_operation: {

		AA_AST_NODE* unaryNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::unop, pNode->position);
		unaryNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));

		return unaryNode;

	}
	case AA_PT_NODE_TYPE::vardecleration: {

		if (pNode->childNodes[0]->content == L"var") {
			return new AA_AST_NODE(pNode->childNodes[1]->content, AA_AST_NODE_TYPE::vardecl, pNode->position);
		} else { // When a specific type is specified
			AA_AST_NODE* varDeclType = new AA_AST_NODE(pNode->childNodes[1]->content, AA_AST_NODE_TYPE::vardecl, pNode->position);
			varDeclType->expressions.push_back(new AA_AST_NODE(pNode->childNodes[0]->content, AA_AST_NODE_TYPE::variable, pNode->position));
			return varDeclType;
		}

		break;
	}
	case AA_PT_NODE_TYPE::fundecleration: {
		
		AA_AST_NODE* funDecl = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::fundecl, pNode->position);
		funDecl->expressions.push_back(new AA_AST_NODE(pNode->childNodes[0]->content, AA_AST_NODE_TYPE::typeidentifier, pNode->position));
		funDecl->expressions.push_back(this->AbstractNode(pNode->childNodes[1]));

		if (pNode->childNodes.size() >= 3) {
			funDecl->expressions.push_back(this->AbstractNode(pNode->childNodes[2]));
		}

		return funDecl;

	}
	case AA_PT_NODE_TYPE::intliteral:
	case AA_PT_NODE_TYPE::charliteral:
	case AA_PT_NODE_TYPE::floatliteral:
	case AA_PT_NODE_TYPE::booliterral:
	case AA_PT_NODE_TYPE::stringliteral:
		return new AA_AST_NODE(pNode->content, GetASTLiteralType(pNode->nodeType), pNode->position);
	case AA_PT_NODE_TYPE::identifier:
		return new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::variable, pNode->position);
	default:
		break;
	}

	return 0;

}

AA_AST_NODE_TYPE AA_AST::GetASTLiteralType(AA_PT_NODE_TYPE type) {
	switch (type) {
	case AA_PT_NODE_TYPE::intliteral:
		return AA_AST_NODE_TYPE::intliteral;
	case AA_PT_NODE_TYPE::floatliteral:
		return AA_AST_NODE_TYPE::floatliteral;
	case AA_PT_NODE_TYPE::stringliteral:
		return AA_AST_NODE_TYPE::stringliteral;
	case AA_PT_NODE_TYPE::charliteral:
		return AA_AST_NODE_TYPE::charliteral;
	case AA_PT_NODE_TYPE::booliterral:
		return AA_AST_NODE_TYPE::boolliteral;
	default:
		return AA_AST_NODE_TYPE::variable;
	}
}

void AA_AST::Simplify() {

	return;

}
