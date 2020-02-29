#pragma once
#include "AA_PT.h"

enum class AA_AST_NODE_TYPE {

	undefined,
	seperator,

	block,
	binop,
	unop,
	
	intliteral,
	floatliteral,
	stringliteral,
	charliteral,

};

struct AA_AST_NODE {
	AA_AST_NODE_TYPE type;
	std::wstring content;
	std::vector<AA_AST_NODE*> expressions;
	AA_AST_NODE(std::wstring content, AA_AST_NODE_TYPE type) {
		this->type = type;
		this->content = content;
	}
};

class AA_AST {

public:

	AA_AST(AA_PT* parseTree);

	AA_AST_NODE* GetRoot() { return m_root; }

	void Simplify(); // Eg. pre-compute operations involving constants

private:

	AA_AST_NODE* AbstractNode(AA_PT_NODE* pNode);

private:

	AA_AST_NODE* m_root;

};
