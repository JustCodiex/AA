#pragma once
#include <string>
#include <vector>
#include "AACodePosition.h"

enum class AA_PT_NODE_TYPE {

	undefined,

	seperator,
	accessor,

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

	forstatement,
	foreachstatement,
	dowhilestatement,
	whilestatement,

	breakstatement,
	continuestatement,
	returnstatement,

	newstatement,

	matchsatement,
	matchcasestatement,

	parenthesis_start,
	parenthesis_end,

	indexer_start,
	indexer_end,
	indexing,

	binary_operation,
	unary_operation,

	keyword,
	identifier,
	vardecleration,
	fundecleration,
	classdecleration,

	funccall,
	funarg,
	funarglist,

	intliteral,
	floatliteral,
	stringliteral,
	charliteral,
	booliterral,
	nullliteral,

};

struct AA_PT_NODE {
	AA_PT_NODE_TYPE nodeType;
	std::wstring content;
	std::vector< AA_PT_NODE*> childNodes;
	AACodePosition position;
	AA_PT_NODE(AACodePosition pos) {
		nodeType = AA_PT_NODE_TYPE::undefined;
		content = L"";
		position = pos;
	}
};
