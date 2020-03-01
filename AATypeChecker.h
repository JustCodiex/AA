#pragma once
#include "AA_AST.h"
#include <map>

class AATypeChecker {

public:

	AATypeChecker(AA_AST* pTree);

	void TypeCheck(); // Run a type-check on the tree

private:

	std::wstring TypeCheckNode(AA_AST_NODE* node);

	std::wstring TypeCheckBinaryOperation(AA_AST_NODE* left, AA_AST_NODE* right);
	std::wstring TypeCheckUnaryOperation(AA_AST_NODE* right);

private:

	AA_AST* m_currentTree;
	std::map<std::wstring, std::wstring> m_vtenv;

};
