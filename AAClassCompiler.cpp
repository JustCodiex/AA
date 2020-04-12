#include "AACodePosition.h"
#include "AAClassCompiler.h"
#include "AAVal.h"
#include "AA_Node_Consts.h"

void AAClassCompiler::RedefineFunDecl(std::wstring className, AA_AST_NODE* pFuncDeclNode) {

	// Flag used to see if we're a constructor func
	bool isConstructor = false;

	// Is the name set to .ctor (marked as a constructor by AST)
	if (pFuncDeclNode->content == L".ctor") {
		pFuncDeclNode->content = className;
		pFuncDeclNode->expressions[AA_NODE_FUNNODE_RETURNTYPE] = new AA_AST_NODE(className, AA_AST_NODE_TYPE::typeidentifier, pFuncDeclNode->position);
		isConstructor = true;
	}

	// Fetch real function name
	pFuncDeclNode->content = className + L"::" + pFuncDeclNode->content;

	// Create 'this' reference for function decleration
	AA_AST_NODE* thisRef = new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::funarg, AACodePosition(0,0));
	thisRef->expressions.push_back(new AA_AST_NODE(className, AA_AST_NODE_TYPE::typeidentifier, AACodePosition(0, 0)));
	
	// Add 'this' reference to function decleration
	pFuncDeclNode->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions.insert(pFuncDeclNode->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions.begin(), thisRef);

	// Are we the constructor
	if (isConstructor) {

		// Set last statement to be a "return this;" statement
		AA_AST_NODE* returnThisKw = new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::variable, AACodePosition(0, 0));
		pFuncDeclNode->expressions[AA_NODE_FUNNODE_BODY]->expressions.push_back(returnThisKw);

	}

}

AAClassSignature* AAClassCompiler::FindClassFromCtor(std::wstring ctorname, aa::list<AAClassSignature*> classes) {

	for (size_t i = 0; i < classes.Size(); i++) {
		if (classes.At(i)->name == ctorname) {
			return classes.At(i);
		}
	}

	return 0;

}

AAFuncSignature* AAClassCompiler::FindBestCtor(AAClassSignature* pCC) {

	// TODO: Fix this so we may have multiple ctors
	return pCC->methods.FindFirst([](AAFuncSignature*& sig) { return sig->isClassCtor; });

}

size_t AAClassCompiler::CalculateMemoryUse(AAClassSignature* cc) {

	// Total size of class in memory
	size_t total = 0;

	// Right now we just have to account for fields
	total += cc->fields.Size() * sizeof(AAVal);

	// Return total
	return total;

}

void AAClassCompiler::CorrectReferences(AAClassSignature* cc, std::vector<AA_AST_NODE*> pFuncBodies) {

	for (size_t i = 0; i < pFuncBodies.size(); i++) {
		this->CorrectFuncFieldReferences(pFuncBodies[i], cc);
	}

}

void AAClassCompiler::CorrectFuncFieldReferences(AA_AST_NODE* pNode, AAClassSignature* cc) {

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::block:
	case AA_AST_NODE_TYPE::funcbody:
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			this->CorrectFuncFieldReferences(pNode->expressions[i], cc);
		}
		break;
	case AA_AST_NODE_TYPE::binop:
		this->CorrectFuncFieldReferences(pNode->expressions[0], cc);
		this->CorrectFuncFieldReferences(pNode->expressions[1], cc);
		break;
	case AA_AST_NODE_TYPE::unop:
		this->CorrectFuncFieldReferences(pNode->expressions[0], cc);
		break;
	case AA_AST_NODE_TYPE::variable: {

		int fID; // If we have a field in the class with specified variable (TODO: Check function params - because then the this word must be explicit)
		if (HasField(cc, pNode->content, fID)) {

			// Update the identiffier to be a member access operation
			this->UpdateIdentifierToThisFieldReference(pNode, fID);
		}

		break;
	}
	default:
		break;
	}

}

void AAClassCompiler::UpdateIdentifierToThisFieldReference(AA_AST_NODE* pNode, int fieldId) {

	pNode->type = AA_AST_NODE_TYPE::fieldaccess;
	pNode->expressions.push_back(new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::variable, pNode->position));
	pNode->expressions.push_back(new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::field, pNode->position));
	pNode->expressions[pNode->expressions.size() - 1]->tags["fieldid"] = fieldId;
	pNode->tags["compiler_generated"] = true;

}

bool AAClassCompiler::AddInheritanceCall(AAClassSignature* cc, AA_AST_NODE* pCtorDeclNode) {

	return cc->extends.ForAll(
		[pCtorDeclNode, this](AAClassSignature*& sig) {
			AAFuncSignature* ctor = this->FindBestCtor(sig);
			if (ctor) {
				return this->AddInheritanceCallNode(ctor, pCtorDeclNode);
			} else {
				return false;
			}
		}
	);

}

bool AAClassCompiler::AddInheritanceCallNode(AAFuncSignature* ctor, AA_AST_NODE* pCtorDeclNode) {

	if (ctor) {

		AA_AST_NODE* pCallNode = new AA_AST_NODE(ctor->name, AA_AST_NODE_TYPE::classctorcall, pCtorDeclNode->position);
		pCallNode->tags["compiler_generated"] = true;
		pCallNode->tags["inheritance_call"] = true;
		pCallNode->tags["calls"] = (int)ctor->procID;
		pCallNode->tags["isVM"] = ctor->isVMFunc;
		pCallNode->tags["args"] = (int)ctor->parameters.size();

		AA_AST_NODE* pThisArgNode = new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::variable, pCtorDeclNode->position);
		pThisArgNode->tags["compiler_generated"] = true;
		pCallNode->expressions.push_back(pThisArgNode);

		// Add inheritance call node to start
		pCtorDeclNode->expressions[AA_NODE_FUNNODE_BODY]->expressions.insert(pCtorDeclNode->expressions[AA_NODE_FUNNODE_BODY]->expressions.begin(), pCallNode);

		return true;

	} else {

		return false;

	}

}

bool AAClassCompiler::HasField(AAClassSignature* cc, std::wstring fieldname, int& fieldID) {
	return cc->fields.FindFirstIndex([fieldname](AAClassFieldSignature& sig) { return sig.name == fieldname; }, fieldID);
}
