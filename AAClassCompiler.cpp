#include "AAClassCompiler.h"
#include "AAVal.h"

void AAClassCompiler::RedefineFunDecl(std::wstring className, AA_AST_NODE* pFuncDeclNode) {

	bool isConstructor = false;

	if (pFuncDeclNode->content == L".ctor") {
		pFuncDeclNode->content = className;
		pFuncDeclNode->expressions[0] = new AA_AST_NODE(className, AA_AST_NODE_TYPE::typeidentifier, pFuncDeclNode->position);
		isConstructor = true;
	}

	pFuncDeclNode->content = className + L"::" + pFuncDeclNode->content;

	AA_AST_NODE* thisRef = new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::funarg, AACodePosition(0,0));
	thisRef->expressions.push_back(new AA_AST_NODE(className, AA_AST_NODE_TYPE::typeidentifier, AACodePosition(0, 0)));
	
	pFuncDeclNode->expressions[1]->expressions.insert(pFuncDeclNode->expressions[1]->expressions.begin(), thisRef);

	// TODO: Go through the body

	// Are we the constructor
	if (isConstructor) {

		// Set last statement to be a "return this;" statement
		AA_AST_NODE* returnThisKw = new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::variable, AACodePosition(0, 0));
		pFuncDeclNode->expressions[2]->expressions.push_back(returnThisKw);

	}

}

CompiledClass AAClassCompiler::FindClassFromCtor(std::wstring ctorname, aa::list<CompiledClass> classes) {

	for (size_t i = 0; i < classes.Size(); i++) {
		if (classes.At(i).name == ctorname) {
			return classes.At(i);
		}
	}

	return CompiledClass();

}

CompiledClassMethod AAClassCompiler::FindBestCtor(CompiledClass* pCC) {

	for (size_t i = 0; i < pCC->methods.Size(); i++) {
		if (pCC->methods.At(i).isCtor) {
			return pCC->methods.At(i);
		}
	}

	return CompiledClassMethod();

}

size_t AAClassCompiler::CalculateMemoryUse(CompiledClass cc) {

	// Total size of class in memory
	size_t total = 0;

	// Right now we just have to account for fields
	total += cc.fields.Size() * sizeof(AAVal);

	// Return total
	return total;

}

void AAClassCompiler::CorrectReferences(CompiledClass* cc, std::vector<AA_AST_NODE*> pFuncBodies) {

	for (size_t i = 0; i < pFuncBodies.size(); i++) {
		this->CorrectFuncFieldReferences(pFuncBodies[i], cc);
	}

}

void AAClassCompiler::CorrectFuncFieldReferences(AA_AST_NODE* pNode, CompiledClass* cc) {

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
		int fID;
		if (HasField(*cc, pNode->content, fID)) {
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
	pNode->expressions.push_back(new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::variable, pNode->position));
	pNode->expressions[pNode->expressions.size() - 1]->tags["fieldid"] = fieldId;

}

bool AAClassCompiler::HasField(CompiledClass cc, std::wstring fieldname, int& fieldID) {

	for (size_t i = 0; i < cc.fields.Size(); i++) {
		if (cc.fields.At(i).name == fieldname) {
			fieldID = cc.fields.At(i).fieldID;
			return true;
		}
	}

	return false;

}
