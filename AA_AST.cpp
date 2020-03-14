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
	case AA_PT_NODE_TYPE::classbody:
	case AA_PT_NODE_TYPE::block: {

		AA_AST_NODE* blockNode = new AA_AST_NODE(L"{<block>}", this->GetASTBlockType(pNode->nodeType), pNode->position);

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
			varDeclType->expressions.push_back(new AA_AST_NODE(pNode->childNodes[0]->content, AA_AST_NODE_TYPE::typeidentifier, pNode->position));
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
	case AA_PT_NODE_TYPE::forstatement: {
		AA_AST_NODE* forstatement = new AA_AST_NODE(L"for", AA_AST_NODE_TYPE::forstatement, pNode->position);
		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			forstatement->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}
		return forstatement;
	}
	case AA_PT_NODE_TYPE::whilestatement: {
		AA_AST_NODE* whilestatement = new AA_AST_NODE(L"while", AA_AST_NODE_TYPE::whilestatement, pNode->position);
		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			whilestatement->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}
		return whilestatement;
	}
	case AA_PT_NODE_TYPE::dowhilestatement: {
		AA_AST_NODE* dowhilestatement = new AA_AST_NODE(L"do-while", AA_AST_NODE_TYPE::dowhilestatement, pNode->position);
		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			dowhilestatement->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}
		return dowhilestatement;
	}
	case AA_PT_NODE_TYPE::classdecleration: {
		AA_AST_NODE* classdef = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::classdecl, pNode->position);
		classdef->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		return classdef;
	}
	case AA_PT_NODE_TYPE::newstatement: {
		AA_AST_NODE* newkw = new AA_AST_NODE(L"new", AA_AST_NODE_TYPE::newstatement, pNode->position);
		if (pNode->childNodes[0]->nodeType == AA_PT_NODE_TYPE::funccall) {
			AA_AST_NODE* ctorNode = this->AbstractNode(pNode->childNodes[0]);
			ctorNode->type = AA_AST_NODE_TYPE::classctorcall;
			newkw->expressions.push_back(ctorNode);
		} else {
			printf("Something");
		}
		return newkw;
	}
	case AA_PT_NODE_TYPE::accessor: {
		AA_AST_NODE_TYPE nType = (pNode->childNodes[1]->nodeType == AA_PT_NODE_TYPE::identifier) ? AA_AST_NODE_TYPE::fieldaccess : AA_AST_NODE_TYPE::callaccess;
		AA_AST_NODE* accessorNode = new AA_AST_NODE(pNode->content, nType, pNode->position);
		accessorNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		accessorNode->expressions.push_back(this->AbstractNode(pNode->childNodes[1]));
		if (nType == AA_AST_NODE_TYPE::fieldaccess) {
			accessorNode->expressions[1]->type = AA_AST_NODE_TYPE::field;
		}
		return accessorNode;
	}
	case AA_PT_NODE_TYPE::intliteral:
	case AA_PT_NODE_TYPE::charliteral:
	case AA_PT_NODE_TYPE::floatliteral:
	case AA_PT_NODE_TYPE::booliterral:
	case AA_PT_NODE_TYPE::stringliteral:
	case AA_PT_NODE_TYPE::nullliteral:
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
	case AA_PT_NODE_TYPE::nullliteral:
		return AA_AST_NODE_TYPE::nullliteral;
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
	this->SimplifyNode(m_root);
}

AA_AST_NODE* AA_AST::SimplifyNode(AA_AST_NODE* pNode) {

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::fundecl:
		if (pNode->expressions.size() >= 3) {
			size_t i = 0;
			while (i < pNode->expressions[2]->expressions.size()) {
				AA_AST_NODE* expNode = this->SimplifyNode(pNode->expressions[2]->expressions[i]);
				if (expNode) {
					pNode->expressions[2]->expressions[i] = expNode;
					i++;
				} else {
					pNode->expressions[2]->expressions.erase(pNode->expressions[2]->expressions.begin() + i);
				}
			}
		}
		return pNode;
	case AA_AST_NODE_TYPE::binop:
		return this->SimplifyBinaryNode(pNode);
	case AA_AST_NODE_TYPE::callaccess:
		return this->SimplifyCallAccessorNode(pNode);
	default:
		return pNode;
	}

}

AA_AST_NODE* AA_AST::SimplifyCallAccessorNode(AA_AST_NODE* pNode) {

	if (pNode->expressions[0]->type == AA_AST_NODE_TYPE::variable && pNode->expressions[1]->type == AA_AST_NODE_TYPE::funcall) {
		pNode->expressions[1]->expressions.insert(pNode->expressions[1]->expressions.begin(), pNode->expressions[0]);
		return pNode->expressions[1];
	}

	return pNode;

}

AA_AST_NODE* AA_AST::SimplifyBinaryNode(AA_AST_NODE* pBinaryNode) {

	// Simplify lhs
	pBinaryNode->expressions[0] = this->SimplifyNode(pBinaryNode->expressions[0]);

	// Simplify rhs
	pBinaryNode->expressions[1] = this->SimplifyNode(pBinaryNode->expressions[1]);

	// TODO: Handle constant binary operations - eg. 5+5 = 10

	return pBinaryNode;

}
