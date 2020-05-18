#pragma once
#include "AAClassCompiler.h"
#include "AACNamespace.h"
#include "AA_AST.h"
#include "list.h"
#include <vector>
#include <map>

typedef std::wstring AAId;
typedef std::map<AAId, AACType*> AAVarTypeEnv;

struct AAStaticEnvironment;
struct AACNamespace;

class AATypeChecker {

private:

	/// <summary>
	/// Container for keeping track of dynamically found types (tuples, arrays, and generics [anything using the encapsulated types])
	/// </summary>
	struct DynamicTypeSet {

		/// <summary>
		/// Map from standardized type format to the registered type
		/// </summary>
		std::map<std::wstring, AACType*> registeredTypes;

		AACType* FindType(std::wstring format, AATypeChecker* pTypeChecker);

		AACType* AddType(std::wstring format, AATypeChecker* pTypeChecker);

		aa::list<AACType*> UnpackTuple(std::wstring format, AATypeChecker* pTypeChecker);

	};

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

	AACType* TypeCheckNode(AA_AST_NODE* node);

	AATypeChecker::Error GetErrorMessage() { return m_errMsg; }

private:

	AACType* TypeCheckClassDotCallAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right);
	AACType* TypeCheckClassDotFieldAccessorOperation(AA_AST_NODE* pAccessorNode, AACType* pLeftType, AA_AST_NODE* left, AA_AST_NODE* right);
	AACType* TypeCheckClassCtorCall(AA_AST_NODE* pCallNode);
	AACType* TypeCheckNewStatement(AA_AST_NODE* pNewStatement);

	AACType* TypeCheckTupleAccessorOperation(AA_AST_NODE* pAccessorNode, AACType* pLeftType, AA_AST_NODE* left, AA_AST_NODE* right);
	AACType* TypeCheckMemberAccessorOperation(AA_AST_NODE* pAccessorNode, AA_AST_NODE* left, AA_AST_NODE* right);
	AACType* TypeCheckIndexOperation(AA_AST_NODE* pIndexNode);

	AACType* TypeCheckUnaryOperation(AA_AST_NODE* pOpNode, AA_AST_NODE* right);	
	AACType* TypeCheckCallOperation(AA_AST_NODE* pCallNode);
	
	AACType* TypeCheckConditionalBlock(AA_AST_NODE* pConditionalNode);

	AACType* TypeCheckUsingOperation(AA_AST_NODE* pUseNode);
	
	AACType* TypeCheckBinaryOperation(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right);
	inline AACType* TypeCheckBinaryVarDecl(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right);
	inline AACType* TypeCheckBinaryIndex(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right);
	inline AACType* TypeCheckBinaryTupleDecl(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right);
	inline AACType* TypeCheckBinaryAssignment(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right, AACType* leftType, AACType* rightType);
	inline AACType* TypeCheckBinaryOperatorCall(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right, AACType* leftType, AACType* rightType);
	inline AACType* TypeCheckBinaryOperatorOp(AA_AST_NODE* pOpNode, AA_AST_NODE* left, AA_AST_NODE* right, AACType* leftType, AACType* rightType);

	AACType* TypeCheckPatternMatchBlock(AA_AST_NODE* pMatchNode);
	AACType* TypeCheckPatternMatchCase(AA_AST_NODE* pCaseNode, AACType* conditionType);
	AACType* TypeCheckPatternMatchCaseCondition(AA_AST_NODE* pConditionNode, AACType* conditionType);

	AACType* TypeCheckCtorAndFindBestMatch(AACNamespace* pDomain, AA_AST_NODE* pCallNode);

	AACType* TypeCheckFuncDecl(AA_AST_NODE* pDeclNode);

	AACType* TypeCheckForStatement(AA_AST_NODE* pForStatementNode);
	AACType* TypeCheckWhileStatement(AA_AST_NODE* pWhileStatementNode);

	AACType* TypeOf(AAId var);

	/*
	** Primitive helper functions
	*/

	AACType* TypeCheckBinaryOperationOnPrimitive(AA_AST_NODE* pOpNode, AACType* typeLeft, AACType* typeRight);

	/*
	** Helper functions
	*/

	bool IsValidType(AACType* t);
	bool IsNumericType(AACType* t);

	AACType* FindType(std::wstring t);

	bool IsTypeMatchingFunction(AAFuncSignature* sig, AA_AST_NODE* pCallNode);
	bool IsMatchingTypes(AACType* tCompare, AACType* tExpected);

	AAClassSignature* FindCompiledClassOfType(AACType* type);
	bool FindCompiledClassOperation(AAClassSignature* cc, std::wstring operatorType, AACType* right, AAClassOperatorSignature& op);

	std::wstring FormalizeTuple(AA_AST_NODE* pTupleNode);
	std::wstring FormalizeTuple(aa::list<AACType*> types);
	//std::wstring FormalizeArray(AA_AST_NODE* pArrayNode);

	std::wstring FlattenNamespacePath(AA_AST_NODE* pNode);
	AACNamespace* FindNamespaceFromFlattenedPath(AACNamespace* root, std::wstring path);

	// Sets the last error if not set
	void SetError(AATypeChecker::Error err) { if (!m_hasEnyErr) { m_errMsg = err; m_hasEnyErr = true; } }

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

	// All dynamically found types
	DynamicTypeSet m_dynamicTypes;

};
