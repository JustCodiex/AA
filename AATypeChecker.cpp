#include "AATypeChecker.h"
#include "astring.h"
#include "AAStaticAnalysis.h"
#include "AAPrimitiveType.h"
#include "set.h"

#define AATC_W_ERROR(__msg, __pos, __err) this->SetError(AATypeChecker::Error(__msg, __err, __pos)); return AACType::ErrorType
#define AATC_ERROR(__msg, __pos) AATC_W_ERROR((__msg), (__pos), (aa::compiler_err::C_Unclassified_Type))

AATypeChecker::AATypeChecker(AA_AST* pTree, AAStaticEnvironment* pStaticEnvionment, AADynamicTypeEnvironment * pDynamicEnvironment) {
	
	// Set the tree to work with
	m_currentTree = pTree;

	// Set the static environment
	m_senv = pStaticEnvionment;

	// Set the dynamic environment
	m_dynamicTypeEnvironment = pDynamicEnvironment;

	// We dont have any error to start with
	m_hasEnyErr = false;

	// Set the current namespace to be that of the global namespace
	m_currentnamespace = m_senv->globalNamespace;

	// Set the local statement namespace to 0
	m_localStatementNamespace = 0;

	// Define the typemapper
	m_typeMappingLambda = [this](std::wstring nodeType) { return this->FindType(nodeType); };

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
	case AA_AST_NODE_TYPE::forafterthought:
	case AA_AST_NODE_TYPE::forinit:
	case AA_AST_NODE_TYPE::funcbody:
	case AA_AST_NODE_TYPE::classbody:
	case AA_AST_NODE_TYPE::enumbody:
	case AA_AST_NODE_TYPE::block: {
		AACType* r = AACType::ErrorType;
		for (size_t i = 0; i < node->expressions.size(); i++) {
			r = this->TypeCheckNode(node->expressions[i]);
			if (r == AACType::ErrorType) {
				printf("Detected error type (Ln %i Column %i)\n", node->expressions[i]->position.line, node->expressions[i]->position.column);
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
		if (node->content.compare(L".") == 0) {
			AACType* leftType = this->TypeCheckNode(node->expressions[0]);
			if (leftType->isRefType) {
				return this->TypeCheckClassDotFieldAccessorOperation(node, leftType, node->expressions[0], node->expressions[1]);
			} else if (leftType->isTupleType) {
				return this->TypeCheckTupleAccessorOperation(node, leftType, node->expressions[0], node->expressions[1]);
			}
		} else {
			break;
		}
	case AA_AST_NODE_TYPE::memberaccess:
		return this->TypeCheckMemberAccessorOperation(node, node->expressions[0], node->expressions[1]);
	case AA_AST_NODE_TYPE::enumdecleration: {

		// Store a copy of our variable type environment
		AAVarTypeEnv typeEnv = AAVarTypeEnv(m_vtenv);

		// Find our type
		AACType* enumType = this->FindType(node->content);

		// Get the enum signature from type
		AACEnumSignature* enumSig = enumType->enumSignature;

		// For all enums
		enumSig->values.ForEach([this, enumType](AACEnumValue& enumVal) { m_vtenv[enumVal.name] = enumType; });

		// Typecheck enum body (not the values, which is the first expression)
		AACType* bodyType = this->TypeCheckNode(node->expressions[AA_NODE_ENUMNODE_BODY]);

		// Restore type environment
		m_vtenv = typeEnv;

		// Did the body throw an error?
		if (bodyType == AACType::ErrorType) {
			return bodyType;
		}

		// Return a void (a decleration doesn't return anything in this context)
		return AACType::Void;

	}		
	case AA_AST_NODE_TYPE::classdecl:

		// Make sure there's a valid class body (a body node and that node actually contains some content)
		if (AA_NODE_CLASSNODE_BODY < node->expressions.size() && node->expressions[AA_NODE_CLASSNODE_BODY]->expressions.size() > 0) {
			
			// Copy the variable type environment
			AAVarTypeEnv vtenv = AAVarTypeEnv(m_vtenv);
			
			// Save a definition of 'this' to point to the typeof of the class currently edited
			m_vtenv[L"this"] = FindType(node->content);
			
			// Typecheck body
			AACType* bodyType = this->TypeCheckNode(node->expressions[AA_NODE_CLASSNODE_BODY]);
			
			// Restore variable environment
			m_vtenv = vtenv;

			// Return body type
			return bodyType;

		} else {
			return AACType::Void; // Merely a declaration => No specific type to return
		}

	case AA_AST_NODE_TYPE::fundecl: 
		return this->TypeCheckFuncDecl(node);
	case AA_AST_NODE_TYPE::funcall:
		return this->TypeCheckCallOperation(node);
	case AA_AST_NODE_TYPE::newstatement:
		return this->TypeCheckNewStatement(node);
	case AA_AST_NODE_TYPE::classctorcall:
		return this->TypeCheckClassCtorCall(node);
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
			AATC_W_ERROR("Undefined type identifier '" + string_cast(node->content) + "'", node->position, aa::compiler_err::C_Invalid_Type);
		} else {
			return t;
		}
	}
	case AA_AST_NODE_TYPE::tupletypeidentifier: {
		std::wstring formalTuple = aa::type::FormalizeTuple(node, m_typeMappingLambda);
		return this->m_dynamicTypeEnvironment->FindType(formalTuple, m_typeMappingLambda);
	}
	case AA_AST_NODE_TYPE::tupleval: {
		aa::list<AACType*> alltypes;
		for (auto& sub : node->expressions) {
			AACType* pTupleValType = this->TypeCheckNode(sub);
			if (pTupleValType != AACType::ErrorType) {
				sub->tags["primitive"] = (int)aa::runtime::runtimetype_from_statictype(pTupleValType);
				alltypes.Add(pTupleValType);
			} else {
				// TODO: Error
			}
		}
		return this->m_dynamicTypeEnvironment->FindType(aa::type::FormalizeTuple(alltypes), m_typeMappingLambda); // TODO: Add if not found
	}
	case AA_AST_NODE_TYPE::funarg: {
		if (node->expressions[0]->type == AA_AST_NODE_TYPE::typeidentifier) {
			AACType* t = this->FindType(node->expressions[0]->content);
			if (t == AACType::ErrorType) {
				AATC_W_ERROR("Undefined type  '" + string_cast(node->expressions[0]->content) + "' in argument", node->position, aa::compiler_err::C_Invalid_Type);
			} else {
				return t;
			}
		} else if (node->expressions[0]->type == AA_AST_NODE_TYPE::tupletypeidentifier) {
			return this->TypeCheckNode(node->expressions[0]);
		} else {
			printf("[AATypeChecker.Cpp@%i] Undefined node type in function argument", __LINE__);
			break;
		}
	}
	case AA_AST_NODE_TYPE::ifstatement:
		return this->TypeCheckConditionalBlock(node);
	case AA_AST_NODE_TYPE::elsestatement:
		return this->TypeCheckNode(node->expressions[0]);
	case AA_AST_NODE_TYPE::vardecl: {
		AACType* t = this->FindType(node->expressions[0]->content);
		if (t == AACType::ErrorType) {
			AATC_W_ERROR("Undefined type '" + string_cast(node->expressions[0]->content) + "'", node->position, aa::compiler_err::C_Invalid_Type);
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
	case AA_AST_NODE_TYPE::matchstatement:
		return this->TypeCheckPatternMatchBlock(node);
	case AA_AST_NODE_TYPE::forstatement:
		return this->TypeCheckForStatement(node);
	case AA_AST_NODE_TYPE::whilestatement:
	case AA_AST_NODE_TYPE::dowhilestatement:
		return this->TypeCheckWhileStatement(node);
	default:
		break;
	}

	return AACType::ErrorType;

}

AACType* AATypeChecker::TypeCheckBinaryOperation(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right) {

	// Is it a declerative assignment operation?
	if (left->type == AA_AST_NODE_TYPE::vardecl) {
		
		// Invoke the binary var decl typecheck;
		return this->TypeCheckBinaryVarDecl(pOpNode, left, right);

	} else if (left->type == AA_AST_NODE_TYPE::tuplevardecl) {
		
		// Invoke the binary tuple decl typecheck
		return this->TypeCheckBinaryTupleDecl(pOpNode, left, right);

	} else if (left->type == AA_AST_NODE_TYPE::index) {

		// Invoke the binary indexing typecheck
		return this->TypeCheckBinaryIndex(pOpNode, left, right);

	} else {

		AACType* typeLeft = this->TypeCheckNode(left);
		AACType* typeRight = this->TypeCheckNode(right);
		
		if (pOpNode->content.compare(L"=") == 0) { // Assignment (Not overloadable)

			return this->TypeCheckBinaryAssignment(pOpNode, left, right, typeLeft, typeRight);

		} else if (aa::runtime::is_primitive_type(typeLeft) && aa::runtime::is_primitive_type(typeRight)) {
			
			// Set the primitive types here
			left->tags["primitive"] = (int)aa::runtime::runtimetype_from_statictype(typeLeft);
			right->tags["primitive"] = (int)aa::runtime::runtimetype_from_statictype(typeRight);

			if (left->tags["primitive"] == (int)AAPrimitiveType::refptr) {

				return this->TypeCheckBinaryOperatorCall(pOpNode, left, right, typeLeft, typeRight);

			} else {
			
				return this->TypeCheckBinaryOperatorOp(pOpNode, left, right, typeLeft, typeRight);

			}

		} else {

			return AACType::ErrorType; // something went wrong...

		}

	}

}

AACType* AATypeChecker::TypeCheckBinaryAssignment(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right, AACType* typeLeft, AACType* typeRight) {

	// Is legal assignment?
	if (IsMatchingTypes(typeRight, typeLeft)) {

		// Set the primitive type
		pOpNode->tags["primitive"] = (int)aa::runtime::runtimetype_from_statictype(typeLeft);

		return typeLeft;
	} else {
		AATC_W_ERROR(
			"Type mismsatch on assignment operation, left operand '"
			+ string_cast(typeLeft->GetFullname()) + "' cannot be assigned to right operand type: '" + string_cast(typeRight->GetFullname()) + "'",
			pOpNode->position,
			aa::compiler_err::C_Mismatching_Types
		);
	}


}

AACType* AATypeChecker::TypeCheckBinaryOperatorCall(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right, AACType* typeLeft, AACType* typeRight) {


	std::wstring op = pOpNode->content;
	AAClassSignature* ccLeft = FindCompiledClassOfType(typeLeft);
	if (ccLeft->name == AACType::ErrorType->name) {
		/*this->SetError(
			AATypeChecker::Error(
				"Unknown operand type '" + string_cast(typeLeft->GetFullname()) + "'",
				__COUNTER__, pOpNode->position)
		);
		return AACType::ErrorType;/*/ return typeLeft; // TODO: Reenable this when array checking has been improved AND class field checking is possible
	}

	AAClassOperatorSignature ccop;
	if (this->FindCompiledClassOperation(ccLeft, op, typeRight, ccop)) {

		// Tag the operator node with the procID
		pOpNode->tags["useCall"] = true;
		pOpNode->tags["operatorProcID"] = ccop.method->procID;
		pOpNode->tags["operatorIsVM"] = ccop.method->isVMFunc;

		if (ccop.method->parameters.back().type == AACType::Any) {
			right->tags["wrap"] = (int)aa::runtime::runtimetype_from_statictype(typeRight);
		}

		// Return the type of whatever the operator will return
		return ccop.method->returnType;

	} else if (op.compare(L"==") == 0) {
		return AACTypeDef::Bool; // Comparrison operator => boolean result (not overloaded, thus we'll just compare pointers)
	} else {
		AATC_W_ERROR(
			"Invalid operator '" + string_cast(op) + "' on left operand type '" + string_cast(typeLeft->GetFullname())
			+ "' and right operand type '" + string_cast(typeRight->GetFullname()) + "'",
			pOpNode->position,
			aa::compiler_err::C_Invalid_Binary_Operator
		);
	}


}

AACType* AATypeChecker::TypeCheckBinaryOperatorOp(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right, AACType* typeLeft, AACType* typeRight) {


	// should be checking directly on the nodes if possible...
	AACType* resultType = this->TypeCheckBinaryOperationOnPrimitive(pOpNode, typeLeft, typeRight);
	pOpNode->tags["primitive"] = left->tags["primitive"];
	pOpNode->tags["useCall"] = false;

	/*if (resultType == AACType::ErrorType) {
	AATC_W_ERROR(
		"Type mismsatch on binary operation '" + string_cast(pOpNode->content) + "', left operand: '"
		+ string_cast(typeLeft->GetFullname()) + "' and right operand: '" + string_cast(typeRight->GetFullname()) + "'",
		pOpNode->position,
		aa::compiler_err::C_Mismatching_Types
	);
	} else {*/

	return resultType; // Reenable once typechecking on field access and such has been implemented

	//}

}

AACType* AATypeChecker::TypeCheckBinaryOperationOnPrimitive(AA_AST_NODE* pOpNode, AACType* typeLeft, AACType* typeRight) {

	if (typeLeft != typeRight) {
		//printf("Incorrect type!"); // TODO: Implement this when function calls have been implemented
	}

	if (pOpNode->content.compare(L"<") == 0 || pOpNode->content.compare(L"<=") == 0 || pOpNode->content.compare(L">") == 0 || 
		pOpNode->content.compare(L">=") == 0 || pOpNode->content.compare(L"==") == 0) {
		return AACTypeDef::Bool; // Result of such binary operations will always be a boolean
	}

	// Return thte lhs type (is equal to rhs)
	return typeLeft;

}

AACType* AATypeChecker::TypeCheckBinaryVarDecl(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right) {

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

		// Set the primitive type
		pOpNode->tags["primitive"] = (int)aa::runtime::runtimetype_from_statictype(typeLeft);

		// return the left type
		return typeLeft;

	} else {

		// Set error message
		AATC_W_ERROR(
			"Type mismsatch on binary operation '" + string_cast(pOpNode->content) + "', left operand: '"
			+ string_cast(typeLeft->GetFullname()) + "' and right operand: '" + string_cast(typeRight->GetFullname()) + "'",
			pOpNode->position,
			aa::compiler_err::C_Mismatching_Types
		);

	}

}

AACType* AATypeChecker::TypeCheckBinaryIndex(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right) {

	// Typecheck left and right
	AACType* typeLeft = this->TypeCheckNode(left);
	AACType* typeRight = this->TypeCheckNode(right);

	// Is it a legal operation (We may have to do more checks here in the future...
	if (IsMatchingTypes(typeRight, typeLeft->GetEncapsulatedType())) {
		return typeLeft;
	} else {
		AATC_W_ERROR(
			"Type mismsatch on binary operation '" + string_cast(pOpNode->content) + "', left operand: '"
			+ string_cast(typeLeft->GetFullname()) + "' and right operand: '" + string_cast(typeRight->GetFullname()) + "'",
			pOpNode->position,
			aa::compiler_err::C_Mismatching_Types
		);
	}

}

AACType* AATypeChecker::TypeCheckBinaryTupleDecl(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right) {

	// Formalize the tuple name
	std::wstring tupleFormalTypename = aa::type::FormalizeTuple(left, m_typeMappingLambda);

	// Find the left type (in case it was declared before)
	AACType* leftType = this->m_dynamicTypeEnvironment->FindType(tupleFormalTypename, m_typeMappingLambda);

	// Register the tuple if it was not found
	if (leftType == AACType::ErrorType) {
		leftType = this->m_dynamicTypeEnvironment->AddType(tupleFormalTypename, m_typeMappingLambda); // Register the type
	}

	// Set the type of the left side
	m_vtenv[left->content] = leftType;

	// Typecheck right side
	AACType* rightType = this->TypeCheckNode(right);

	// Set the primitive type
	pOpNode->tags["primitive"] = (int)aa::runtime::runtimetype_from_statictype(leftType);

	// Matching types?
	if (rightType != AACType::ErrorType && leftType == rightType) {
		return AACType::Void; // Assignment always returns void
	} else {
		return AACType::ErrorType;
	}

}

AACType* AATypeChecker::TypeCheckUnaryOperation(AA_AST_NODE* pOpNode, AA_AST_NODE* right) {
	AACType* pType = this->TypeCheckNode(right);
	if (pType != AACType::ErrorType) {

		if (aa::runtime::is_primitive_type(pType)) {

			// TODO: Verify

			pOpNode->tags["primitive"] = (int)aa::runtime::runtimetype_from_statictype(pType);

		} else {
			// potentially something else
		}

		return pType;

	} else {
		AATC_W_ERROR(
			"Undefined type: " + string_cast(right->content),
			pOpNode->position,
			aa::compiler_err::C_Undefined_Type
		);
	}
}

AACType* AATypeChecker::TypeCheckConditionalBlock(AA_AST_NODE* pConditionalNode) {

	// Condition verification variables
	AACType* conditionType = AACType::ErrorType;
	AA_AST_NODE* pConditionNode = pConditionalNode->expressions[0];

	// Verify condition
	for (size_t i = 0; i < pConditionNode->expressions.size(); i++) {
		AACType* t = this->TypeCheckNode(pConditionNode->expressions[i]);
		if (t == AACTypeDef::Bool || this->IsNumericType(t)) {
			conditionType = AACTypeDef::Bool;
		} else {
			conditionType = t; // and will not pass
			break;
		}
	}

	// Make sure it's valid a valid condition type
	if (conditionType != AACTypeDef::Bool) {
		if (conditionType != AACType::ErrorType) {
			AATC_ERROR(
				"Invalid evaluation of condition type. Type of '" + string_cast(conditionType->GetFullname()) + "' is an invalid conditional type.",
				pConditionalNode->position,
				aa::compiler_err::C_Invalid_Condition_Type
			);
		} else {
			AATC_W_ERROR(
				"Undefined type in condition",
				pConditionalNode->position,
				aa::compiler_err::C_Undefined_Type
			);
		}
	}

	// Typecheck other statements in block (Only done in if-statement, this section is ignored by other else and elseif statements)
	for (size_t i = 1; i < pConditionalNode->expressions.size(); i++) {
		if (pConditionalNode->expressions[i]->type == AA_AST_NODE_TYPE::elseifstatement) {
			if (this->TypeCheckConditionalBlock(pConditionalNode->expressions[i]) == AACType::ErrorType) {
				return AACType::ErrorType;
			}
		} else {
			// Typecheck body (Don't care about the body return type, unless it's an error type)
			if (this->TypeCheckNode(pConditionalNode->expressions[i]) == AACType::ErrorType) {
				return AACType::ErrorType;
			}
		}
	}

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

AACType* AATypeChecker::TypeCheckClassDotFieldAccessorOperation(AA_AST_NODE* pAccessorNode, AACType* pLeftType, AA_AST_NODE* left, AA_AST_NODE* right) {

	// Get class from lhs
	AAClassSignature* cc = this->FindCompiledClassOfType(pLeftType);

	// Make sure we got a valid class from this
	if (cc->name == AACType::ErrorType->name) {
		// Push error
		return AACType::ErrorType;
	}

	int field;
	if (AAClassCompiler::HasField(cc, right->content, field)) {

		// Get the field type
		AACType* pFieldType = cc->fields.Apply(field).type;

		// Save the ID of the field we're referencing
		right->tags["fieldid"] = field;
		right->tags["primitive"] = (int)aa::runtime::runtimetype_from_statictype(pFieldType);

		// Return field type
		return pFieldType;

	} else {

		// Push error (invalid field name)

		return AACType::ErrorType;

	}

}

AACType* AATypeChecker::TypeCheckTupleAccessorOperation(AA_AST_NODE* pAccessorNode, AACType* pLeftType, AA_AST_NODE* left, AA_AST_NODE* right) {

	// Make sure right element is a field
	if (right->type == AA_AST_NODE_TYPE::field) {

		// Get index of field
		unsigned int field = aa::wstring_trail(right->content);

		// Make sure index is inside the bounds of the tuple
		if (field >= 1 && field <= pLeftType->encapsulatedTypes.Size()) {

			// Update node type to tupleaccess
			pAccessorNode->type = AA_AST_NODE_TYPE::tupleaccess;

			// Get the tuple type
			AACType* pTupleFieldType = pLeftType->encapsulatedTypes.At(field - 1);

			// Tag some stuff
			pAccessorNode->tags["primitive"] = (int)aa::runtime::runtimetype_from_statictype(pTupleFieldType);
			pAccessorNode->tags["tupleindex"] = field - 1;

			// Return the type found at that index
			return pTupleFieldType;

		} else {

			// Tuple index is out of bounds
			AATC_W_ERROR(
				"Tuple value index '" + std::to_string(field) + "' is outside tuple bounds",
				right->position,
				aa::compiler_err::C_Invalid_Tuple_Index
			);

		}

	} else {
		printf("[AATypeChecker.Cpp@%i] Unexpected node type!", __LINE__);
	}

	return AACType::ErrorType;

}

AACType* AATypeChecker::TypeCheckCallOperation(AA_AST_NODE* pCallNode) {

	// Find the first AAFuncSignature matching our conditions
	aa::set<AAFuncSignature*> sigs = m_senv->availableFunctions.FindAll([pCallNode](AAFuncSignature*& sig) { return sig->name == pCallNode->content; });

	// Make sure we have functions to work with
	if (sigs.Size() > 0) {

		// Find best matching function from this
		AAFuncSignature* sig = sigs.FindFirst([this, pCallNode](AAFuncSignature*& sig) { return this->IsTypeMatchingFunction(sig, pCallNode); });

		// Make sure we found something
		if (sig != 0) {

			// Add some compiler data
			pCallNode->tags["calls"] = (int)sig->procID;
			pCallNode->tags["isVM"] = sig->isVMFunc;
			pCallNode->tags["args"] = (int)sig->parameters.size();
			pCallNode->tags["returns"] = (sig->returnType == AACType::Void) ? 0 : 1;

			// If the matching function is virtual
			if (aa::modifiers::ContainsFlag(sig->storageModifier, AAStorageModifier::VIRTUAL)) {
				pCallNode->tags["isVirtualCall"] = true;
			}

			// Return the type returned
			return sig->returnType;

		} else {

			// No function overload was found matching argument list
			AATC_W_ERROR(
				"No function '" + string_cast(pCallNode->content) + "' overload found matching argument list",
				pCallNode->position,
				aa::compiler_err::C_Undefined_Function_Overload
			);

		}

	} else {

		// The function does not even exist
		AATC_W_ERROR(
			"Attempt to call undefined function '" + string_cast(pCallNode->content) + "'",
			pCallNode->position,
			aa::compiler_err::C_Undefined_Function
		);

	}

}

AACType* AATypeChecker::TypeCheckClassCtorCall(AA_AST_NODE* pCallNode) {

	// Does the node contain a inheritance call tag => Void
	if (pCallNode->HasTag("inheritance_call")) {
		pCallNode->type = AA_AST_NODE_TYPE::funcall;
		return AACType::Void;
	}

	// Is there a local statement space defined?
	if (m_localStatementNamespace) {

		// The loc type
		AACType* pLocType = 0;

		// Run standard typecheck using local namespace, if that fails, try with current namespace
		if ((pLocType = this->TypeCheckCtorAndFindBestMatch(m_localStatementNamespace, pCallNode)) != AACType::ErrorType) {
			return pLocType;
		} else {
			pLocType = this->TypeCheckCtorAndFindBestMatch(m_currentnamespace, pCallNode);
			return pLocType;
		}

	} else {

		// Create the domain
		AACNamespace* tempDomain = m_currentnamespace->TemporaryDomain(m_senv->availableTypes, m_senv->availableClasses, m_senv->availableFunctions, m_senv->availableEnums);

		// Get type
		AACType* pType = this->TypeCheckCtorAndFindBestMatch(tempDomain, pCallNode);;

		// Get rid of temporary domain
		delete tempDomain;

		// Return found type
		return pType;

	}

	// Return the error type
	return AACType::ErrorType;

}

AACType* AATypeChecker::TypeCheckCtorAndFindBestMatch(AACNamespace* pDomain, AA_AST_NODE* pCallNode) {

	// Class index
	int cIndex;

	// Can we find the class in the namespace?
	if (pDomain->classes.FindFirstIndex([pCallNode](AAClassSignature*& sig) {
		return sig->name.compare(pCallNode->content) == 0;
		}, cIndex)) {

		// Class found
		AAClassSignature* classSig = pDomain->classes.Apply(cIndex);

		// Constructor name to look for
		std::wstring ctorname = pCallNode->content + L"::" + pCallNode->content;

		// All signatures where the function name is matching
		aa::set<AAFuncSignature*> sigs = pDomain->functions.FindAll([ctorname](AAFuncSignature*& sig) { return ctorname.compare(sig->name) == 0; });

		// Did we find any signatures?
		if (sigs.Size() > 0) {

			// Get the first type-matching signature
			AAFuncSignature* sig = sigs.FindFirst([this, pCallNode](AAFuncSignature*& sig) { return this->IsTypeMatchingFunction(sig, pCallNode); });

			// Is it valid?
			if (sig != 0) {

				// Set call ID
				pCallNode->tags["calls"] = (int)sig->procID;
				pCallNode->tags["isVM"] = sig->isVMFunc;
				pCallNode->tags["args"] = (int)sig->parameters.size();
				pCallNode->tags["allocsz"] = (int)classSig->classByteSz;
				pCallNode->tags["returns"] = (sig->returnType == AACType::Void) ? 0 : 1;

				// Return the proper type
				return sig->returnType;

			} else {

				// No function overload was found matching argument list
				AATC_W_ERROR(
					"No constructor '" + string_cast(pCallNode->content) + "' overload found matching argument list",
					pCallNode->position,
					aa::compiler_err::C_Undefined_Constructor_Overload
				);

			}

		} else {

			// The function does not even exist
			AATC_W_ERROR(
				"No constructor '" + string_cast(pCallNode->content) + "' overload found matching argument list",
				pCallNode->position,
				aa::compiler_err::C_Undefined_Constructor_Overload
			);

		}
	}

	// Error, not able to find the type in the local space ==> does absolutely not exist
	AATC_W_ERROR(
		"Undefined class type '" + string_cast(pCallNode->content) + "'",
		pCallNode->position,
		aa::compiler_err::C_Undefined_Class
	);

}

AACType* AATypeChecker::TypeCheckFuncDecl(AA_AST_NODE* pDeclNode) {

	// Do we have a body that also needs type verification/checking?
	if (aa::parsing::Function_HasBody(pDeclNode)) {
		
		// Make a copy of the current variable environment
		AAVarTypeEnv vtenv = AAVarTypeEnv(m_vtenv);

		// For all arguments
		for (size_t i = 0; i < pDeclNode->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions.size(); i++) {

			// Typecheck argument
			AACType* argType = this->TypeCheckNode(pDeclNode->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions[i]);

			// Set variable type in environment
			if (argType != AACType::ErrorType) {
				
				// Set type in variable environment
				m_vtenv[pDeclNode->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions[i]->content] = argType;

				// Set primittive type
				pDeclNode->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions[i]->tags["primitive"] = (int)aa::runtime::runtimetype_from_statictype(argType);

			} else {
				AATC_W_ERROR(
					"Undefined type  '" + string_cast(pDeclNode->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions[i]->content) + "' in argument", 
					pDeclNode->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions[i]->position, aa::compiler_err::C_Invalid_Type
				);
			}

		}

		// Type verify body
		AACType* pBodyType = this->TypeCheckNode(pDeclNode->expressions[AA_NODE_FUNNODE_BODY]);

		// Reset variable types
		m_vtenv = vtenv;

	}

	// Return the type the function returns
	AACType* returnType = this->TypeCheckNode(pDeclNode->expressions[AA_NODE_FUNNODE_RETURNTYPE]);

	// Return that
	return returnType;

}

AACType* AATypeChecker::TypeCheckNewStatement(AA_AST_NODE* pNewStatement) {

	if (pNewStatement->expressions[0]->type == AA_AST_NODE_TYPE::classctorcall) {
		AACType* type = this->TypeCheckNode(pNewStatement->expressions[0]);
		if (type != AACType::ErrorType) {
			pNewStatement->expressions[0]->tags["typeID"] = type->constantID;
			return type;
		}
	} else if (pNewStatement->expressions[0]->type == AA_AST_NODE_TYPE::index) {
		AACType* tp = this->FindType(pNewStatement->expressions[0]->expressions[0]->content + L"[]"); // Put an additional [] on, such that it picks up the correct type
		if (this->IsValidType(tp)) {

			pNewStatement->tags["primitive"] = (int)aa::runtime::runtimetype_from_statictype(tp->GetEncapsulatedType());

			return tp;
		}
	}

	AATC_W_ERROR("Undefined type '" + string_cast(pNewStatement->expressions[0]->content) + "' in new statement", pNewStatement->position, aa::compiler_err::C_Invalid_Type);

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

		std::wstring from = this->FlattenNamespacePath(pUseNode->expressions[0]);
		AACNamespace* domain = this->FindNamespaceFromFlattenedPath(m_currentnamespace, from);

		if (domain != NULL) {

			this->m_senv->availableClasses.UnionWith(domain->classes);
			this->m_senv->availableTypes.UnionWith(domain->types);
			this->m_senv->availableFunctions.UnionWith(domain->functions);
			this->m_senv->availableEnums.UnionWith(domain->enums);

		} else {
			AATC_ERROR("Undefined namespace '" + string_cast(from) + "' in using directive", pUseNode->position);
		}

		break;
	}
	case AA_AST_NODE_TYPE::usingspecificstatement: {
		
		std::wstring import = pUseNode->content;
		std::wstring from = this->FlattenNamespacePath(pUseNode->expressions[0]->expressions[0]);
		AACNamespace* domain = this->FindNamespaceFromFlattenedPath(m_currentnamespace, from);
		
		int i1;
		if (domain != NULL) {
			if (domain->classes.FindFirstIndex([import](AAClassSignature*& sig) { return sig->name.compare(import) == 0; }, i1)) {
				AAClassSignature* imported = domain->classes.Apply(i1);
				this->m_senv->availableTypes.Add(imported->type);
				this->m_senv->availableClasses.Add(imported);
				imported->methods.ForEach([this](AAFuncSignature*& sig) { this->m_senv->availableFunctions.Add(sig); });
			} else if (domain->enums.FindFirstIndex([import](AACEnumSignature*& sig) { return sig->name.compare(import) == 0; }, i1)) {
				AACEnumSignature* imported = domain->enums.Apply(i1);
				this->m_senv->availableTypes.Add(imported->type);
				this->m_senv->availableEnums.Add(imported);
				imported->functions.ForEach([this](AAFuncSignature*& sig) { this->m_senv->availableFunctions.Add(sig); });
			} else {
				AATC_ERROR("Undefined class '" + string_cast(import) + "' in namespace '" + string_cast(from) + "'", pUseNode->position);
			}
		} else {
			AATC_ERROR("Undefined namespace '" + string_cast(from) + "' in using directive", pUseNode->position);
		}
		break;
	}
	}

	// Will always return void
	return AACType::Void;

}

