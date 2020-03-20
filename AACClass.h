#pragma once
#include "AACFunction.h"

struct AACClassField {
	std::wstring fieldname;
	std::wstring fieldtype;
	AACClassField() { fieldname = L"ERROR"; fieldtype = L"INVALID TYPE"; }
	AACClassField(std::wstring type, std::wstring name) {
		this->fieldname = name;
		this->fieldtype = type;
	}
};

struct AACClass {

	std::vector<AACClassField> classFields;
	std::vector<AACSingleFunction> classMethods;

};
