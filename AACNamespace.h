#pragma once
#include "set.h"
#include "AAClassSignature.h"
#include <string>

struct AACNamespace {

	std::wstring name;
	AACNamespace* parentspace;
	aa::set<AACNamespace*> childspaces;
	aa::set<AAClassSignature> classes;
	aa::set<AAFuncSignature> functions;
	aa::set<std::wstring> types;

	AACNamespace() {
		this->name = L"INVALID SPACE";
		this->parentspace = (AACNamespace*)0xfffffff;
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
	/// 
	/// </summary>
	/// <returns></returns>
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

	/// <summary>
	/// Moves the entire namespace into the given namespace
	/// </summary>
	/// <param name="otherspace">The other space to move to</param>
	void UsingNamespace(AACNamespace* otherspace) {
		
		// For all types in our namespace, "copy" them into the otherspace
		//this->types.ForEach([otherspace](std::wstring t) { otherspace->types.Add(t); });

	}

	bool AddFunction(AAFuncSignature& sig) {
		sig.domain = this;
		return this->functions.Add(sig);
	}

	bool AddClass(AAClassSignature& sig) {
		sig.domain = this;
		return this->classes.Add(sig);
	}

	bool AddType(std::wstring type) {
		return this->types.Add(type);
	}

	bool operator==(AACNamespace other) {
		return other.name == this->name;
	}

};
