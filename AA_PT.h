#pragma once
#include "AALexer.h"
#include "AA_PT_NODE.h"

const int AA_PT_NODE_OUT_OF_BOUNDS_INDEX = 9999;

class AA_PT {
	
public:

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

	AA_PT(AA_PT_NODE* root);

	AA_PT_NODE* GetRoot() { return m_root; }

	void Clear();

	static std::vector<AA_PT*> CreateTrees(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Mathematical and language binding functions
	*/

	static void ApplyOrderOfOperationBindings(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Flow control parsing
	*/
	static void ApplyFlowControlBindings(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Incorrect structure fix
	*/
	static void ApplySyntaxRules(std::vector<AA_PT_NODE*>& nodes);

	/*
	** To node converter functions
	*/
	static std::vector<AA_PT_NODE*> ToNodes(std::vector<AALexicalResult> lexResult);

	/*
	** Last error message the parse tree received
	*/
	static AA_PT::Error GetLastErrorMessage() { return g_errMsg; }

	/*
	** Check if an error occured last time the parser was used
	*/
	static bool HasLastErrorMessage() { return g_hasEnyErr; }

	// Makes sure there's no parser errors
	static void ResetLastError() {
		g_hasEnyErr = false;
	}

private:

	/*
	** Recursive clear
	*/
	void ClearNode(AA_PT_NODE* node);

	/*
	** To node converter functions
	*/

	static AA_PT_NODE_TYPE GetSeperatorType(std::wstring val);
	static bool IsUnaryOperator(std::vector<AA_PT_NODE*> nodes);
	static bool IsBoolKeyword(std::wstring keyword);

	/*
	** To tree functions
	*/
	
	AA_PT_NODE* CreateTree(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateExpressionTree(std::vector<AA_PT_NODE*>& nodes, size_t from);
	std::vector<AA_PT_NODE*> CreateDeclerativeBody(std::vector<AA_PT_NODE*> nodes);
	
	AA_PT_NODE* CreateVariableDecl(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateClassDecl(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateFunctionDecl(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateConstructorDecl(std::vector<AA_PT_NODE*>& nodes, size_t from);

	AA_PT_NODE* CreateConditionBlock(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateIfStatement(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateForStatement(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateWhileStatement(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateDoWhileStatement(std::vector<AA_PT_NODE*>& nodes, size_t from);
	
	std::vector<AA_PT_NODE*> CreateArgumentTree(AA_PT_NODE* pExpNode);
	
	inline void HandleTreeCase(std::vector<AA_PT_NODE*>& nodes, size_t& index);
	inline void HandleKeywordCase(std::vector<AA_PT_NODE*>& nodes, size_t& nodeIndex);

	inline void HandleIndexDecl(std::vector<AA_PT_NODE*>& nodes, size_t nodeIndex);

	inline AA_PT_NODE* HandleFunctionDecleration(std::vector<AA_PT_NODE*>& nodes, size_t& from);
	inline AA_PT_NODE* HandleVariableDecleration(std::vector<AA_PT_NODE*>& nodes, size_t& from);

	AA_PT_NODE* CreateFunctionArgList(AA_PT_NODE* pExpNode);

	/*
	** Mathematical and language binding functions
	*/

	static void ApplyGroupings(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyAccessorBindings(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyFunctionBindings(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyUnaryBindings(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyArithemticRules(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyAssignmentOrder(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Flow control parsing
	*/
	static void ApplyStatementBindings(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Open/Close operator pairing operations - eg. { <some content> } => BlockExpr, ( <some content> ) => Expr, [ <some content> ] => IndexerExpr
	*/
	static std::vector<AA_PT_NODE*> PairStatements(std::vector<AA_PT_NODE*>& nodes, int& index, AA_PT_NODE_TYPE open, AA_PT_NODE_TYPE close, AA_PT_NODE_TYPE contentType);

	// Sets the last error if not set
	static void SetError(AA_PT::Error err) { if (!g_hasEnyErr) { g_errMsg = err; g_hasEnyErr = true; } }

private:

	AA_PT_NODE* m_root;

private:

	static bool g_hasEnyErr;
	static AA_PT::Error g_errMsg;

};
