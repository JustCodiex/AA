#include "AATypeChecker.h"
#include "astring.h"
#include "AAStaticAnalysis.h"
#include "set.h"

// Temp solution, replace with const variables later
static int AATypeCheckerGlobalErrorCounter = 1000;

#define AATC_ERROR(msg, pos) this->SetError(AATypeChecker::Error(msg, ++AATypeCheckerGlobalErrorCounter, pos)); return AACType::ErrorType

AATypeChecker::AATypeChecker(AA_AST* pTree, AAStaticEnvironment* senv) {
	
	// Set the tree to work with
	m_currentTree = pTree;

	// Set environment
	m_senv = senv;

	// We dont have any error to start with
	m_hasEnyErr = false;

	// Set the current namespace to be that of the global namespace
	m_currentnamespace = m_senv->globalNamespace;

	// Set the local statement namespace to 0
	m_localStatementNamespace = 0;

}

bool AATypeChecker::TypeCheck() {

	// Type check root
	AACType* type = this->TypeCheckNode(m_currentTree->GetRoot());

	// Did we get invalid output?
	if (m_hasEnyErr) {
		return false;
	} else {
		return this->IsValidType(type);
	}

}

AACType* AATypeChecker::TypeCheckNode(AA_AST_NODE* node) {

	switch (node->type) {
	case AA_AST_NODE_TYPE::funcbody:
	case AA_AST_NODE_TYPE::classbody:
	case AA_AST_NODE_TYPE::block: {
		AACType* r = AACType::ErrorType;
		for (size_t i = 0; i < node->expressions.size(); i++) {
			r = this->TypeCheckNode(node->expressions[i]);
			if (r == AACType::ErrorType) {
				printf("Detected error type (Ln %i Column %i)\n", node->position.line, node->position.column);
			}
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
	case AA_AST_NODE_TYPE::memberaccess:
		return this->TypeCheckMemberAccessorOperation(node, node->expressions[0], node->expressions[1]);
	case AA_AST_NODE_TYPE::classdecl:
		return this->TypeCheckNode(node->expressions[0]);
	case AA_AST_NODE_TYPE::fundecl: 
		return this->TypeCheckFuncDecl(node);
	case AA_AST_NODE_TYPE::funcall:
		return this->TypeCheckCallOperation(node);
	case AA_AST_NODE_TYPE::newstatement:
		return this->TypeCheckNewStatement(node);
	case AA_AST_NODE_TYPE::classctorcall:
		return this->FindType(node->content); // Node content will match the ctor class type
	case AA_AST_NODE_TYPE::intliteral:
		return AACTypeDef::Int32;
	case AA_AST_NODE_TYPE::charliteral:
		return AACTypeDef::Char;
	case AA_AST_NODE_TYPE::floatliteral:
		return AACTypeDef::Float32;
	case AA_AST_NODE_TYPE::stringliteral:
		return AACTypeDef::String;
	case AA_AST_NODE_TYPE::boolliteral:
		return AACTypeDef::Bool;
	case AA_AST_NODE_TYPE::nullliteral:
		return AACType::Null;
	case AA_AST_NODE_TYPE::variable:
		return m_vtenv[node->content];
	case AA_AST_NODE_TYPE::typeidentifier: {
		AACType* t = this->FindType(node->content);
		if (t == AACType::ErrorType) {
			wprintf(L"Invalid type identifier '%s'\n", node->content.c_str());
			break;
		} else {
			return t;
		}
	}
	case AA_AST_NODE_TYPE::funarg: {
		AACType* t = this->FindType(node->expressions[0]->content);
		if (t == AACType::ErrorType) {
			wprintf(L"Invalid type '%s' in argument!\n", node->content.c_str());
			break;
		} else {
			return t;
		}
	}
	case AA_AST_NODE_TYPE::ifstatement:
		return this->TypeCheckConditionalBlock(node);
	case AA_AST_NODE_TYPE::vardecl: {
		AACType* t = this->FindType(node->expressions[0]->content);
		if (t == AACType::ErrorType) {
			AATC_ERROR("Undefined type " + string_cast(node->expressions[0]->content) + "\n", node->position);
		} else {
			return t;
		}
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
			wprintf(L"Something very unfotunate has happened\n"); // Very fatal, should never happen
		}
		return AACType::Void;
	}
	case AA_AST_NODE_TYPE::usingspecificstatement:
	case AA_AST_NODE_TYPE::usingstatement:
		return this->TypeCheckUsingOperation(node);
	default:
		break;
	}

	return AACType::ErrorType;

}

AACType* AATypeChecker::TypeCheckBinaryOperation(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right) {

	// Is it a declerative assignment operation?
	if (left->type == AA_AST_NODE_TYPE::vardecl) {
		
		// declare left and right type
		AACType* typeLeft = 0;
		AACType* typeRight = 0;

		// Does LHS contain a vardecl with a specific type?
		if (left->expressions.size() > 0) {
			typeLeft = m_vtenv[left->content] = this->TypeCheckNode(left->expressions[0]);
			typeRight = this->TypeCheckNode(right);
		} else { // Does LHS contain a vardecl without a specific type (eg. var or val)
			typeRight = typeLeft = m_vtenv[left->content] = this->TypeCheckNode(right);
		}
		
		// reset local namespace
		m_localStatementNamespace = 0;

		// Do these types match each other?
		if (this->IsMatchingTypes(typeRight, typeLeft)) {

			// return the left type
			return typeLeft;

		} else {

			// Set error message
			AATC_ERROR(
				"Type mismsatch on binary operation '" + string_cast(pOpNode->content) + "', left operand: '"
				+ string_cast(typeLeft->GetFullname()) + "' and right operand: '" + string_cast(typeRight->GetFullname()) + "'",
				pOpNode->position
			);
		
		}

	} else {

		AACType* typeLeft = this->TypeCheckNode(left);
		AACType* typeRight = this->TypeCheckNode(right);
		
		if (IsPrimitiveType(typeLeft) && IsPrimitiveType(typeRight)) {
			pOpNode->tags["useCall"] = false;
			return this->TypeCheckBinaryOperationOnPrimitive(pOpNode, typeLeft, typeRight);
		} else {

			std::wstring op = pOpNode->content;
			AAClassSignature* ccLeft = FindCompiledClassOfType(typeLeft);

			if (ccLeft->name == AACType::ErrorType->name) {
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
				AATC_ERROR(
					"Invalid operator '" + string_cast(op) + "' on left operand type '" + string_cast(typeLeft->GetFullname())
					+ "' and right operand type '" + string_cast(typeRight->GetFullname()) + "'",
					pOpNode->position
				);
			}

		}

	}

}

AACType* AATypeChecker::TypeCheckBinaryOperationOnPrimitive(AA_AST_NODE* pOpNode, AACType* typeLeft, AACType* typeRight) {

	if (typeLeft != typeRight) {
		//printf("Incorrect type!"); // TODO: Implement this when function calls have been implemented
	}

	return typeLeft;

}

AACType* AATypeChecker::TypeCheckUnaryOperation(AA_AST_NODE* pOpNode, AA_AST_NODE* right) {
	// TODO: Actually do a type check
	return this->TypeCheckNode(right);
}

AACType* AATypeChecker::TypeCheckConditionalBlock(AA_AST_NODE* pConditionalNode) {



	// A conditional block may be part of flow-control system and may not actually return something
	// Which is perfectly legal
	return AACType::Void;

}

AACType* AATypeChecker::TypeCheckClassDotCallAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right) {

	// Check types on left and right side
	AACType* l = this->TypeCheckNode(left);
	
	// If the right operator is a function call
	if (right->type == AA_AST_NODE_TYPE::funcall) {
		right->content = l->name + L"::" + right->content;
	} // else ....

	// Get type on the right
	AACType* r = this->TypeCheckNode(right);

	// Return whatever we've accessed from the right side
	return r;

}

