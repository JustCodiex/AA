#pragma once
#include "AACodePosition.h"
#include <string>
#include <vector>
#include <map>

enum class AA_AST_NODE_TYPE {

	undefined,

	seperator,
	//accessor,
	callaccess, // Call method on object
	fieldaccess, // Get or set field access
	memberaccess, // Get class type
	enumvalueaccess, // Get enum value access (warning => will use memberaccess until typechecker corrects it)

	block,
	funcbody,
	classbody,
	enumbody,
	lambdabody,

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

	matchstatement,
	matchcaselist,
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
	namespaceidentifier,

	name_space,

	vardecl,
	variable, // variable identifier

	field,
	index,

	funcall,
	funarg,
	funarglist,

	enumdecleration,
	enumvallist,
	enumidentifier,

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
	bool HasTag(const char* tag) { return tags.find(tag) != tags.end(); }
};
