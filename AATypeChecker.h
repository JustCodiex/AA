#pragma once
#include "AAFuncSignature.h"
#include "AA_AST.h"
#include "list.h"
#include <vector>
#include <map>

typedef std::wstring AAId;
typedef std::wstring AAValType;
typedef std::map<AAId, AAValType> AAVarTypeEnv;

class AATypeChecker {

public:

	AATypeChecker(AA_AST* pTree, aa::list<std::wstring> regTypes, aa::list<AAFuncSignature> sigs);

	bool TypeCheck(); // Run a type-check on the tree

	AAValType TypeCheckNode(AA_AST_NODE* node);

	static aa::list<std::wstring> DefaultTypeEnv;

private:

	AAValType TypeCheckClassDotAccessorOperation(AA_AST_NODE* left, AA_AST_NODE* right);
	AAValType TypeCheckBinaryOperation(AA_AST_NODE* left, AA_AST_NODE* right);
	AAValType TypeCheckUnaryOperation(AA_AST_NODE* right);
	AAValType TypeCheckCallOperation(AA_AST_NODE* pCallNode);

	AAValType TypeCheckFuncDecl(AA_AST_NODE* pDeclNode);

	AAValType TypeOf(AAId var);

	bool IsValidType(AAValType t);

	static AAValType InvalidTypeStr;

private:

	AA_AST* m_currentTree;
	AAVarTypeEnv m_vtenv;
	aa::list<AAValType> m_types;
	aa::list<AAFuncSignature> m_ftenv;

};
