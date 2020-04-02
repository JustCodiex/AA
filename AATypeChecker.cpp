#include "AATypeChecker.h"
#include "astring.h"
#include "AAStaticAnalysis.h"
#include "set.h"

aa::list<std::wstring> _getdeftypeenv() {
	aa::list<std::wstring> types;
	types.Add(L"null");
	types.Add(L"int");
	types.Add(L"float");
	types.Add(L"bool");
	types.Add(L"char");
	return types;
}

aa::list<std::wstring> AATypeChecker::DefaultTypeEnv = _getdeftypeenv();

std::wstring AATypeChecker::InvalidTypeStr = L"TYPE NOT FOUND";

#define AATC_ERROR(msg, pos) this->SetError(AATypeChecker::Error(msg, __COUNTER__, pos)); return AATypeChecker::InvalidTypeStr

AATypeChecker::AATypeChecker(AA_AST* pTree, AAStaticEnvironment* senv) {
	
	// Set the tree to work with
	m_currentTree = pTree;

	// Set environment
	m_senv = senv;

	// We dont have any error to start with
	m_hasEnyErr = false;

	// Set the current namespace to be that of the global namespace
	m_currentnamespace = m_senv->globalNamespace;

}

bool AATypeChecker::TypeCheck() {

	// Type check root
	std::wstring ws = this->TypeCheckNode(m_currentTree->GetRoot());

	// Did we get invalid output?
	if (m_hasEnyErr || ws == InvalidTypeStr) {
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
		return this->TypeCheckBinaryOperation(node, node->expressions[0], node->expressions[1]);
	case AA_AST_NODE_TYPE::unop:
		return this->TypeCheckUnaryOperation(node, node->expressions[0]);
	case AA_AST_NODE_TYPE::callaccess:
		if (node->content == L".") {
			return this->TypeCheckClassDotCallAccessorOperation(node, node->expressions[0], node->expressions[1]);
		} else {
			break;
		}
	case AA_AST_NODE_TYPE::fieldaccess:
		if (node->content == L".") {
			return this->TypeCheckClassDotFieldAccessorOperation(node, node->expressions[0], node->expressions[1]);
		} else {
			break;
		}
	case AA_AST_NODE_TYPE::classdecl:
		return this->TypeCheckNode(node->expressions[0]);
	case AA_AST_NODE_TYPE::fundecl: 
		return this->TypeCheckFuncDecl(node);
	case AA_AST_NODE_TYPE::funcall:
		return this->TypeCheckCallOperation(node);
	case AA_AST_NODE_TYPE::newstatement:
		return this->TypeCheckNewStatement(node);
	case AA_AST_NODE_TYPE::classctorcall:
		return node->content; // Node content will match the ctor class type
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
		if (this->IsValidType(node->content)) {
			return node->content;
		} else {
			wprintf(L"Invalid type identifier '%s'\n", node->content.c_str());
			break;
		}
	case AA_AST_NODE_TYPE::funarg:
		if (this->IsValidType(node->expressions[0]->content)) {
			return node->expressions[0]->content;
		} else {
			wprintf(L"Invalid type '%s' in argument!\n", node->content.c_str());
			break;
		}
	case AA_AST_NODE_TYPE::ifstatement:
		return this->TypeCheckConditionalBlock(node);
	case AA_AST_NODE_TYPE::vardecl:
		if (this->IsValidType(node->expressions[0]->content)) {
			return node->expressions[0]->content;
		} else {
			AATC_ERROR("Undefined type " + string_cast(node->expressions[0]->content), node->position);
		}
	case AA_AST_NODE_TYPE::index:
		return this->TypeCheckIndexOperation(node);
	case AA_AST_NODE_TYPE::name_space: {
		
		// Define the matching lambda
		auto lambda = [node](AACNamespace*& s) { return s->name.compare(node->content) == 0; };

		// Check if the current namespace has the new namespace
		if (this->m_currentnamespace->childspaces.Contains(lambda)) {

			// Update current namespace
			this->m_currentnamespace = this->m_currentnamespace->childspaces.FindFirst(lambda);

			// Add new elements
			this->m_senv->availableClasses = this->m_currentnamespace->classes.Union(this->m_senv->availableClasses);
			this->m_senv->availableTypes = this->m_currentnamespace->types.Union(this->m_senv->availableTypes);
			this->m_senv->availableFunctions = this->m_currentnamespace->functions.Union(this->m_senv->availableFunctions);

			// Typecheck all subelements of the namespace
			for (size_t i = 0; i < node->expressions.size(); i++) {
				this->TypeCheckNode(node->expressions[i]);
			}

			// Return to previous namespace			
			this->m_senv->availableClasses = this->m_currentnamespace->classes.Difference(this->m_senv->availableClasses);
			this->m_senv->availableTypes = this->m_currentnamespace->types.Difference(this->m_senv->availableTypes);
			this->m_senv->availableFunctions = this->m_currentnamespace->functions.Difference(this->m_senv->availableFunctions);
			this->m_currentnamespace = this->m_currentnamespace->parentspace;

		} else {
			wprintf(L"Something very unfotunate has happened"); // Very fatal, should never happen
		}
		return L"void";
	}
	default:
		break;
	}

	return InvalidTypeStr;

}

