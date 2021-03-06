#include "AA_AST.h"
#include "AA_AST_Expander.h"
#include "astring.h"

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
	case AA_PT_NODE_TYPE::compile_unit: {

		AA_AST_NODE* pCompileUnitNode = new AA_AST_NODE(L"compile_unit", AA_AST_NODE_TYPE::compile_unit, pNode->position);

		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			pCompileUnitNode->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}

		return pCompileUnitNode;

	}
	case AA_PT_NODE_TYPE::funcbody:
	case AA_PT_NODE_TYPE::classbody:
	case AA_PT_NODE_TYPE::enumbody:
	case AA_PT_NODE_TYPE::lambdabody:
	case AA_PT_NODE_TYPE::block: {

		AA_AST_NODE* blockNode = new AA_AST_NODE(L"{<block>}", this->GetASTBlockType(pNode->nodeType), pNode->position);

		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			blockNode->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}

		return blockNode;

	}
	case AA_PT_NODE_TYPE::keyword: {
		if (pNode->content.compare(L"this") == 0) {
			return new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::variable, pNode->position);
		}
		break;
	}
	case AA_PT_NODE_TYPE::binary_operation: {

		AA_AST_NODE* binaryNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::binop, pNode->position);

		binaryNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		binaryNode->expressions.push_back(this->AbstractNode(pNode->childNodes[1]));

		return binaryNode;

	}
	case AA_PT_NODE_TYPE::unary_operation_pre: {

		AA_AST_NODE* unaryNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::unop_pre, pNode->position);
		unaryNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));

		return unaryNode;

	}
	case AA_PT_NODE_TYPE::unary_operation_post: {
		
		AA_AST_NODE* unaryNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::unop_post, pNode->position);
		unaryNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));

		return unaryNode;
	
	}
	case AA_PT_NODE_TYPE::vardecleration: {
		
		// AST node to create
		AA_AST_NODE* vDecl = 0;

		if (pNode->childNodes[AA_NODE_VARDECL_TYPE]->content.compare(L"var") == 0) {

			// Create node
			vDecl = new AA_AST_NODE(pNode->childNodes[AA_NODE_VARDECL_IDENTIFIER]->content, AA_AST_NODE_TYPE::vardecl, pNode->position);

		} else if (pNode->childNodes[AA_NODE_VARDECL_TYPE]->nodeType == AA_PT_NODE_TYPE::lambdatype) {
		
			// Create node
			vDecl = new AA_AST_NODE(pNode->childNodes[AA_NODE_VARDECL_IDENTIFIER]->content, AA_AST_NODE_TYPE::lambdadecl, pNode->position);
			vDecl->expressions.push_back(this->AbstractNode(pNode->childNodes[AA_NODE_VARDECL_TYPE]));

		} else if (pNode->content.compare(L"tupledecl") == 0) {
			
			// Create tuple node
			vDecl = new AA_AST_NODE(pNode->childNodes[AA_NODE_VARDECL_IDENTIFIER]->content, AA_AST_NODE_TYPE::tuplevardecl, pNode->position);

			// Add tuple's internal types
			for (auto& type : pNode->childNodes[AA_NODE_VARDECL_TYPE]->childNodes) {
				vDecl->expressions.push_back(new AA_AST_NODE(type->content, AA_AST_NODE_TYPE::typeidentifier, type->position));
			}

		} else { // When a specific type is specified
			
			// Create node
			vDecl = new AA_AST_NODE(pNode->childNodes[AA_NODE_VARDECL_IDENTIFIER]->content, AA_AST_NODE_TYPE::vardecl, pNode->position);
			
			// Sort out type
			if (pNode->childNodes[AA_NODE_VARDECL_TYPE]->nodeType == AA_PT_NODE_TYPE::accessor) { // However, it could also be a member accessor (ie. we're trying to access a type in a namespace or a class subtype)
				AA_AST_NODE* accessNode = this->AbstractNode(pNode->childNodes[AA_NODE_VARDECL_TYPE]);
				vDecl->expressions.push_back(accessNode);
			} else { // default is of course a type identifier
				vDecl->expressions.push_back(new AA_AST_NODE(pNode->childNodes[AA_NODE_VARDECL_TYPE]->content, AA_AST_NODE_TYPE::typeidentifier, pNode->position));
			}
		
		}
		
		// Transfer flags (if any)
		vDecl->tags = pNode->flags;

		// If any modifiers, add those
		if (aa::parsing::Var_HasModifiers(pNode)) {
			for (auto& modNode : pNode->childNodes[AA_NODE_VARDECL_MODIFIERLIST]->childNodes) {
				vDecl->tags["modifier_" + string_cast(modNode->content)] = 1;
			}
		}

		// Return vdecl
		return vDecl;

	}
	case AA_PT_NODE_TYPE::fundecleration: {
		
		// Create function declaration
		AA_AST_NODE* funDecl = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::fundecl, pNode->position);

		// Convert return type
		if (pNode->childNodes[AA_NODE_FUNNODE_RETURNTYPE]->nodeType == AA_PT_NODE_TYPE::tupleval) {
			AA_AST_NODE* tupleIdentifier = new AA_AST_NODE(L"(<...>)", AA_AST_NODE_TYPE::tupletypeidentifier, pNode->position);
			for (size_t i = 0; i < pNode->childNodes[AA_NODE_FUNNODE_RETURNTYPE]->childNodes.size(); i++) {
				tupleIdentifier->expressions.push_back(new AA_AST_NODE( // Add type to tuple type
					pNode->childNodes[AA_NODE_FUNNODE_RETURNTYPE]->childNodes[i]->content, 
					AA_AST_NODE_TYPE::typeidentifier, 
					pNode->childNodes[AA_NODE_FUNNODE_RETURNTYPE]->childNodes[i]->position)
				);
			}
			funDecl->expressions.push_back(tupleIdentifier);
		} else {
			funDecl->expressions.push_back(new AA_AST_NODE(pNode->childNodes[AA_NODE_FUNNODE_RETURNTYPE]->content, AA_AST_NODE_TYPE::typeidentifier, pNode->position));
		}

		// Convert arguments and modifiers
		funDecl->expressions.push_back(this->AbstractNode(pNode->childNodes[AA_NODE_FUNNODE_ARGLIST]));
		funDecl->expressions.push_back(this->AbstractNode(pNode->childNodes[AA_NODE_FUNNODE_MODIFIER]));

		// Convert the body (if any)
		if (aa::parsing::Function_HasBody(pNode)) {
			funDecl->expressions.push_back(this->AbstractNode(pNode->childNodes[AA_NODE_FUNNODE_BODY]));
		} else {
			// print stuff ...
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
		if (pNode->childNodes[0]->nodeType == AA_PT_NODE_TYPE::tupleval) {
			AA_AST_NODE* tupleIdentifier = new AA_AST_NODE(L"(<...>)", AA_AST_NODE_TYPE::tupletypeidentifier, pNode->position);
			for (size_t i = 0; i < pNode->childNodes[0]->childNodes.size(); i++) {
				tupleIdentifier->expressions.push_back(new AA_AST_NODE( // Add type to tuple type
					pNode->childNodes[0]->childNodes[i]->content,
					AA_AST_NODE_TYPE::typeidentifier,
					pNode->childNodes[0]->childNodes[i]->position)
				);
			}
			arg->expressions.push_back(tupleIdentifier);
		} else {
			arg->expressions.push_back(new AA_AST_NODE(pNode->childNodes[0]->content, AA_AST_NODE_TYPE::typeidentifier, pNode->childNodes[0]->position));
		}
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
		forstatement->expressions.push_back(this->AbstractNode(pNode->childNodes[0])); // init
		forstatement->expressions.push_back(this->AbstractNode(pNode->childNodes[1])); // condition
		forstatement->expressions.push_back(this->AbstractNode(pNode->childNodes[2])); // afterthought
		forstatement->expressions.push_back(this->AbstractNode(pNode->childNodes[3])); // body
		return forstatement;
	}
	case AA_PT_NODE_TYPE::forinit: {
		AA_AST_NODE* con = new AA_AST_NODE(L"init", AA_AST_NODE_TYPE::forinit, pNode->position);
		con->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		return con;
	}
	case AA_PT_NODE_TYPE::forafterthought: {
		AA_AST_NODE* con = new AA_AST_NODE(L"afterthought", AA_AST_NODE_TYPE::forafterthought, pNode->position);
		con->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		return con;
	}
	case AA_PT_NODE_TYPE::whilestatement: {
		AA_AST_NODE* whilestatement = new AA_AST_NODE(L"while", AA_AST_NODE_TYPE::whilestatement, pNode->position);
		whilestatement->expressions.push_back(this->AbstractNode(pNode->childNodes[0])); // condition
		whilestatement->expressions.push_back(this->AbstractNode(pNode->childNodes[1])); // body
		return whilestatement;
	}
	case AA_PT_NODE_TYPE::dowhilestatement: {
		AA_AST_NODE* dowhilestatement = new AA_AST_NODE(L"do-while", AA_AST_NODE_TYPE::dowhilestatement, pNode->position);
		dowhilestatement->expressions.push_back(this->AbstractNode(pNode->childNodes[0])); // condition
		dowhilestatement->expressions.push_back(this->AbstractNode(pNode->childNodes[1])); // body
		return dowhilestatement;
	}
	case AA_PT_NODE_TYPE::classdecleration: {
		AA_AST_NODE* classdef = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::classdecl, pNode->position);
		classdef->expressions.push_back(this->AbstractNode(pNode->childNodes[AA_NODE_CLASSNODE_MODIFIER]));
		classdef->expressions.push_back(this->AbstractNode(pNode->childNodes[AA_NODE_CLASSNODE_INHERITANCE]));
		if (AA_NODE_CLASSNODE_BODY < pNode->childNodes.size()) {
			classdef->expressions.push_back(this->AbstractNode(pNode->childNodes[AA_NODE_CLASSNODE_BODY]));
		} else {
			if (!IsClassBodyOmmisionAllowed(pNode)) {
				this->SetError("Class body expected in class definition", 0, pNode->position);
			}
		}
		return classdef;
	}
	case AA_PT_NODE_TYPE::newstatement: {
		AA_AST_NODE* newkw = new AA_AST_NODE(L"new", AA_AST_NODE_TYPE::newstatement, pNode->position);
		if (pNode->childNodes[0]->nodeType == AA_PT_NODE_TYPE::funccall) {
			AA_AST_NODE* ctorNode = this->AbstractNode(pNode->childNodes[0]);
			ctorNode->type = AA_AST_NODE_TYPE::classctorcall;
			newkw->expressions.push_back(ctorNode);
		} else if (pNode->childNodes[0]->nodeType == AA_PT_NODE_TYPE::indexat) {
			newkw->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		} else {
			printf("Undefined (new) operation @%i", __LINE__);
		}
		return newkw;
	}
	case AA_PT_NODE_TYPE::accessor: {
		AA_AST_NODE_TYPE nType = this->GetASTAccessType(pNode);
		AA_AST_NODE* accessorNode = new AA_AST_NODE(pNode->content, nType, pNode->position);
		accessorNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		accessorNode->expressions.push_back(this->AbstractNode(pNode->childNodes[1]));
		if (nType == AA_AST_NODE_TYPE::fieldaccess) { // TODO: Check if it's a possible function access instead (Might have to do that in the typechecker where more information is available)
			accessorNode->expressions[1]->type = AA_AST_NODE_TYPE::field;
		} else if (nType == AA_AST_NODE_TYPE::memberaccess) {
			accessorNode->expressions[1]->type = AA_AST_NODE_TYPE::typeidentifier;
		}
		return accessorNode;
	}
	case AA_PT_NODE_TYPE::indexat: {
		AA_AST_NODE* indexatNode = new AA_AST_NODE(L"", AA_AST_NODE_TYPE::index, pNode->position);
		indexatNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		indexatNode->expressions.push_back(this->AbstractNode(pNode->childNodes[1]));
		return indexatNode;
	}
	case AA_PT_NODE_TYPE::namespacedecleration: {
		AA_AST_NODE* namespacedeclNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::name_space, pNode->position);
		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			namespacedeclNode->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}
		return namespacedeclNode;
	}
	case AA_PT_NODE_TYPE::usingstatement: {
		if (pNode->childNodes.size() == 1 && pNode->childNodes[0]->nodeType != AA_PT_NODE_TYPE::fromstatement) {
			AA_AST_NODE* useNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::usingstatement, pNode->position);
			useNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0])); // Actual name of namespace to use from (incase it's an accessor)
			return useNode;
		} else if (pNode->childNodes.size() == 1 && pNode->childNodes[0]->nodeType == AA_PT_NODE_TYPE::fromstatement) {
			AA_AST_NODE* usefromNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::usingspecificstatement, pNode->position);
			AA_AST_NODE* fromNode = new AA_AST_NODE(pNode->childNodes[0]->content, AA_AST_NODE_TYPE::name_space, pNode->position);
			fromNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0]->childNodes[0])); // Add actual name of namespace to import from
			usefromNode->expressions.push_back(fromNode);
			return usefromNode;
		} else {
			throw std::exception("FIX THIS!");
			break;
		}
	}
	case AA_PT_NODE_TYPE::enumdecleration: {
		AA_AST_NODE* enumDeclNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::enumdecleration, pNode->position);
		enumDeclNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		enumDeclNode->expressions.push_back(this->AbstractNode(pNode->childNodes[1]));
		return enumDeclNode;
	}
	case AA_PT_NODE_TYPE::enumvallist: {
		AA_AST_NODE* pEnumValListNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::enumvallist, pNode->position);
		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			pEnumValListNode->expressions.push_back(new AA_AST_NODE(pNode->childNodes[i]->content, AA_AST_NODE_TYPE::enumidentifier, pNode->childNodes[i]->position));
		}
		return pEnumValListNode;
	}
	case AA_PT_NODE_TYPE::matchcaselist: {
		AA_AST_NODE* pCaseList = new AA_AST_NODE(L"", AA_AST_NODE_TYPE::matchcaselist, pNode->position);
		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			pCaseList->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}
		return pCaseList;
	} 
	case AA_PT_NODE_TYPE::matchcasestatement: {
		AA_AST_NODE* pMatchStatement = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::matchcasestatement, pNode->position);
		pMatchStatement->expressions.push_back(this->AbstractNode(pNode->childNodes[0]));
		pMatchStatement->expressions.push_back(this->AbstractNode(pNode->childNodes[1]));
		return pMatchStatement;
	}
	case AA_PT_NODE_TYPE::matchstatement: {
		AA_AST_NODE* pMatchNode = new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::matchstatement, pNode->position);
		pMatchNode->expressions.push_back(this->AbstractNode(pNode->childNodes[0])); // match target
		pMatchNode->expressions.push_back(this->AbstractNode(pNode->childNodes[1])); // match cases
		return pMatchNode;
	}
	case AA_PT_NODE_TYPE::classinheritancelist: {
		AA_AST_NODE* pInheritanceListNode = new AA_AST_NODE(L"", AA_AST_NODE_TYPE::classinheritancelist, pNode->position);
		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			pInheritanceListNode->expressions.push_back(new AA_AST_NODE(pNode->childNodes[i]->content, AA_AST_NODE_TYPE::typeidentifier, pNode->childNodes[i]->position));
		}
		return pInheritanceListNode;
	}
	case AA_PT_NODE_TYPE::modifierlist: {
		AA_AST_NODE* pModifierListNode = new AA_AST_NODE(L"", AA_AST_NODE_TYPE::modifierlist, pNode->position);
		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			pModifierListNode->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}
		return pModifierListNode;
	}
	case AA_PT_NODE_TYPE::modifier:
		return new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::modifier, pNode->position);
	case AA_PT_NODE_TYPE::tupleval: {
		AA_AST_NODE* pTupleValNode = new AA_AST_NODE(L"", AA_AST_NODE_TYPE::tupleval, pNode->position);
		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			pTupleValNode->expressions.push_back(this->AbstractNode(pNode->childNodes[i]));
		}
		return pTupleValNode;
	}
	case AA_PT_NODE_TYPE::lambdatype: {

		AA_AST_NODE* lambdaType = new AA_AST_NODE(L"", AA_AST_NODE_TYPE::lambdatype, pNode->position);
		for (size_t i = 0; i < pNode->childNodes.size(); i++) {
			lambdaType->expressions.push_back(new AA_AST_NODE(pNode->childNodes[i]->content, AA_AST_NODE_TYPE::typeidentifier, pNode->childNodes[i]->position));
		}

		return lambdaType;

	}
	case AA_PT_NODE_TYPE::lambdaexpression: {

		// Create expression node
		AA_AST_NODE* lambdaExpr = new AA_AST_NODE(L"=>", AA_AST_NODE_TYPE::lambdaexpression, pNode->position);

		// Create params
		AA_AST_NODE* lambdaParams = new AA_AST_NODE(L"", AA_AST_NODE_TYPE::lambdaparams, pNode->childNodes[0]->position);
		for (size_t i = 0; i < pNode->childNodes[0]->childNodes.size(); i++) {
			AA_AST_NODE* var = new AA_AST_NODE(pNode->childNodes[0]->childNodes[i]->content, AA_AST_NODE_TYPE::vardecl, pNode->childNodes[0]->childNodes[i]->position);
			if (pNode->childNodes[0]->childNodes[i]->childNodes.size() > 0) {
				var->expressions.push_back(new AA_AST_NODE(pNode->childNodes[0]->childNodes[i]->childNodes[0]->content, AA_AST_NODE_TYPE::typeidentifier, pNode->childNodes[0]->childNodes[i]->position));
			}
			lambdaParams->expressions.push_back(var);
		}

		lambdaExpr->expressions.push_back(lambdaParams);

		// Abstract body and add it
		AA_AST_NODE* lambdaBody = this->AbstractNode(pNode->childNodes[1]);
		lambdaBody->type = AA_AST_NODE_TYPE::lambdabody;
		lambdaExpr->expressions.push_back(lambdaBody);

		return lambdaExpr;

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

#pragma region PT to AST Helper functions

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

AA_AST_NODE_TYPE AA_AST::GetASTAccessType(AA_PT_NODE* pNode) {
	if (pNode->childNodes[1]->nodeType == AA_PT_NODE_TYPE::identifier) {
		return (pNode->content.compare(L"::") == 0) ? AA_AST_NODE_TYPE::memberaccess : AA_AST_NODE_TYPE::fieldaccess;
	} else {
		return AA_AST_NODE_TYPE::callaccess;
	}
}

bool AA_AST::IsClassBodyOmmisionAllowed(AA_PT_NODE* pNode) {

	for (auto pModifier : pNode->childNodes[AA_NODE_CLASSNODE_MODIFIER]->childNodes) {
		if (pModifier->content.compare(L"abstract") == 0 || pModifier->content.compare(L"tagged") == 0) {
			return true;
		}
	}

	return false;

}

#pragma endregion

#pragma region Simplify Tree

void AA_AST::Simplify() {
	this->SimplifyNode(m_root);
}

AA_AST_NODE* AA_AST::SimplifyNode(AA_AST_NODE* pNode) {

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::compile_unit: 
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			this->SimplifyNode(pNode->expressions[i]);
		}
		return pNode;
	case AA_AST_NODE_TYPE::fundecl:
		if (pNode->expressions.size() >= 3) {
			size_t i = 0;
			while (i < pNode->expressions[AA_NODE_FUNNODE_BODY]->expressions.size()) {
				AA_AST_NODE* expNode = this->SimplifyNode(pNode->expressions[AA_NODE_FUNNODE_BODY]->expressions[i]);
				if (expNode) {
					pNode->expressions[AA_NODE_FUNNODE_BODY]->expressions[i] = expNode;
					i++;
				} else {
					pNode->expressions[AA_NODE_FUNNODE_BODY]->expressions.erase(pNode->expressions[AA_NODE_FUNNODE_BODY]->expressions.begin() + i);
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

#pragma endregion

void AA_AST::Expand() {

	// Expand tool
	AA_AST_Expander expander;

	// Expand ourselves
	expander.ExpandTree(this);

	// TODO: Solve errors (if any)

}

void AA_AST::SetError(std::string msg, int type, AACodePosition src) {
	m_anyLastErr = true;
	m_lastError = AA_AST::Error(msg, type, src);
}
