#pragma once
#include "AAFuncSignature.h"
#include "AAClassCompiler.h"
#include "AA_AST.h"
#include "list.h"
#include <vector>
#include <map>

typedef std::wstring AAId;
typedef std::wstring AAValType;
typedef std::map<AAId, AAValType> AAVarTypeEnv;

class AATypeChecker {

public:

	// Struct containing information about an error that has occured in the type checker
	struct Error {
		char* errMsg;
		int errType;
		AACodePosition errSrc;
		Error() {
			errMsg = 0;
			errType = 1;
			errSrc = AACodePosition(0, 0);
		}
		Error(const char* msg, int tp, AACodePosition pos) {
			errType = tp;
			errMsg = new char[strlen(msg) + 1];
			strcpy(errMsg, msg);
			errSrc = pos;
		}
		Error(std::string msg, int tp, AACodePosition pos) {
			errType = tp;
			errMsg = new char[msg.length() + 1];
			strcpy(errMsg, msg.c_str());
			errSrc = pos;
		}
	};

public:

	AATypeChecker(AA_AST* pTree, aa::list<std::wstring> regTypes, aa::list<AAFuncSignature> sigs, aa::list<CompiledClass> classes);

	bool TypeCheck(); // Run a type-check on the tree

	AAValType TypeCheckNode(AA_AST_NODE* node);

	AATypeChecker::Error GetErrorMessage() { return m_errMsg; }

	static aa::list<std::wstring> DefaultTypeEnv;

private:

	AAValType TypeCheckClassDotCallAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right);
	AAValType TypeCheckClassDotFieldAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right);
	AAValType TypeCheckBinaryOperation(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right);
	AAValType TypeCheckUnaryOperation(AA_AST_NODE* pOpNode, AA_AST_NODE* right);
	AAValType TypeCheckCallOperation(AA_AST_NODE* pCallNode);
	AAValType TypeCheckConditionalBlock(AA_AST_NODE* pConditionalNode);
	AAValType TypeCheckIndexOperation(AA_AST_NODE* pIndexNode);

	AAValType TypeCheckNewStatement(AA_AST_NODE* pNewStatement);

	AAValType TypeCheckFuncDecl(AA_AST_NODE* pDeclNode);

	AAValType TypeOf(AAId var);
	AAValType TypeOfArrayElements(AAValType t);

	/*
	** Primitive helper functions
	*/

	AAValType TypeCheckBinaryOperationOnPrimitive(AA_AST_NODE* pOpNode, AAValType typeLeft, AAValType typeRight);

	/*
	** Helper functions
	*/

	bool IsValidType(AAValType t);
	bool IsPrimitiveType(AAValType t);
	bool IsArrayType(AAValType t);

	CompiledClass FindCompiledClassOfType(AAValType type);
	bool FindCompiledClassOperation(CompiledClass cc, std::wstring operatorType, AAValType right, CompiledClassOperator& op);

	// Sets the last error if not set
	void SetError(AATypeChecker::Error err) { if (!m_hasEnyErr) { m_errMsg = err; m_hasEnyErr = true; } }

	static AAValType InvalidTypeStr;

private:

	AA_AST* m_currentTree;
	AAVarTypeEnv m_vtenv;
	aa::list<AAValType> m_types;
	aa::list<AAFuncSignature> m_ftenv;

	// Class Auxiliary Environment
	aa::list<CompiledClass> m_caenv;

	bool m_hasEnyErr;
	AATypeChecker::Error m_errMsg;

};