std::wstring AATypeChecker::TypeCheckBinaryOperation(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right) {
	if (left->type == AA_AST_NODE_TYPE::vardecl) {
		
		std::wstring typeRight = this->TypeCheckNode(right);
		if (left->expressions.size() > 0) {
			m_vtenv[left->content] = left->expressions[0]->content;
		} else {
			m_vtenv[left->content] = typeRight;
		}
		std::wstring typeLeft = m_vtenv[left->content];
		if (typeLeft != typeRight) { 

			// TODO: Actual typecheck (comparing int and float is allowed, string and int not)
			// TODO: Consider custom operators

			// Set error message
			this->SetError(
				AATypeChecker::Error(
					"Type mismsatch on binary operation '" + string_cast(pOpNode->content) + "', left operand: '" + string_cast(typeLeft) + "' and right operand: '" + string_cast(typeRight) + "'",
					__COUNTER__, pOpNode->position)
			); // Compiler Error 1 (Typemismatch on binary operator)
			return InvalidTypeStr;

		}
		
		return typeLeft;

	} else {

		AAValType typeLeft = this->TypeCheckNode(left);
		AAValType typeRight = this->TypeCheckNode(right);
		
		if (IsPrimitiveType(typeLeft) && IsPrimitiveType(typeRight)) {
			pOpNode->tags["useCall"] = false;
			return this->TypeCheckBinaryOperationOnPrimitive(pOpNode, typeLeft, typeRight);
		} else {

			std::wstring op = pOpNode->content;
			AAClassSignature ccLeft = FindCompiledClassOfType(typeLeft);

			if (ccLeft.name == InvalidTypeStr) {
				/*this->SetError(
					AATypeChecker::Error(
						"Unknown operand type '" + string_cast(typeLeft) + "'",
						__COUNTER__, pOpNode->position)
				);
				return InvalidTypeStr;*/ return typeLeft; // TODO: Reenable this when function parameter type checking has been enabled
			}

			AAClassOperatorSignature ccop;
			if (this->FindCompiledClassOperation(ccLeft, op, typeRight, ccop)) {

				// Tag the operator node with the procID
				pOpNode->tags["useCall"] = true;
				pOpNode->tags["operatorProcID"] = ccop.method.procID;
				pOpNode->tags["operatorIsVM"] = ccop.method.isVMFunc;

				// Return the type of whatever the operator will return
				return ccop.method.returnType;

			} else {
				this->SetError(
					AATypeChecker::Error(
						"Invalid operator '" + string_cast(op) + "' on left operand type '" + string_cast(typeLeft) + "' and right operand type '" + string_cast(typeRight) + "'",
						__COUNTER__, pOpNode->position)
					);
				return InvalidTypeStr;
			}

		}

	}

}

