#pragma once
#include "AALexer.h"

enum class AA_PT_NODE_TYPE {

	undefined,

	seperator,

	block,
	block_start,
	block_end,

	expression,
	parenthesis_start,
	parenthesis_end,

	binary_operation,
	unary_operation,

	keyword,
	identifier,
	decleration,

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
	AA_PT_NODE* CreateVariableDecl(std::vector<AA_PT_NODE*>& nodes, int from);
	void HandleTreeCase(std::vector<AA_PT_NODE*>& nodes, size_t& index);

	/*
	** Mathematical and language binding functions
	*/

	void ApplyOrderOfOperationBindings(std::vector<AA_PT_NODE*>& nodes);
	void ApplyGroupings(std::vector<AA_PT_NODE*>& nodes);
	void ApplyUnaryBindings(std::vector<AA_PT_NODE*>& nodes);
	void ApplyArithemticRules(std::vector<AA_PT_NODE*>& nodes);
	void ApplyAssignmentOrder(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Flow control parsing
	*/
	void ApplyFlowControlBindings(std::vector<AA_PT_NODE*>& nodes);
	void ApplyStatementBindings(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Open/Close operator pairing operations - eg. { <some content> } => BlockExpr, ( <some content> ) => Expr, [ <some content> ] => IndexerExpr
	*/
	std::vector<AA_PT_NODE*> PairStatements(std::vector<AA_PT_NODE*>& nodes, int& index, AA_PT_NODE_TYPE open, AA_PT_NODE_TYPE close, AA_PT_NODE_TYPE contentType);

private:

	AA_PT_NODE* m_root;

};
