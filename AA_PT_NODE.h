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
	enumbody,
	lambdabody,

	expression,
	condition,

	usingstatement,
	fromstatement,

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

	matchstatement,
	matchcaselist,
	matchcasestatement,

	parenthesis_start,
	parenthesis_end,

	indexer_start,
	indexer_end,
	indexing,
	indexat,

	binary_operation,
	unary_operation,

	keyword,
	identifier,
	vardecleration,
	fundecleration,
	classdecleration,
	namespacedecleration,
	enumdecleration,

	enumvallist,

	funccall,
	funarg,
	funarglist,

	intliteral,
	shortliteral,
	longliteral,
	doubleliteral,
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
