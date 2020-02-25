#pragma once
#include "AALexer.h"

enum class AA_PT_NODE_TYPE {
	undefined,
	seperator,
	intliteral,
	binary_operator,
	unary_operator,
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

private:

	std::vector<AA_PT_NODE*> ToNodes(std::vector<AALexicalResult> lexResult);

	bool IsUnaryOperator(std::vector<AA_PT_NODE*> nodes);

	void CreateTree(std::vector<AA_PT_NODE*> nodes);

};

