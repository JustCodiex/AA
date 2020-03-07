#pragma once
#include "AA_PT.h"
#include <map>

enum class AA_AST_NODE_TYPE {

	undefined,
	seperator,

	block,
	funcbody,
	classbody,

	condition,
	ifstatement,
	elseifstatement,
	elsestatement,

	forstatement,
	foreachstatement,
	dowhilestatement,
	whilestatement,

	breakstatement,
	continuestatement,
	returnstatement,

	matchsatement,
	matchcasestatement,

	binop,
	unop,

	fundecl,

	classdecl,

	typeidentifier,

	vardecl,
	variable,

	funcall,
	funarg,
	funarglist,

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
	std::map<const char*, int> tags;
	AA_AST_NODE(std::wstring content, AA_AST_NODE_TYPE type, AACodePosition pos) {
		this->type = type;
		this->content = content;
		this->position = pos;
	}
};

class AA_AST {

public:

	AA_AST(AA_PT* parseTree);
	AA_AST(AA_AST_NODE* rootNode) {
		m_root = rootNode;
	}

	AA_AST_NODE* GetRoot() { return m_root; }

	void Simplify(); // Eg. pre-compute operations involving constants

	void Clear();

private:

	AA_AST_NODE* AbstractNode(AA_PT_NODE* pNode);

	AA_AST_NODE_TYPE GetASTLiteralType(AA_PT_NODE_TYPE type);
	AA_AST_NODE_TYPE GetASTBlockType(AA_PT_NODE_TYPE type);

	/*
	** Recursive clear
	*/
	void ClearNode(AA_AST_NODE* node);

private:

	AA_AST_NODE* m_root;

};
