#include "AAClassCompiler.h"

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