AAValType AATypeChecker::TypeCheckBinaryOperationOnPrimitive(AA_AST_NODE* pOpNode, AAValType typeLeft, AAValType typeRight) {

	if (typeLeft != typeRight) {
		//printf("Incorrect type!"); // TODO: Implement this when function calls have been implemented
	}

	return typeLeft;

}

AAValType AATypeChecker::TypeCheckUnaryOperation(AA_AST_NODE* pOpNode, AA_AST_NODE* right) {
	// TODO: Actually do a type check
	return this->TypeCheckNode(right);
}

AAValType AATypeChecker::TypeCheckConditionalBlock(AA_AST_NODE* pConditionalNode) {



	// A conditional block may be part of flow-control system and may not actually return something
	// Which is perfectly legal
	return L"void";

}

AAValType AATypeChecker::TypeCheckClassDotCallAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right) {

	// Check types on left and right side
	AAValType l = this->TypeCheckNode(left);
	
	// If the right operator is a function call
	if (right->type == AA_AST_NODE_TYPE::funcall) {
		right->content = l + L"::" + right->content;
	} // else ....

	// Get type on the right
	AAValType r = this->TypeCheckNode(right);

	// Return whatever we've accessed from the right side
	return r;

}

AAValType AATypeChecker::TypeCheckClassDotFieldAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right) {

	// Check types on the left
	AAValType l = this->TypeCheckNode(left);

	// Get class from lhs
	AAClassSignature cc = this->FindCompiledClassOfType(l);

	// Make sure we got a valid class from this
	if (cc.name == InvalidTypeStr) {
		// Push error
		return InvalidTypeStr;
	}

	int field;
	if (AAClassCompiler::HasField(cc, right->content, field)) {

		// Save the ID of the field we're referencing
		right->tags["fieldid"] = field;

		// Return field type
		return cc.fields.Apply(field).type;

	} else {

		// Push error (invalid field name)

		return InvalidTypeStr;

	}

}

AAValType AATypeChecker::TypeCheckCallOperation(AA_AST_NODE* pCallNode) {

	// Find the first AAFuncSignature matching our conditions
	aa::set<AAFuncSignature> sigs = m_senv->availableFunctions.FindAll([pCallNode](AAFuncSignature& sig) { return sig.name == pCallNode->content; });
	AAFuncSignature sig = sigs.FindFirst([this, pCallNode](AAFuncSignature& sig) { return this->IsTypeMatchingFunction(sig, pCallNode); });

	if (!(sig == AAFuncSignature())) {

		pCallNode->tags["calls"] = (int)sig.procID;

		return sig.returnType;

	} else {

		wprintf(L"Attempt to call undefined function '%s'", pCallNode->content.c_str());

		return InvalidTypeStr;

	}

}

AAValType AATypeChecker::TypeCheckFuncDecl(AA_AST_NODE* pDeclNode) {

	// Do we have a body that also needs type verification/checking?
	if (pDeclNode->expressions.size() >= 3) {
		
		// Type verify body
		this->TypeCheckNode(pDeclNode->expressions[2]);

	}

	// Return the type the function returns
	return this->TypeCheckNode(pDeclNode->expressions[0]);

}

AAValType AATypeChecker::TypeCheckNewStatement(AA_AST_NODE* pNewStatement) {

	if (pNewStatement->expressions[0]->type == AA_AST_NODE_TYPE::classctorcall) {
		if (this->IsValidType(pNewStatement->expressions[0]->content)) {
			return this->TypeCheckNode(pNewStatement->expressions[0]);
		}
	} else if (pNewStatement->expressions[0]->type == AA_AST_NODE_TYPE::index) {
		AAValType tp = pNewStatement->expressions[0]->expressions[0]->content;
		if (this->IsValidType(tp)) {
			return tp + L"[]";
		}
	}

	AATC_ERROR("Undefined type '" + string_cast(pNewStatement->expressions[0]->content) + "' in new statement", pNewStatement->position);
	return InvalidTypeStr;

}

