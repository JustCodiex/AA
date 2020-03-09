#include "AAClassCompiler.h"


void AAClassCompiler::RedefineFunDecl(std::wstring className, AA_AST_NODE* pFuncDeclNode) {

	pFuncDeclNode->content = className + L"::" + pFuncDeclNode->content;

	AA_AST_NODE* thisRef = new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::funarg, AACodePosition(0,0));
	thisRef->expressions.push_back(new AA_AST_NODE(className, AA_AST_NODE_TYPE::typeidentifier, AACodePosition(0, 0)));
	
	pFuncDeclNode->expressions[1]->expressions.insert(pFuncDeclNode->expressions[1]->expressions.begin(), thisRef);

	// TODO: Go through the body

}