AACType* AATypeChecker::TypeCheckMemberAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right) {

	// Attempt 1: Find in namespace
	{

		// Get the flattened name and get the namespace from that
		std::wstring flattened = this->FlattenNamespacePath(left);
		AACNamespace* domain = this->FindNamespaceFromFlattenedPath(m_currentnamespace, flattened);

		// Make sure we got it, otherwise we failed this attempt
		if (domain != NULL) {

			// Attempt 1.1: Find class
			int classID;
			if (domain->classes.FindFirstIndex([right](AAClassSignature*& sig) { return right->content.compare(sig->name) == 0; }, classID)) {

				// Update the local namespace
				m_localStatementNamespace = domain;

				// Return type
				return domain->classes.Apply(classID)->type;

			}

		}

	}

	// Try and get type of LHS
	AACType* lhsType = this->FindType(left->content);

	// Did we find it?
	if (lhsType != AACType::ErrorType) {

		if (lhsType->isRefType) { // TODO: Check classes (attempt 2)

		} else if (lhsType->isEnum) { // Attempt 3: Find enum member

			if (lhsType->enumSignature->HasValue(right->content)) {
				pAccessorNode->type = AA_AST_NODE_TYPE::enumvalueaccess;
				right->tags["enumval"] = (int)lhsType->enumSignature->GetValue(right->content);
				return lhsType;
			} else {
				AATC_ERROR("Enum '" + string_cast(lhsType->GetFullname()) + "' has no member value '" + string_cast(right->content) + "'", pAccessorNode->position);
			}

		}

	}

	return AACType::ErrorType;

}