AAValType AATypeChecker::TypeCheckIndexOperation(AA_AST_NODE* pIndexNode) {

	AAValType arrType = this->TypeOf(pIndexNode->expressions[0]->content);

	if (IsValidType(arrType) && IsArrayType(arrType)) {
		return this->TypeOfArrayElements(arrType);
	}

	return InvalidTypeStr;

}

AAValType AATypeChecker::TypeOf(AAId var) {

	if (this->m_vtenv.find(var) != this->m_vtenv.end()) {
		return this->m_vtenv[var];
	} else {
		return InvalidTypeStr;
	}

}

AAValType AATypeChecker::TypeOfArrayElements(AAValType t) {
	if (t.length() > 2) {
		return t.substr(0, t.length() - 2);
	} else {
		return InvalidTypeStr;
	}
}

bool AATypeChecker::IsValidType(AAValType t) {
	if (!m_senv->availableTypes.Contains(t)) {
		if (t.length() > 2 && t.substr(t.length() - 2) == L"[]") {
			return m_senv->availableTypes.Contains(t.substr(0, t.length() - 2));
		} else {
			return t.compare(L"void") == 0;
		}
	} else {
		return true;
	}	
}

bool AATypeChecker::IsPrimitiveType(AAValType t) {
	return t.compare(L"int") == 0 || t.compare(L"float") == 0 || t.compare(L"char") == 0 || t.compare(L"bool") == 0;
}

bool AATypeChecker::IsArrayType(AAValType t) {
	return (t.length() > 2 && t.substr(t.length() - 2).compare(L"[]") == 0);
}

bool AATypeChecker::IsMatchingTypes(AAValType tCompare, AAValType tExpected) {

	if (tCompare.compare(tExpected) == 0) {
		return true;
	} else {
		if (tExpected.compare(L"Any") == 0) {
			return true;
		} else {
			return false; // TODO: Check inheritance
		}
	}

}

bool AATypeChecker::IsTypeMatchingFunction(AAFuncSignature sig, AA_AST_NODE* pCallNode) {

	// Parameter offset
	size_t paramOffset = sig.isClassMethod ? 1 : 0;

	// Is this the correct function to call?
	if (pCallNode->expressions.size() == sig.parameters.size() - paramOffset) {

		// Flag to make sure the argument types are valid
		bool isMatchingArgTypes = true;

		// For all params
		for (size_t j = paramOffset; j < sig.parameters.size(); j++) {

			// If they're not a type match, break
			if (!this->IsMatchingTypes(this->TypeCheckNode(pCallNode->expressions[j - paramOffset]), sig.parameters[j].type)) {
				isMatchingArgTypes = false;
				break;
			}

		}

		// Return whatever we found out in the above loop
		return isMatchingArgTypes;

	}

	return false;

}

AAClassSignature AATypeChecker::FindCompiledClassOfType(AAValType type) {

	try {

		AAClassSignature& cc = m_senv->availableClasses.FindFirst([type](AAClassSignature& sig) { return sig.name == type; });
		return cc;

	} catch (std::exception e) {

		AAClassSignature no;
		no.name = InvalidTypeStr;

		return no;

	}

}

bool AATypeChecker::FindCompiledClassOperation(AAClassSignature cc, std::wstring operatorType, AAValType right, AAClassOperatorSignature& op) {

	// Find the first operator matching the condition
	op = cc.operators.FindFirst([operatorType, right](AAClassOperatorSignature& sig) { return sig.op.compare(operatorType) == 0 && sig.method.parameters[1].type.compare(right) == 0; });

	// Return true if the operator is not a default operator
	return !(op == AAClassOperatorSignature());

}
