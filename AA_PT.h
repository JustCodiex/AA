#pragma once
#include "AALexer.h"

enum class AA_PT_NODE_TYPE {

	undefined,
	
	seperator,
	
	block,
	expression,
	parenthesis_start,
	parenthesis_end,

	binary_operation,
	unary_operation,

	intliteral,
	floatliteral,
	stringliteral,
	charliteral,

};

struct AA_PT_NODE {	
	AA_PT_NODE_TYPE nodeType;
	std::wstring content;
	std::vector< AA_PT_NODE*> childNodes;
	AA_PT_NODE* parent;
	AA_PT_NODE() {
		nodeType = AA_PT_NODE_TYPE::undefined;
		content = L"";
		parent = 0;
	}
};

class AA_PT {
	
public:

	AA_PT(std::vector<AALexicalResult> lexResult);

	AA_PT_NODE* GetRoot() { return m_root; }

private:

	std::vector<AA_PT_NODE*> ToNodes(std::vector<AALexicalResult> lexResult);

	bool IsUnaryOperator(std::vector<AA_PT_NODE*> nodes);

	AA_PT_NODE_TYPE GetSeperatorType(std::wstring val);

	AA_PT_NODE* CreateTree(std::vector<AA_PT_NODE*>& nodes, int from);
	AA_PT_NODE* CreateExpressionTree(std::vector<AA_PT_NODE*>& nodes, int from);

	void Parenthesise(std::vector<AA_PT_NODE*>& nodes);
	std::vector<AA_PT_NODE*> Parenthesise(std::vector<AA_PT_NODE*>& nodes, int& index);

private:

	AA_PT_NODE* m_root;

};

