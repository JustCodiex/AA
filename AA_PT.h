#pragma once
#include "AALexer.h"
#include "AA_PT_Unflatten.h"

/// <summary>
/// Å Parse Tree
/// </summary>
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
	AA_PT_NODE* CreateNamespaceDecl(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateEnumDecl(std::vector<AA_PT_NODE*>& nodes, size_t from);

	AA_PT_NODE* CreateConditionBlock(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateIfStatement(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateForStatement(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateWhileStatement(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateDoWhileStatement(std::vector<AA_PT_NODE*>& nodes, size_t from);
	
	AA_PT_NODE* CreatePatternMatchBlock(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreatePatternCase(std::vector<AA_PT_NODE*> nodes);
	std::vector<AA_PT_NODE*> CreatePatternCases(AA_PT_NODE* pBlockNode);

	std::vector<AA_PT_NODE*> CreateArgumentTree(AA_PT_NODE* pExpNode);
	
	inline void HandleTreeCase(std::vector<AA_PT_NODE*>& nodes, size_t& index);
	inline void HandleKeywordCase(std::vector<AA_PT_NODE*>& nodes, size_t& nodeIndex);
	inline void HandleTupleCase(std::vector<AA_PT_NODE*>& nodes, size_t& nodeIndex);

	inline void HandleIndexDecl(std::vector<AA_PT_NODE*>& nodes, size_t nodeIndex);

	inline AA_PT_NODE* HandleFunctionDecleration(std::vector<AA_PT_NODE*>& nodes, size_t& from);
	inline AA_PT_NODE* HandleVariableDecleration(std::vector<AA_PT_NODE*>& nodes, size_t& from);

	AA_PT_NODE* CreateArgList(AA_PT_NODE* pExpNode, AA_PT_NODE_TYPE outType, AA_PT_NODE_TYPE elementType);
	AA_PT_NODE* CreateEnumValueList(AA_PT_NODE* pBlockNode);

	/*
	** To tree helper functions
	*/
	bool IsDeclarativeIndexer(AA_PT_NODE* pNode);

	bool IsModifierKeyword(std::wstring ws);

	bool IsLiteral(AA_PT_NODE* pNode);

	bool IsOperator(AA_PT_NODE* pNode, std::wstring op, bool isBinary = true, bool isUnary = false);

	bool ContainsSeperator(AA_PT_NODE* pNode, std::wstring seperator);

	std::wstring FindClassDeclName(std::vector<AA_PT_NODE*>& nodes, const size_t from, AA_PT_NODE* pClassDeclNode);

	// Sets the last error if not set
	static void SetError(AA_PT::Error err) { if (!g_hasEnyErr) { g_errMsg = err; g_hasEnyErr = true; } }

private:

	AA_PT_NODE* m_root;

private:

	static bool g_hasEnyErr;
	static AA_PT::Error g_errMsg;

};
