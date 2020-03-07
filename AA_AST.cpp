#include "AA_AST.h"

AA_AST::AA_AST(AA_PT* parseTree) {
	m_root = this->AbstractNode(parseTree->GetRoot());
}

void AA_AST::Clear() {
	this->ClearNode(m_root);
}

void AA_AST::ClearNode(AA_AST_NODE* pNode) {

	for (AA_AST_NODE* child : pNode->expressions) {
		this->ClearNode(child);
	}

	delete pNode;

}

AA_AST_NODE* AA_AST::AbstractNode(AA_PT_NODE* pNode) {

	switch (pNode->nodeType) {
	case AA_PT_NODE_TYPE::funcbody:
	case AA_PT_NODE_TYPE::block: {

		AA_AST_NODE* blockNode = new AA_AST_NODE(L"", this->GetASTBlockType(pNode->nodeType), pNode->position);

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

		if (pNode->childNodes.size() == 3) {
			funDecl->expressions.push_back(this->AbstractNode(pNode->childNodes[2]));
		}

		return funDecl;

	}
	case AA_PT_NODE_TYPE::funccall: {

		AA_AST_NODE* funCall = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::funcall, pNode->position);

		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			funCall->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}

		return funCall;

	}
	case AA_PT_NODE_TYPE::funarglist: {

		AA_AST_NODE* ls = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::funarglist, pNode->position);
		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			ls->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}

		return ls;

	}
	case AA_PT_NODE_TYPE::funarg: {
		AA_AST_NODE* arg = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::funarg, pNode->position);
		arg->expressions.push_back(new AA_AST_NODE(pNode->childNodes[0]->content, AA_AST_NODE_TYPE::typeidentifier, pNode->childNodes[0]->position));
		return arg;
	}
	case AA_PT_NODE_TYPE::condition: {
		AA_AST_NODE* con = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::condition, pNode->position);
		con->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		return con;
	}
	case AA_PT_NODE_TYPE::ifstatement: {
		AA_AST_NODE* ifstatement = new AA_AST_NODE(L"if", AA_AST_NODE_TYPE::ifstatement, pNode->position);
		ifstatement->expressions.push_back(this->AbstractNode(pNode->childNodes[0])); // condition
		ifstatement->expressions.push_back(this->AbstractNode(pNode->childNodes[1])); // stuff to execute if condition holds
		size_t restBlock = pNode->childNodes.size() - 2;
		for (size_t i = 0; i < restBlock; i++) {
			ifstatement->expressions.push_back(this->AbstractNode(pNode->childNodes[i + 2]));
		}
		return ifstatement;
	}
	case AA_PT_NODE_TYPE::elsestatement: {
		AA_AST_NODE* elsestatement = new AA_AST_NODE(L"else", AA_AST_NODE_TYPE::elsestatement, pNode->position);
		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			elsestatement->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}
		return elsestatement;
	}
	case AA_PT_NODE_TYPE::elseifstatement: {
		AA_AST_NODE* elifstatement = new AA_AST_NODE(L"elseif", AA_AST_NODE_TYPE::elseifstatement, pNode->position);
		elifstatement->expressions.push_back(this->AbstractNode(pNode->childNodes[0])); // condition
		elifstatement->expressions.push_back(this->AbstractNode(pNode->childNodes[1])); // stuff to execute if condition holds
		return elifstatement;
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

AA_AST_NODE_TYPE AA_AST::GetASTBlockType(AA_PT_NODE_TYPE type) {
	switch (type) {
	case AA_PT_NODE_TYPE::funcbody:
		return AA_AST_NODE_TYPE::funcbody;
	case AA_PT_NODE_TYPE::classbody:
		return AA_AST_NODE_TYPE::classbody;
	case AA_PT_NODE_TYPE::block:
	default:
		return AA_AST_NODE_TYPE::block;
	}
}

void AA_AST::Simplify() {

	return;

}
