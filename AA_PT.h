#pragma once
#include "AALexer.h"

enum class AA_PT_NODE_TYPE {

	undefined,

	seperator,

	block,
	block_start,
	block_end,

	funcbody,
	classbody,

	expression,
	condition,

	ifstatement,
	elseifstatement,
	elsestatement,

	parenthesis_start,
	parenthesis_end,

	binary_operation,
	unary_operation,

	keyword,
	identifier,
	vardecleration,
	fundecleration,
	
	funccall,
	funarg,
	funarglist,

	intliteral,
	floatliteral,
	stringliteral,
	charliteral,
	booliterral,

};

struct AA_PT_NODE {	
	AA_PT_NODE_TYPE nodeType;
	std::wstring content;
	std::vector< AA_PT_NODE*> childNodes;
	AA_PT_NODE* parent;
	AACodePosition position;
	AA_PT_NODE(AACodePosition pos) {
		nodeType = AA_PT_NODE_TYPE::undefined;
		content = L"";
		parent = 0;
		position = pos;
	}
};

class AA_PT {
	
public:

	AA_PT(AA_PT_NODE* root);

	AA_PT_NODE* GetRoot() { return m_root; }

	static std::vector<AA_PT*> CreateTrees(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Mathematical and language binding functions
	*/

	static void ApplyOrderOfOperationBindings(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Flow control parsing
	*/
	static void ApplyFlowControlBindings(std::vector<AA_PT_NODE*>& nodes);

	/*
	** To node converter functions
	*/
	static std::vector<AA_PT_NODE*> ToNodes(std::vector<AALexicalResult> lexResult);

private:

	/*
	** To node converter functions
	*/

	static AA_PT_NODE_TYPE GetSeperatorType(std::wstring val);
	static bool IsUnaryOperator(std::vector<AA_PT_NODE*> nodes);
	static bool IsBoolKeyword(std::wstring keyword);

	/*
	** To tree functions
	*/
	
	AA_PT_NODE* CreateTree(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateExpressionTree(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateVariableDecl(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateFunctionDecl(std::vector<AA_PT_NODE*>& nodes, size_t from);
	AA_PT_NODE* CreateConditionBlock(std::vector<AA_PT_NODE*>& nodes, size_t from);
	std::vector<AA_PT_NODE*> CreateArgumentTree(AA_PT_NODE* pExpNode);
	void HandleTreeCase(std::vector<AA_PT_NODE*>& nodes, size_t& index);

	AA_PT_NODE* CreateFunctionArgList(AA_PT_NODE* pExpNode);

	/*
	** Mathematical and language binding functions
	*/

	static void ApplyGroupings(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyUnaryBindings(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyArithemticRules(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyAssignmentOrder(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Flow control parsing
	*/
	static void ApplyStatementBindings(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Open/Close operator pairing operations - eg. { <some content> } => BlockExpr, ( <some content> ) => Expr, [ <some content> ] => IndexerExpr
	*/
	static std::vector<AA_PT_NODE*> PairStatements(std::vector<AA_PT_NODE*>& nodes, int& index, AA_PT_NODE_TYPE open, AA_PT_NODE_TYPE close, AA_PT_NODE_TYPE contentType);

private:

	AA_PT_NODE* m_root;

};
