#pragma once
#include "AACodePosition.h"
#include <string>
#include <vector>
#include <unordered_map>

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
	forinit,
	forafterthought,
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
	classinheritancelist,

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

	objdeconstruct,

	modifierlist,
	modifier,

	enumdecleration,
	enumvallist,
	enumidentifier,
	enuminheritancelist,

	intliteral,
	floatliteral,
	stringliteral,
	charliteral,
	boolliteral,
	nullliteral,

	tupleval, // tuple value eg. (0,1,2,3,4)
	tuplevardecl, // tuple definition eg. (int, int, int, int, int)
	tupleaccess,
	tupletypeidentifier, // like tuple var decl but for use in typeidentifer cases

};

struct AA_AST_NODE {
	AA_AST_NODE_TYPE type;
	std::wstring content;
	std::vector<AA_AST_NODE*> expressions;
	AACodePosition position;
	std::unordered_map<const char*, int> tags;
	AA_AST_NODE(std::wstring content, AA_AST_NODE_TYPE type, AACodePosition pos) {
		this->type = type;
		this->content = content;
		this->position = pos;
	}
	bool HasTag(const char* tag) { return tags.find(tag) != tags.end(); }
};
