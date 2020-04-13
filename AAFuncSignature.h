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
	AAFuncSignature* overrides; // When calling base.<> we now know which function to call!
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
		this->overrides = 0;
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
		this->overrides = 0;
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
		this->overrides = 0;
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
		this->overrides = 0;
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

	bool EqualsParams(std::vector<AAFuncParam> params, bool ignoreFirst = false) {
		if (this->parameters.size() == params.size()) {
			for (size_t i = (ignoreFirst)?1:0; i < this->parameters.size(); i++) {
				if (this->parameters[i].type != params[i].type) {
					return false;
				}
			}
			return true;
		} else {
			return false;
		}
	}

	/// <summary>
	/// Check if the given function can be overriden by the function
	/// </summary>
	/// <param name="other">Function to compare with</param>
	/// <returns>True if function is a valid override for 'other' function</returns>
	bool IsValidOverride(AAFuncSignature* other) {
		if (this->storageModifier == AAStorageModifier::OVERRIDE) {
			if (this->GetName().compare(other->GetName()) == 0) {
				if (this->returnType == other->returnType && this->EqualsParams(other->parameters, other->isClassCtor && this->isClassCtor)) {
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else {
			return false;
		}
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