AACType* AATypeChecker::TypeCheckClassDotFieldAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right) {

	// Check types on the left
	AACType* l = this->TypeCheckNode(left);

	// Get class from lhs
	AAClassSignature* cc = this->FindCompiledClassOfType(l);

	// Make sure we got a valid class from this
	if (cc->name == AACType::ErrorType->name) {
		// Push error
		return AACType::ErrorType;
	}

	int field;
	if (AAClassCompiler::HasField(cc, right->content, field)) {

		// Save the ID of the field we're referencing
		right->tags["fieldid"] = field;

		// Return field type
		return cc->fields.Apply(field).type;

	} else {

		// Push error (invalid field name)

		return AACType::ErrorType;

	}

}

AACType* AATypeChecker::TypeCheckCallOperation(AA_AST_NODE* pCallNode) {

	// Find the first AAFuncSignature matching our conditions
	aa::set<AAFuncSignature> sigs = m_senv->availableFunctions.FindAll([pCallNode](AAFuncSignature& sig) { return sig.name == pCallNode->content; });

	if (sigs.Size() > 0) {

		AAFuncSignature sig = sigs.FindFirst([this, pCallNode](AAFuncSignature& sig) { return this->IsTypeMatchingFunction(sig, pCallNode); });

		if (!(sig == AAFuncSignature())) {

			pCallNode->tags["calls"] = (int)sig.procID;

			return sig.returnType;

		} else {

			wprintf(L"Attempt to call undefined function '%s'\n", pCallNode->content.c_str());

			return AACType::ErrorType;

		}

	} else {

		wprintf(L"Attempt to call undefined function '%s'\n", pCallNode->content.c_str());

		return AACType::ErrorType;

	}

}

