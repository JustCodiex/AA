#pragma once
#include "AA_PT.h"
#include "AA_AST_NODE.h"

/// <summary>
/// Represents an Abstract Syntax Tree (AST) for the AA language
/// </summary>
class AA_AST {

public:

	AA_AST(AA_PT* parseTree);
	AA_AST(AA_AST_NODE* rootNode) {
		m_root = rootNode;
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

private:

	/*
	** AA_PT_NODE to AA_AST_NODE
	*/
	AA_AST_NODE* AbstractNode(AA_PT_NODE* pNode);

	AA_AST_NODE_TYPE GetASTLiteralType(AA_PT_NODE_TYPE type);
	AA_AST_NODE_TYPE GetASTBlockType(AA_PT_NODE_TYPE type);
	AA_AST_NODE_TYPE GetASTAccessType(AA_PT_NODE* pNode);

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

private:

	AA_AST_NODE* m_root;

};