AACType* AATypeChecker::TypeCheckPatternMatchBlock(AA_AST_NODE* pMatchNode) {

	// Typecheck the target type
	AACType* matchOnType = this->TypeCheckNode(pMatchNode->expressions[0]);
	AACType* returnType = AACType::ErrorType;

	// For all cases
	for (size_t i = 0; i < pMatchNode->expressions[1]->expressions.size(); i++) {

		// Get the node
		AACType* caseType = this->TypeCheckPatternMatchCase(pMatchNode->expressions[1]->expressions[i], matchOnType);

		// Did we get an error type?
		if (caseType == AACType::ErrorType) {
			returnType = caseType;
			break;
		} else if (i == 0) { // If first case, we don't actually know what to check for, so set retun type to this
			returnType = caseType; 
		} else {
			if (!IsMatchingTypes(caseType, returnType)) { // Does it not match with previous type? => Set out type to any
				returnType = AACType::Any;
				printf("<Type Warning> Detected match case where result type is 'Any'"); // (Should be a compiler flag to allow/disallow this?)
			}
		}

	}

	// Did we not do anything (or did the veri first ase throw an error, in which case we don't want to override that)
	if (!m_hasEnyErr && returnType == AACType::ErrorType && pMatchNode->expressions[1]->expressions.size() == 0) {
		AATC_ERROR("Empty 'match' statement detected. This is not allowed!", pMatchNode->position);
	}

	return returnType;

}