AACType* AATypeChecker::TypeCheckFuncDecl(AA_AST_NODE* pDeclNode) {

	// Do we have a body that also needs type verification/checking?
	if (pDeclNode->expressions.size() >= 3) {
		
		// Make a copy of the current variable environment
		AAVarTypeEnv vtenv = AAVarTypeEnv(m_vtenv);

		// For all arguments
		for (size_t i = 0; i < pDeclNode->expressions[1]->expressions.size(); i++) {

			// Typecheck argument
			AACType* argType = this->TypeCheckNode(pDeclNode->expressions[1]->expressions[i]);

			// Set variable type in environment
			if (argType != AACType::ErrorType) {
				m_vtenv[pDeclNode->expressions[1]->expressions[i]->content] = argType;
			} else {
				printf("Arg type error\n");
			}

		}

		// Type verify body
		this->TypeCheckNode(pDeclNode->expressions[2]);

		// Reset variable types
		m_vtenv = vtenv;

	}

	// Return the type the function returns
	return this->TypeCheckNode(pDeclNode->expressions[0]);

}

AACType* AATypeChecker::TypeCheckNewStatement(AA_AST_NODE* pNewStatement) {

	if (pNewStatement->expressions[0]->type == AA_AST_NODE_TYPE::classctorcall) {
		AACType* type = this->FindType(pNewStatement->expressions[0]->content);
		if (type != AACType::ErrorType) {
			return type;
		}
	} else if (pNewStatement->expressions[0]->type == AA_AST_NODE_TYPE::index) {
		AACType* tp = this->FindType(pNewStatement->expressions[0]->expressions[0]->content + L"[]"); // Put an additional [] on, such that it picks up the correct type
		if (this->IsValidType(tp)) {
			return tp;
		}
	}

	AATC_ERROR("Undefined type '" + string_cast(pNewStatement->expressions[0]->content) + "' in new statement", pNewStatement->position);

}

AACType* AATypeChecker::TypeCheckIndexOperation(AA_AST_NODE* pIndexNode) {

	AACType* arrType = this->TypeOf(pIndexNode->expressions[0]->content);

	if (arrType != AACType::ErrorType && arrType->isArrayType) {
		return arrType;
	}

	return AACType::ErrorType;

}

