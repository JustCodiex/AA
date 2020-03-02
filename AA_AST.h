#pragma once
#include "AA_PT.h"

enum class AA_AST_NODE_TYPE {

	undefined,
	seperator,

	block,
	binop,
	unop,

	fundecl,

	typeidentifier,

	vardecl,
	variable,

	intliteral,
	floatliteral,
	stringliteral,
	charliteral,
	boolliteral,

};

struct AA_AST_NODE {
	AA_AST_NODE_TYPE type;
	std::wstring content;
	std::vector<AA_AST_NODE*> expressions;
	AACodePosition position;
	AA_AST_NODE(std::wstring content, AA_AST_NODE_TYPE type, AACodePosition pos) {
		this->type = type;
		this->content = content;
		this->position = pos;
	}
};

class AA_AST {

public:

	AA_AST(AA_PT* parseTree);

	AA_AST_NODE* GetRoot() { return m_root; }

	void Simplify(); // Eg. pre-compute operations involving constants

private:

	AA_AST_NODE* AbstractNode(AA_PT_NODE* pNode);

	AA_AST_NODE_TYPE GetASTLiteralType(AA_PT_NODE_TYPE type);

private:

	AA_AST_NODE* m_root;

};
