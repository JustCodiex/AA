#pragma once
#include "AAClassCompiler.h"
#include "AA_AST.h"
#include "list.h"
#include <vector>
#include <map>

typedef std::wstring AAId;
typedef std::wstring AAValType;
typedef std::map<AAId, AAValType> AAVarTypeEnv;

struct AAStaticEnvironment;
struct AACNamespace;

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

	AATypeChecker(AA_AST* pTree, AAStaticEnvironment* senv);

	bool TypeCheck(); // Run a type-check on the tree

	AAValType TypeCheckNode(AA_AST_NODE* node);

	AATypeChecker::Error GetErrorMessage() { return m_errMsg; }

	static aa::list<std::wstring> DefaultTypeEnv;

private:

	AAValType TypeCheckClassDotCallAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right);
	AAValType TypeCheckClassDotFieldAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right);
	AAValType TypeCheckMemberAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right);
	AAValType TypeCheckBinaryOperation(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right);
	AAValType TypeCheckUnaryOperation(AA_AST_NODE* pOpNode, AA_AST_NODE* right);
	AAValType TypeCheckCallOperation(AA_AST_NODE* pCallNode);
	AAValType TypeCheckConditionalBlock(AA_AST_NODE* pConditionalNode);
	AAValType TypeCheckIndexOperation(AA_AST_NODE* pIndexNode);
	AAValType TypeCheckUsingOperation(AA_AST_NODE* pUseNode);

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

	bool IsTypeMatchingFunction(AAFuncSignature sig, AA_AST_NODE* pCallNode);
	bool IsMatchingTypes(AAValType tCompare, AAValType tExpected);

	AAClassSignature FindCompiledClassOfType(AAValType type);
	bool FindCompiledClassOperation(AAClassSignature cc, std::wstring operatorType, AAValType right, AAClassOperatorSignature& op);

	// Sets the last error if not set
	void SetError(AATypeChecker::Error err) { if (!m_hasEnyErr) { m_errMsg = err; m_hasEnyErr = true; } }

	static AAValType InvalidTypeStr;

private:

	// Current tree we're working with
	AA_AST* m_currentTree;

	// Variable type environment
	AAVarTypeEnv m_vtenv;

	// Do we have any errors flag
	bool m_hasEnyErr;

	// Last error message
	AATypeChecker::Error m_errMsg;

	// The static environment available while type-checking
	AAStaticEnvironment* m_senv;

	// The current namespace
	AACNamespace* m_currentnamespace;

	// The current, local, namespace for a statement. Warning! Must be properly cleared after use
	AACNamespace* m_localStatementNamespace;

};
