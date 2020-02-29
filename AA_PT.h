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

	/*
	** To node converter functions
	*/

	std::vector<AA_PT_NODE*> ToNodes(std::vector<AALexicalResult> lexResult);
	AA_PT_NODE_TYPE GetSeperatorType(std::wstring val);
	bool IsUnaryOperator(std::vector<AA_PT_NODE*> nodes);

	/*
	** To tree functions
	*/
	
	AA_PT_NODE* CreateTree(std::vector<AA_PT_NODE*>& nodes, int from);
	AA_PT_NODE* CreateExpressionTree(std::vector<AA_PT_NODE*>& nodes, int from);

	/*
	** Mathematical and language binding functions
	*/

	void PrioritizeBinding(std::vector<AA_PT_NODE*>& nodes);
	void Parenthesise(std::vector<AA_PT_NODE*>& nodes);
	std::vector<AA_PT_NODE*> Parenthesise(std::vector<AA_PT_NODE*>& nodes, int& index);

	void ApplyBindings(std::vector<AA_PT_NODE*>& nodes);
	void ApplyArithemticRules(std::vector<AA_PT_NODE*>& nodes);

private:

	AA_PT_NODE* m_root;

};
