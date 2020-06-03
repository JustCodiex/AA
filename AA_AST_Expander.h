#pragma once
#include "AA_AST.h"

/// <summary>
/// Helper tool for expanding an Abstract Syntax Tree such that the syntax is normalised
/// </summary>
class AA_AST_Expander {

public:

	/// <summary>
	/// Expand the given tree
	/// </summary>
	/// <param name="pTree">Pointer to AST to expand</param>
	void ExpandTree(AA_AST* pTree);

private:

	/*
	* Recursive node functions
	*/

	void ExpandNode(AA_AST_NODE* pNode);

	void ExpandBinaryOperation(AA_AST_NODE* pNode);
	void ExpandUnaryOperation(AA_AST_NODE* pNode);

	void ExpandCompundAssignment(AA_AST_NODE* pNode);

	/*
	* Helper functions
	*/

	bool IsCompundAssignment(AA_AST_NODE_TYPE type, std::wstring operatorContent);

};