AACType* AATypeChecker::TypeCheckUsingOperation(AA_AST_NODE* pUseNode) {

	switch (pUseNode->type) {
	case AA_AST_NODE_TYPE::usingstatement: {

		break;
	}
	case AA_AST_NODE_TYPE::usingspecificstatement: {
		std::wstring import = pUseNode->content;
		std::wstring from = pUseNode->expressions[0]->content;
		auto findlambda = []() {};
		int i1;
		int i2;
		if (m_currentnamespace->childspaces.FindFirstIndex([from](AACNamespace*& domain) { return domain->name.compare(from) == 0; }, i1)) {
			if (m_currentnamespace->childspaces.Apply(i1)->classes.FindFirstIndex([import](AAClassSignature*& sig) { return sig->name.compare(import) == 0; }, i2)) {
				AAClassSignature* imported = m_currentnamespace->childspaces.Apply(i1)->classes.Apply(i2);
				this->m_senv->availableTypes.Add(imported->type);
				this->m_senv->availableClasses.Add(imported);
				wprintf(L"%ws\n", import.c_str());
			} else {
				// throw error
				wprintf(L"Class '%ws' not found\n", import.c_str());
			}
		} else {
			// throw error
			printf("Namespace '%ws' not found\n", from.c_str());
		}
		break;
	}
	}

	// Will always return void
	return AACType::Void;

}

AACType* AATypeChecker::TypeCheckMemberAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right) {

	int i1;
	if (m_currentnamespace->childspaces.FindFirstIndex([left](AACNamespace*& domain) { return left->content.compare(domain->name); }, i1)) {
		int i2;
		if (m_currentnamespace->childspaces.Apply(i1)->classes.FindFirstIndex([right](AAClassSignature*& sig) { return right->content.compare(sig->name) == 0; }, i2)) {
			m_localStatementNamespace = m_currentnamespace->childspaces.Apply(i1);
			return this->FindType(right->content);
		} else {
			AATC_ERROR("Member not found '" + string_cast(right->content) + "'", pAccessorNode->position);
		}
	}

	// TODO: Check classes

	return AACType::ErrorType;

}

AACType* AATypeChecker::TypeOf(AAId var) {

	if (this->m_vtenv.find(var) != this->m_vtenv.end()) {
		return this->m_vtenv[var];
	} else {
		return AACType::ErrorType;
	}

}

bool AATypeChecker::IsValidType(AACType* t) {
	if (!m_senv->availableTypes.Contains(t)) {
		return t == AACType::Void;
	} else {
		return true;
	}	
}

bool AATypeChecker::IsPrimitiveType(AACType* t) {
	return !t->isRefType;
}

bool AATypeChecker::IsMatchingTypes(AACType* tCompare, AACType* tExpected) {

	if (tCompare == tExpected) {
		return true;
	} else {
		if (tExpected == AACType::Any) {
			return true;
		} else {
			return false; // TODO: Check inheritance
		}
	}

}

AACType* AATypeChecker::FindType(std::wstring t) {

	// Is it void type?
	if (t.compare(L"void") == 0) {
		return AACType::Void;
	} else if (t.compare(L"Any") == 0) { // Or is it the any type?
		return AACType::Any;
	}

	int i;
	if (m_senv->availableTypes.FindFirstIndex([t](AACType*& type) { return type->name.compare(t) == 0; }, i)) {
		return m_senv->availableTypes.Apply(i);
	} else {

		// Is it an array type then?
		if (t.substr(t.length() - 2).compare(L"[]") == 0) {
			std::wstring subtype = t.substr(0, t.length() - 2);
			AACType* t = this->FindType(subtype);
			if (t != AACType::ErrorType && t != AACType::Any && t != AACType::Void) {
				AACType* arrType = t->AsArrayType();
				m_senv->availableTypes.Add(arrType); // add type (for recursive reasons)
				return arrType;
			} else {
				return AACType::ErrorType;
			}
		} else {
			return AACType::ErrorType;
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

AAClassSignature* AATypeChecker::FindCompiledClassOfType(AACType* type) {

	try {

		AAClassSignature* cc = m_senv->availableClasses.FindFirst([type](AAClassSignature*& sig) { return sig == type->classSignature; });
		return cc;

	} catch (std::exception e) {

		AAClassSignature* no = new AAClassSignature;
		no->name = AACType::ErrorType->name;

		return no;

	}

}

bool AATypeChecker::FindCompiledClassOperation(AAClassSignature* cc, std::wstring operatorType, AACType* right, AAClassOperatorSignature& op) {

	// Find the first operator matching the condition
	op = cc->operators.FindFirst([operatorType, right](AAClassOperatorSignature& sig) { return sig.op.compare(operatorType) == 0 && sig.method.parameters[1].type == right; });

	// Return true if the operator is not a default operator
	return !(op == AAClassOperatorSignature());

}
