#pragma once
#include "AAFuncSignature.h"
#include "set.h"

struct AAClassOperatorSignature {
	std::wstring op;
	AAFuncSignature* method;
	AAClassOperatorSignature() {
		this->op = L"";
		this->method = 0;
	}
	AAClassOperatorSignature(std::wstring op, AAFuncSignature* method) {
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
	aa::set<AAFuncSignature*> methods;
	aa::set<AAClassOperatorSignature> operators;
	aa::set<AAClassFieldSignature> fields;
	aa::set<AAClassSignature*> extends; // (The class(es) from which we inherit from)
	
	size_t classByteSz;
	
	AAAccessModifier accessModifier;
	AAStorageModifier storageModifier;
	AACNamespace* domain;
	AACType* type;

	AAClassSignature() {
		this->name = L"";
		this->classByteSz = 0;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->storageModifier = AAStorageModifier::NONE;
		this->domain = 0;
		this->type = new AACType(this);
	}
	
	AAClassSignature(std::wstring name) {
		this->name = name;
		this->classByteSz = 0;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->storageModifier = AAStorageModifier::NONE;
		this->domain = 0;
		this->type = new AACType(this);
	}

	/// <summary>
	/// Get the full name of the class (domain name and class name)
	/// </summary>
	/// <returns></returns>
	std::wstring GetFullname(); // Definition in AACNamespace.cpp

	/// <summary>
	/// Does class derive from the class
	/// </summary>
	/// <param name="other">The type to determine if class is a derivitve of</param>
	/// <returns>True iff this derives from other</returns>
	bool DerivesFrom(AAClassSignature* other) {
		if (this->extends.Contains([other](AAClassSignature*& sig) { return sig == other || sig->DerivesFrom(other); })) {
			return true;
		} else {
			return false;
		}
	}

	bool operator==(AAClassSignature other) {
		return other.name.compare(this->name) == 0 && other.domain == this->domain;
	}
	

};
