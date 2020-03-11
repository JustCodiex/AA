#include "AATypeChecker.h"

aa::list<std::wstring> _getdeftypeenv() {
	aa::list<std::wstring> types;
	types.Add(L"int");
	types.Add(L"float");
	types.Add(L"bool");
	types.Add(L"string");
	types.Add(L"char");
	types.Add(L"null");
	return types;
}

aa::list<std::wstring> AATypeChecker::DefaultTypeEnv = _getdeftypeenv();

std::wstring AATypeChecker::InvalidTypeStr = L"TYPE NOT FOUND";

AATypeChecker::AATypeChecker(AA_AST* pTree, aa::list<std::wstring> regTypes, aa::list<AAFuncSignature> sigs) {
	
	// Set the tree to work with
	m_currentTree = pTree;

	// Set the registered types
	m_types = regTypes;

	// Set the function environment
	m_ftenv = sigs;

}

bool AATypeChecker::TypeCheck() {

	// Type check root
	std::wstring ws = this->TypeCheckNode(m_currentTree->GetRoot());

	// Did we get invalid output?
	if (ws == InvalidTypeStr) {
		return false;
	} else {
		if (!this->IsValidType(ws)) {
			return ws == L"void";
		} else {
			return true;
		}
	}

}

std::wstring AATypeChecker::TypeCheckNode(AA_AST_NODE* node) {

	switch (node->type) {
	case AA_AST_NODE_TYPE::funcbody:
	case AA_AST_NODE_TYPE::classbody:
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
	case AA_AST_NODE_TYPE::accessor:
		if (node->content == L".") {
			return this->TypeCheckClassDotAccessorOperation(node->expressions[0], node->expressions[1]);
		} else {
			break;
		}
	case AA_AST_NODE_TYPE::classdecl:
		return this->TypeCheckNode(node->expressions[0]);
	case AA_AST_NODE_TYPE::fundecl: 
		return this->TypeCheckFuncDecl(node);
	case AA_AST_NODE_TYPE::funcall:
		return this->TypeCheckCallOperation(node);
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
	case AA_AST_NODE_TYPE::nullliteral:
		return L"null";
	case AA_AST_NODE_TYPE::variable:
		return m_vtenv[node->content];
	case AA_AST_NODE_TYPE::typeidentifier:
		if (m_types.Contains(node->content)) {
			return node->content;
		} else {
			printf("Invalid type identifier!");
			break;
		}
	case AA_AST_NODE_TYPE::funarg:
		if (m_types.Contains(node->expressions[0]->content)) {
			return node->expressions[0]->content;
		} else {
			printf("Invalid type identifier!");
			break;
		}
	default:
		break;
	}

	return InvalidTypeStr;

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

std::wstring AATypeChecker::TypeCheckClassDotAccessorOperation(AA_AST_NODE* left, AA_AST_NODE* right) {

	// Check types on left and right side
	std::wstring l = this->TypeCheckNode(left);
	
	// If the right operator is a function call
	if (right->type == AA_AST_NODE_TYPE::funcall) {
		right->content = l + L"::" + right->content;
	}

	// Get type on the right
	std::wstring r = this->TypeCheckNode(right);

	// Store some static data in the left-side node
	//left->tags["accesstype"] = 0; // Let the accessor operation know it's a class access
	//left->tags["classtyperef"] = (int)(m_types.IndexOf(l)); // Let accessor operation know the index of the class type

	// Return whatever we've accessed from the right side
	return r;

}

AAValType AATypeChecker::TypeCheckCallOperation(AA_AST_NODE* pCallNode) {

	// Loop through all possible functions
	for (size_t i = 0; i < m_ftenv.Size(); i++) {

		// Does the function contain the name we need?
		if (m_ftenv.At(i).name == pCallNode->content) {

			AAFuncSignature sig = m_ftenv.At(i);

			// TODO: Typecheck params
			pCallNode->tags["calls"] = (int)i;

			return sig.returnType;

		}

	}

	printf("Err: Call to undefined function!");

	return InvalidTypeStr;

}

AAValType AATypeChecker::TypeCheckFuncDecl(AA_AST_NODE* pDeclNode) {

	// Do we have a body that also needs type verification/checking?
	if (pDeclNode->expressions.size() >= 3) {
		
		// Type verify body
		this->TypeCheckNode(pDeclNode->expressions[2]);

	}

	return this->TypeCheckNode(pDeclNode->expressions[0]);

}

std::wstring AATypeChecker::TypeOf(AAId var) {

	if (this->m_vtenv.find(var) != this->m_vtenv.end()) {
		return this->m_vtenv[var];
	} else {
		return InvalidTypeStr;
	}

}

bool AATypeChecker::IsValidType(AAValType t) {
	return m_types.Contains(t);
}
