#pragma once
#include "AALexer.h"

enum class AA_PT_NODE_TYPE {

	undefined,
	seperator,
	
	block,
	binary_operation,
	unary_operator,
	expression,

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

	AA_PT_NODE* CreateTree(std::vector<AA_PT_NODE*>& nodes, int from);

private:

	AA_PT_NODE* m_root;

};

