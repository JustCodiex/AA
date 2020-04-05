#pragma once
#include "AAFuncSignature.h"
#include "set.h"

struct AAClassOperatorSignature {
	std::wstring op;
	AAFuncSignature method;
	AAClassOperatorSignature() {
		this->op = L"";
		this->method = AAFuncSignature();
	}
	AAClassOperatorSignature(std::wstring op, AAFuncSignature method) {
		this->op = op;
		this->method = method;
	}
	bool operator==(AAClassOperatorSignature other) {
		return other.method == this->method;
	}
};

struct AAClassFieldSignature {
	std::wstring name;
	AACType* type;
	int fieldID;
	AAAccessModifier accessModifier;
	AAClassFieldSignature() {
		this->name = L"";
		this->type = AACType::ErrorType;
		this->fieldID = -1;
		this->accessModifier = AAAccessModifier::PUBLIC;
	}
	bool operator==(AAClassFieldSignature other) {
		return other.name == this->name;
	}
};

struct AAClassSignature {
	
	std::wstring name;
	aa::set<AAFuncSignature> methods;
	aa::set<AAClassOperatorSignature> operators;
	aa::set<AAClassFieldSignature> fields;
	
	size_t classByteSz;
	
	AAAccessModifier accessModifier;
	AACNamespace* domain;
	AACType* type;

	AAClassSignature() {
		this->name = L"";
		this->classByteSz = 0;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->domain = 0;
		this->type = new AACType(this);
	}
	
	AAClassSignature(std::wstring name) {
		this->name = name;
		this->classByteSz = 0;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->domain = 0;
		this->type = new AACType(this);
	}

	/// <summary>
	/// Get the full name of the class (domain name and class name)
	/// </summary>
	/// <returns></returns>
	std::wstring GetFullname(); // Definition in AACNamespace.cpp

	bool operator==(AAClassSignature other) {
		return other.name.compare(this->name) == 0 && other.domain == this->domain;
	}
	
};
