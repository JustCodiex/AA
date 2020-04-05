#pragma once
#include "AACFunction.h"

struct AACClassField {
	std::wstring fieldname;
	AACType* fieldtype;
	AACClassField() { fieldname = L"ERROR"; fieldtype = AACType::ErrorType; }
	AACClassField(AACType* type, std::wstring name) {
		this->fieldname = name;
		this->fieldtype = type;
	}
};

struct AACClassOperator {
	std::wstring operatorToOverride;
	AACSingleFunction funcPtr;
	AACClassOperator() {
		this->operatorToOverride = L"NO OPERATOR";
		this->funcPtr = AACSingleFunction(L"", 0, AACType::ErrorType, 0);
	}
	AACClassOperator(std::wstring op, AACSingleFunction fPtr) {
		this->operatorToOverride = op;
		this->funcPtr = fPtr;
	}
};

struct AACClass {

	std::vector<AACClassField> classFields;
	std::vector<AACSingleFunction> classMethods;
	std::vector<AACClassOperator> classOperators;

};