AACType* AATypeChecker::TypeCheckPatternMatchCase(AA_AST_NODE* pCaseNode, AACType* pConditionType) {

	// Copy the current variable type environment (Incase the condition/body introduce new variables)
	AAVarTypeEnv vtenv = AAVarTypeEnv(m_vtenv);
	
	AACType* conditionType = this->TypeCheckPatternMatchCaseCondition(pCaseNode->expressions[0], pConditionType);
	if (!IsMatchingTypes(conditionType, pConditionType)) {
		AATC_ERROR(
			"Detected 'case' condition type mismsatch '" + string_cast(conditionType->GetFullname()) + "' cannot be matched with condition of type '" + 
			string_cast(pConditionType->GetFullname()) + "'",
			pCaseNode->position
		);
	} else {
		pCaseNode->expressions[0]->tags["primitive"] = (int)aa::runtime::runtimetype_from_statictype(conditionType);
	}

	// Typecheck body
	AACType* bodyType = this->TypeCheckNode(pCaseNode->expressions[1]);

	// Reset variable type environment
	m_vtenv = vtenv;

	// Return found body type
	return bodyType;

}

AACType* AATypeChecker::TypeCheckPatternMatchCaseCondition(AA_AST_NODE* pConditionNode, AACType* _conditionType) {

	AACType* conditionType = AACType::ErrorType;
	for (size_t i = 0; i < pConditionNode->expressions.size(); i++) {

		// Is it a function call?
		if (pConditionNode->expressions[i]->type == AA_AST_NODE_TYPE::funcall) {

			// Get potential candidates
			aa::set<AAFuncSignature*> candidates = m_currentnamespace->functions.FindAll(
				[pConditionNode, i](AAFuncSignature*& sig) {
					return sig->name.compare(pConditionNode->expressions[i]->content) == 0
						&& sig->parameters.size() == pConditionNode->expressions[i]->expressions.size();
				}
			);

			// Make sure there's only ONE candidate
			if (candidates.Size() == 1) {

				// We'll use the first
				AAFuncSignature* first = candidates.Apply(0);

				// For each argument in function call
				for (size_t j = 0; j < pConditionNode->expressions[i]->expressions.size(); j++) {

					if (pConditionNode->expressions[i]->expressions[j]->type == AA_AST_NODE_TYPE::variable) {
						if (pConditionNode->expressions[i]->expressions[j]->content.compare(L"_") != 0) {
							m_vtenv[pConditionNode->expressions[i]->expressions[j]->content] = first->parameters[j].type; // Add variable to type environment
						}
					} else {
						AATC_ERROR(
							"Cannot pattern match on non-variable in function call!",
							pConditionNode->expressions[i]->expressions[j]->position
						);
					}

				}

				pConditionNode->expressions[i]->tags["params"] = first->parameters.size();
				pConditionNode->expressions[i]->tags["isVM"] = first->isVMFunc;
				pConditionNode->expressions[i]->tags["procID"] = first->procID;

				// The condition type is then of that type
				return first->returnType;

			} else {
				AATC_ERROR(
					"Found no function in pattern match case",
					pConditionNode->expressions[i]->position
				);
			}

		} else if (pConditionNode->expressions[i]->type == AA_AST_NODE_TYPE::variable) {

			if (pConditionNode->expressions[i]->content.compare(L"_") == 0) { // The dummy
				return _conditionType; // No new variables or anything special to consider (But also not an error)
			} else {

				AACType* foundType = this->TypeCheckNode(pConditionNode->expressions[i]);
				if (foundType == AACType::ErrorType) {
					// throw error
					return foundType;
				} else if (foundType != AACType::Void) { // Found may come from if-statements and such
					conditionType = foundType;
					if (foundType->isEnum && pConditionNode->expressions[i]->type == AA_AST_NODE_TYPE::variable) {
						pConditionNode->expressions[i]->type = AA_AST_NODE_TYPE::enumidentifier;
						pConditionNode->expressions[i]->tags["enumval"] = (int)foundType->enumSignature->GetValue(pConditionNode->expressions[i]->content);
					}
				}

			}

		}

	}

	return conditionType;

}

