#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "AACodePosition.h"
#include "AA_Node_Consts.h"

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
	lambdaparams,

	expression,
	parameterlist,
	condition,

	usingstatement,
	fromstatement,

	ifstatement,
	elseifstatement,
	elsestatement,

	forstatement,
	forinit,
	forafterthought,
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
	enuminheritancelist,

	classinheritancelist,

	funccall,
	funarg,
	funarglist,

	modifierlist,
	modifier,

	intliteral,
	uintliteral,
	shortliteral,
	ushortliteral,
	longliteral,
	ulongliteral,
	doubleliteral,
	floatliteral,
	stringliteral,
	charliteral,
	booliterral,
	byteliteral,
	nullliteral,

	tupleval, // tuple value eg. (0,1,2,3,4)
	tupleaccess,

};

struct AA_PT_NODE {
	AA_PT_NODE_TYPE nodeType;
	std::wstring content;
	std::vector< AA_PT_NODE*> childNodes;
	AACodePosition position;
	std::unordered_map<const char*, int> flags;
	AA_PT_NODE(AACodePosition pos) {
		this->nodeType = AA_PT_NODE_TYPE::undefined;
		this->content = L"";
		this->position = pos;
	}
	AA_PT_NODE(AA_PT_NODE_TYPE type, AACodePosition pos) {
		this->nodeType = type;
		this->content = L"";
		this->position = pos;
	}
	AA_PT_NODE(AA_PT_NODE_TYPE type, const wchar_t* content, AACodePosition pos) {
		this->nodeType = type;
		this->content = content;
		this->position = pos;
	}
	bool HasFlag(const char* flg) { return flags.find(flg) != flags.end(); }
};
