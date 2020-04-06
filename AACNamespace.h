#pragma once
#include "AAClassSignature.h"

struct AACNamespace {

	std::wstring name;
	AACNamespace* parentspace;
	aa::set<AACNamespace*> childspaces;
	aa::set<AAClassSignature*> classes;
	aa::set<AAFuncSignature*> functions;
	aa::set<AACType*> types;

	AACNamespace() {
		this->name = L"INVALID SPACE";
		this->parentspace = 0;
	}

	AACNamespace(std::wstring name, AACNamespace* parent) {
		this->name = name;
		this->parentspace = parent;
	}

	/// <summary>
	/// Checks if the namespace is considered to be the global namespace (has no parent and no name)
	/// </summary>
	/// <returns>True if global namespace</returns>
	bool IsGlobalSpace() { return parentspace == 0 && name == L""; }

	/// <summary>
	/// Get the full name of the namespace
	/// </summary>
	/// <returns>Full name path of namespace</returns>
	std::wstring GetNamePath() {
		if (parentspace == 0) {
			return name;
		} else {
			if (parentspace->IsGlobalSpace()) {
				return name;
			} else {
				return parentspace->GetNamePath() + L"::" + name;
			}
		}
	}

	bool AddFunction(AAFuncSignature* sig) {
		sig->domain = this;
		return this->functions.Add(sig);
	}

	bool AddClass(AAClassSignature* sig) {
		sig->domain = this;
		if (this->AddType(sig->type)) {
			return this->classes.Add(sig);
		} else {
			return false;
		}
	}

	bool AddType(AACType* type) {
		return this->types.Add(type);
	}

	AACNamespace* TemporaryDomain(aa::set<AACType*> types, aa::set<AAClassSignature*> classes, aa::set<AAFuncSignature*> functions) {
		
		// New domain
		AACNamespace* domain = new AACNamespace;
		domain->functions = this->functions.Union(functions);
		domain->classes = this->classes.Union(classes);
		domain->types = this->types.Union(types);
		
		// Return the new domain
		return domain;

	}

	bool operator==(AACNamespace other) {
		return other.name == this->name;
	}

};