AACType* AATypeChecker::TypeCheckForStatement(AA_AST_NODE* pForStatementNode) {

	// Copy the variable type environment
	AAVarTypeEnv vtenv = AAVarTypeEnv(m_vtenv);

	// Typecheck init
	if (this->TypeCheckNode(pForStatementNode->expressions[0]) == AACType::ErrorType) {
		return AACType::ErrorType;
	}

	// Typecheck condition and make sure it's a boolean
	if (this->TypeCheckNode(pForStatementNode->expressions[1]->expressions[0]) != AACTypeDef::Bool) {
		AATC_W_ERROR(
			"Expected boolean type in for condition",
			pForStatementNode->position,
			aa::compiler_err::C_Undefined_Constructor_Overload
		);
	}

	// Typecheck afterthought
	if (this->TypeCheckNode(pForStatementNode->expressions[2]) == AACType::ErrorType) {
		return AACType::ErrorType;
	}

	// Typecheck body
	if (this->TypeCheckNode(pForStatementNode->expressions[3]) == AACType::ErrorType) {
		return AACType::ErrorType;
	}

	// Restore variable type environment
	m_vtenv = vtenv;

	// We cannot return anything from a for-statement ==> Void
	return AACType::Void;

}

AACType* AATypeChecker::TypeCheckWhileStatement(AA_AST_NODE* pWhileStatementNode) {

	// Typecheck body failed?
	if (this->TypeCheckNode(pWhileStatementNode->expressions[1]) == AACType::ErrorType) {
		return AACType::ErrorType;
	}

	// Typecheck condition and make sure it's a boolean
	if (this->TypeCheckNode(pWhileStatementNode->expressions[0]->expressions[0]) != AACTypeDef::Bool) {
		AATC_W_ERROR(
			"Expected boolean type in while condition",
			pWhileStatementNode->position,
			aa::compiler_err::C_Undefined_Constructor_Overload
		);
	}

	// We cannot return anything from a while-statement ==> Void
	return AACType::Void;

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
		return t == AACType::Void || m_dynamicTypeEnvironment->IsValidDynamicType(t);
	} else {
		return true;
	}	
}

