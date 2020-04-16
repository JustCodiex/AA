#pragma once
#include "AA_PT.h"
#include "AA_AST_NODE.h"

/// <summary>
/// Represents an Abstract Syntax Tree (AST) for the AA language
/// </summary>
class AA_AST {

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

	AA_AST(AA_PT* parseTree);
	AA_AST(AA_AST_NODE* rootNode) {
		m_root = rootNode;
		m_anyLastErr = false;
	}

	/// <summary>
	/// Get the root of the tree
	/// </summary>
	/// <returns>The root AA_AST_NODE</returns>
	AA_AST_NODE* GetRoot() { return m_root; }

	/// <summary>
	/// Simplifies the whole tree to the extent it's possible
	/// </summary>
	void Simplify(); // Eg. pre-compute operations involving constants

	/// <summary>
	/// Clears (and deletes) all nodes used by the tree
	/// </summary>
	void Clear();

	/// <summary>
	/// Does the AST contain an error?
	/// </summary>
	/// <returns>Returns ture if the tree contains an error</returns>
	bool HasError() { return m_anyLastErr; }

	/// <summary>
	/// Get the last error
	/// </summary>
	/// <returns>Last error found in tree</returns>
	AA_AST::Error GetError() { return m_lastError; }

private:

	/*
	** AA_PT_NODE to AA_AST_NODE
	*/
	AA_AST_NODE* AbstractNode(AA_PT_NODE* pNode);

	AA_AST_NODE_TYPE GetASTLiteralType(AA_PT_NODE_TYPE type);
	AA_AST_NODE_TYPE GetASTBlockType(AA_PT_NODE_TYPE type);
	AA_AST_NODE_TYPE GetASTAccessType(AA_PT_NODE* pNode);

	bool IsClassBodyOmmisionAllowed(AA_PT_NODE* pNode);

	/*
	** AA_AST_NODE simplifier
	*/
	AA_AST_NODE* SimplifyNode(AA_AST_NODE* pNode);
	AA_AST_NODE* SimplifyBinaryNode(AA_AST_NODE* pNode);
	AA_AST_NODE* SimplifyCallAccessorNode(AA_AST_NODE* pNode);

	/*
	** Recursive clear
	*/
	void ClearNode(AA_AST_NODE* node);

	void SetError(std::string msg, int type, AACodePosition src);

private:

	AA_AST_NODE* m_root;

	bool m_anyLastErr;
	AA_AST::Error m_lastError;

};
