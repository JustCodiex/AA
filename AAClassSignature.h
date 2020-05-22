#pragma once
#include "AAFuncSignature.h"
#include "set.h"
#include <unordered_map>

/// <summary>
/// Class operator signature (Maps the operator to a function)
/// </summary>
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
	bool operator==(AAClassOperatorSignature other) const {
		return other.method == this->method;
	}
};

/// <summary>
/// Class field signature (name, type, and access level)
/// </summary>
struct AAClassFieldSignature {
	std::wstring name;
	AACType* type;
	int fieldID;
	bool tagged;
	AAAccessModifier accessModifier;
	AAClassFieldSignature() {
		this->name = L"";
		this->type = AACType::ErrorType;
		this->fieldID = -1;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->tagged = false;
	}
	bool operator==(AAClassFieldSignature other) const {
		return other.name.compare(this->name) == 0;
	}
};

/// <summary>
/// Class property signature (name, type, get & set functions, and access level)
/// </summary>
struct AAClassPropertySignature {
	std::wstring name;
	AACType* type;
	int propertyID;
	AAAccessModifier accessModifier;
	AAFuncSignature* getPropertyFunction;
	AAFuncSignature* setPropertyFunction;
	AAClassPropertySignature() {
		this->name = L"";
		this->type = AACType::ErrorType;
		this->propertyID = -1;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->getPropertyFunction = 0;
		this->setPropertyFunction = 0;
	}
	bool operator==(AAClassPropertySignature other) const {
		return other.name.compare(this->name) == 0;
	}
};

struct AAClassVirtualTable; // Forward declaration

/// <summary>
/// A static signature representation of a class
/// </summary>
struct AAClassSignature {
	
	std::wstring name;
	aa::set<AAFuncSignature*> methods;
	aa::set<AAClassOperatorSignature> operators;
	aa::set<AAClassFieldSignature> fields;
	aa::set<AAClassPropertySignature> properties;
	aa::set<AAClassSignature*> extends; // (The class(es) from which the class instance inherits from)
	unsigned int basePtr;

	size_t classByteSz;
	
	AA_AST_NODE* pSourceNode; // Defining AST node (May be NULL, should only ever be used in compile phase)

	AAAccessModifier accessModifier;
	AAStorageModifier storageModifier;
	AACNamespace* domain;
	AACType* type;

	AAClassVirtualTable* classVTable;

	AAClassSignature() {
		this->name = L"";
		this->classByteSz = 0;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->storageModifier = AAStorageModifier::NONE;
		this->domain = 0;
		this->type = new AACType(this);
		this->pSourceNode = 0;
		this->basePtr = 0;
		this->classVTable = 0;
	}
	
	AAClassSignature(std::wstring name) {
		this->name = name;
		this->classByteSz = 0;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->storageModifier = AAStorageModifier::NONE;
		this->domain = 0;
		this->type = new AACType(this);
		this->pSourceNode = 0;
		this->basePtr = 0;
		this->classVTable = 0;
	}

	void CreateVTable();

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
	bool DerivesFrom(AAClassSignature* other);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="functionalSignature"></param>
	/// <returns></returns>
	AAFuncSignature* FindMethodFromFunctionalSignature(std::wstring functionalSignature);

	bool operator==(AAClassSignature other) {
		return other.name.compare(this->name) == 0 && other.domain == this->domain;
	}

};

/// <summary>
/// Virtual table for a class
/// </summary>
struct AAClassVirtualTable {

	// All virtual functions and their respective pointers
	std::unordered_map<std::wstring, aa::list<std::pair<AAClassSignature*, AAFuncSignature*>>> virtualFunctions;

	// The class signature this is a virtual table for
	AAClassSignature* virtualTableClass;

	AAClassVirtualTable(AAClassSignature* associatedClass) {
		this->virtualTableClass = associatedClass;
	}

	/// <summary>
	/// Adds a function as a virtual function
	/// </summary>
	/// <param name="pSignature">Virtual function to add to VTable</param>
	bool AddVirtualFunction(AAFuncSignature* pSignature);

	/// <summary>
	/// Register some function in the VTable
	/// </summary>
	/// <param name="pMethodOwner">The class that owns the function</param>
	/// <param name="pMethodSignature">The method to register in the VTable</param>
	/// <returns></returns>
	bool RegisterFunction(AAClassSignature* pMethodOwner, AAFuncSignature* pMethodSignature);

};
