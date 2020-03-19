#pragma once
#include "AACFunction.h"

struct AACClassField {
	std::wstring fieldname;
	std::wstring fieldtype;
};

struct AACClass {

	std::vector<AACClassField> classFields;
	std::vector<AACSingleFunction> classMethods;

};