bool AATypeChecker::IsNumericType(AACType* t) {
	return t == AACTypeDef::Int32;
}

bool AATypeChecker::IsMatchingTypes(AACType* tCompare, AACType* tExpected) {

	if (tCompare == tExpected) {
		return true;
	} else {
		if (tExpected == AACType::Any) {
			return true;
		} else {
			if (tCompare->isRefType && tCompare->classSignature && tExpected->isRefType && tExpected->classSignature) { // Could it be a derivitve?
				if (tCompare->classSignature->DerivesFrom(tExpected->classSignature)) { // Check if tCompare is derived from the expected type (because then it's a legal match)
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			}
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

bool AATypeChecker::IsTypeMatchingFunction(AAFuncSignature* sig, AA_AST_NODE* pCallNode) {

	// Parameter offset
	size_t paramOffset = (sig->isClassMethod && !sig->isClassCtor) ? 1 : 0;
	size_t paramEndOffset = (sig->isClassCtor) ? 1 : 0;

	// Is this the correct function to call?
	if (pCallNode->expressions.size() == sig->parameters.size() - paramOffset - paramEndOffset) {

		// Flag to make sure the argument types are valid
		bool isMatchingArgTypes = true;

		// For all params
		for (size_t j = paramOffset; j < sig->parameters.size() - paramEndOffset; j++) {

			// Get the type
			AACType* argType = this->TypeCheckNode(pCallNode->expressions[j - paramOffset]);

			// If they're not a type match, break
			if (!this->IsMatchingTypes(argType, sig->parameters[j].type)) {
				isMatchingArgTypes = false;
				break;
			} else {

				// If any - we need to add a wrap
				if (sig->parameters[j].type == AACType::Any) {
					pCallNode->expressions[j - paramOffset]->tags["wrap"] = (int)aa::runtime::runtimetype_from_statictype(argType);
				}

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

	// Try and find the operator
	try {

		// Find the first operator matching the condition
		op = cc->operators.FindFirst([operatorType, right, this](AAClassOperatorSignature& sig) { return sig.op.compare(operatorType) == 0 && IsMatchingTypes(right, sig.method->parameters[1].type); });

	} catch (std::exception exec) {
		return false;
	}

	// Return true if the operator is not a default operator
	return !(op == AAClassOperatorSignature());

}

std::wstring AATypeChecker::FlattenNamespacePath(AA_AST_NODE* pNode) {

	if (pNode->type == AA_AST_NODE_TYPE::memberaccess) {
		return this->FlattenNamespacePath(pNode->expressions[0]) + L"::" + this->FlattenNamespacePath(pNode->expressions[1]);
	} else {
		return pNode->content;
	}

}

AACNamespace* AATypeChecker::FindNamespaceFromFlattenedPath(AACNamespace* root, std::wstring path) {

	size_t pos;
	if ((pos = path.find(L"::")) != std::wstring::npos) {

		std::wstring name = path.substr(0, pos);
		std::wstring subpath = path.substr(pos + 2);

		int i;
		if (root->childspaces.FindFirstIndex([name](AACNamespace*& domain) { return domain->name.compare(name) == 0; }, i)) {
			return this->FindNamespaceFromFlattenedPath(root->childspaces.Apply(i), subpath);
		} else {
			return NULL;
		}

	} else {


		int i;
		if (root->childspaces.FindFirstIndex([path](AACNamespace*& domain) { return domain->name.compare(path) == 0; }, i)) {
			return root->childspaces.Apply(i);
		} else {
			return NULL;
		}

	}

}
