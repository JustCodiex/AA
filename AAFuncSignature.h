#pragma once
#include "AA_AST_NODE.h"
#include "AACType.h"
#include "AAModifier.h"
#include <string>
#include <vector>

struct AACNamespace;

/// <summary>
/// Represents a function parameter (type and name)
/// </summary>
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
	bool operator==(AAFuncParam other) const{
		return identifier.compare(other.identifier) == 0;
	}
	bool operator!=(AAFuncParam other) const {
		return !(*this == other);
	}
};

/// <summary>
/// Static representation of a function signature
/// </summary>
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

	std::wstring GetName() const;

	/// <summary>
	/// Retrieve a functional signature
	/// </summary>
	/// <param name="ignoreClass"></param>
	/// <returns></returns>
	const std::wstring GetFunctionalSignature(bool ignoreClass, bool ignoreName = false) const;

	bool Equals(AAFuncSignature* other) {
		return *this == *other;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="params"></param>
	/// <param name="ignoreFirst"></param>
	/// <returns></returns>
	bool EqualsParams(std::vector<AAFuncParam> params, bool ignoreFirst = false);

	/// <summary>
	/// Check if the given function can be overriden by the function
	/// </summary>
	/// <param name="other">Function to compare with</param>
	/// <returns>True if function is a valid override for 'other' function</returns>
	bool IsValidOverride(AAFuncSignature* other);

	bool operator==(AAFuncSignature other) const;

};
