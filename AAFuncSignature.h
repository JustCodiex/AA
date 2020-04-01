#pragma once
#include "AA_AST_NODE.h"
#include "AAAccessModifier.h"
#include <string>
#include <vector>

struct AAFuncParam {
	std::wstring type;
	std::wstring identifier;
	AAFuncParam() {
		this->type = L"void";
		this->identifier = L"";
	}
	AAFuncParam(std::wstring type, std::wstring identifier) {
		this->type = type;
		this->identifier = identifier;
	}
	bool operator==(AAFuncParam other) {
		return identifier == other.identifier;
	}
	bool operator!=(AAFuncParam other) {
		return !(*this == other);
	}
};

struct AAFuncSignature {

	std::wstring returnType;
	std::wstring name;
	std::vector<AAFuncParam> parameters;
	bool isVMFunc;
	bool isClassMethod;
	bool isClassCtor;
	AAAccessModifier accessModifier;
	bool isVirtual;

	AA_AST_NODE* node;
	int procID;

	AAFuncSignature() {
		this->returnType = L"void";
		this->name = L"anonymousfunc000";
		this->isVMFunc = false;
		this->isClassMethod = false;
		this->isClassCtor = false;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->isVirtual = false;
		this->node = 0;
		this->procID = -1;
	}

	AAFuncSignature(std::wstring name) {
		this->returnType = L"void";
		this->name = name;
		this->isVMFunc = false;
		this->isClassMethod = false;
		this->isClassCtor = false;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->isVirtual = false;
		this->node = 0;
		this->procID = -1;
	}

	AAFuncSignature(std::wstring name, std::wstring type) {
		this->returnType = type;
		this->name = name;
		this->isVMFunc = false;
		this->isClassMethod = false;
		this->isClassCtor = false;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->isVirtual = false;
		this->node = 0;
		this->procID = -1;
	}

	AAFuncSignature(std::wstring name, std::wstring type, std::vector<AAFuncParam> params) {
		this->returnType = L"void";
		this->name = L"anonymousfunc000";
		this->parameters = params;
		this->isVMFunc = false;
		this->isClassMethod = false;
		this->isClassCtor = false;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->isVirtual = false;
		this->node = 0;
		this->procID = -1;
	}

	bool operator==(AAFuncSignature other) {
		if (this->name == other.name) {
			if (this->parameters.size() == other.parameters.size()) {
				for (size_t p = 0; p < this->parameters.size(); p++) {
					if (this->parameters[p] != other.parameters[p]) {
						return false;
					}
				}
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

};
