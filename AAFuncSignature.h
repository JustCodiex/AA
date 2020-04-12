#pragma once
#include "AA_AST_NODE.h"
#include "AACType.h"
#include "AAModifier.h"
#include <string>
#include <vector>

struct AACNamespace;

struct AAFuncParam {
	AACType* type;
	std::wstring identifier;
	AAFuncParam() {
		this->type = AACType::Void;
		this->identifier = L"";
	}
	AAFuncParam(AACType* type, std::wstring identifier) {
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

	AACType* returnType;
	std::wstring name;
	std::vector<AAFuncParam> parameters;
	bool isVMFunc;
	bool isClassMethod;
	bool isClassCtor;
	bool isClassDtor;
	bool isCompilerGenerated;
	AAAccessModifier accessModifier;
	AAStorageModifier storageModifier;
	AACNamespace* domain;

	AA_AST_NODE* node;
	int procID;

	AAFuncSignature() {
		this->returnType = AACType::Void;
		this->name = L"anonymousfunc000";
		this->isVMFunc = false;
		this->isClassMethod = false;
		this->isClassCtor = false;
		this->isClassDtor = false;
		this->isCompilerGenerated = false;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->storageModifier = AAStorageModifier::NONE;
		this->node = 0;
		this->procID = -1;
		this->domain = 0;
	}

	AAFuncSignature(std::wstring name) {
		this->returnType = AACType::Void;
		this->name = name;
		this->isVMFunc = false;
		this->isClassMethod = false;
		this->isClassCtor = false;
		this->isClassDtor = false;
		this->isCompilerGenerated = false;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->storageModifier = AAStorageModifier::NONE;
		this->node = 0;
		this->procID = -1;
		this->domain = 0;
	}

	AAFuncSignature(std::wstring name, AACType* type) {
		this->returnType = type;
		this->name = name;
		this->isVMFunc = false;
		this->isClassMethod = false;
		this->isClassCtor = false;
		this->isClassDtor = false;
		this->isCompilerGenerated = false;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->storageModifier = AAStorageModifier::NONE;
		this->node = 0;
		this->procID = -1;
		this->domain = 0;
	}

	AAFuncSignature(std::wstring name, std::wstring type, std::vector<AAFuncParam> params) {
		this->returnType = AACType::Void;
		this->name = L"anonymousfunc000";
		this->parameters = params;
		this->isVMFunc = false;
		this->isClassMethod = false;
		this->isClassCtor = false;
		this->isClassDtor = false;
		this->isCompilerGenerated = false;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->storageModifier = AAStorageModifier::NONE;
		this->node = 0;
		this->procID = -1;
		this->domain = 0;
	}
	
	/// <summary>
	/// Get the full name of the function (domain name and function name)
	/// </summary>
	/// <returns></returns>
	std::wstring GetFullname(); // Definition in AACNamespace.cpp

	std::wstring GetName() {
		if (isClassMethod) {
			return this->name.substr(this->name.find_last_of(':') + 1);
		} else {
			return this->name;
		}
	}

	bool Equals(AAFuncSignature* other) {
		return *this == *other;
	}

	bool operator==(AAFuncSignature other) {
		if (this->name.compare(other.name) == 0 && this->domain == other.domain) {
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
