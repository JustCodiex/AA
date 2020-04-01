#pragma once
#include "AACodePosition.h"
#include <string>
#include <vector>
#include <map>

enum class AA_AST_NODE_TYPE {

	undefined,

	seperator,
	//accessor,
	callaccess,
	fieldaccess,

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

	newstatement,

	usingstatement,
	usingspecificstatement,

	binop,
	unop,

	fundecl,

	classdecl,
	classctorcall,

	typeidentifier,

	name_space,

	vardecl,
	variable,

	field,
	index,

	funcall,
	funarg,
	funarglist,

	intliteral,
	floatliteral,
	stringliteral,
	charliteral,
	boolliteral,
	nullliteral,

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
