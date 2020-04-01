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
	std::wstring type;
	int fieldID;
	AAAccessModifier accessModifier;
	AAClassFieldSignature() {
		this->name = L"";
		this->type = L"";
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
	AAClassSignature() {
		this->name = L"";
		this->classByteSz = 0;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->domain = 0;
	}
	bool operator==(AAClassSignature other) {
		return other.name == this->name /*&& other.domain == this->domain*/;
	}
};
