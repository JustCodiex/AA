#pragma once
#include <string>
#include <unordered_map>
#include "set.h"
#include "AA_literals.h"
#include "AAFuncSignature.h"

struct AACNamespace;

struct AACEnumValue {

	std::wstring name;
	int64_t value;

	AACEnumValue() {
		this->name = L"INVALID ENUM VALUE";
		this->value = 0;
	}

	AACEnumValue(std::wstring name, int64_t v) {
		this->name = name;
		this->value = v;
	}

	bool operator==(AACEnumValue other) {
		return this->value == other.value;
	}

};

struct AACEnumSignature {

	std::wstring name;
	aa::set<AACEnumValue> values;

	AALiteralType literalType;
	AAAccessModifier accessModifier;
	AACNamespace* domain;
	AACType* type;

	aa::set<AAFuncSignature*> functions;

	AACEnumSignature() {
		this->name = L"INVALID_ENUM";
		this->literalType = AALiteralType::Int;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->domain = 0;
		this->type = 0;
	}

	AACEnumSignature(std::wstring name) {
		this->name = name;
		this->literalType = AALiteralType::Int;
		this->accessModifier = AAAccessModifier::PUBLIC;
		this->domain = 0;
		this->type = new AACType(this);
	}

	/// <summary>
	/// Get the full name of the class (domain name and class name)
	/// </summary>
	/// <returns></returns>
	std::wstring GetFullname(); // Definition in AACNamespace.cpp

	/// <summary>
	/// Check if the enum has a value of the given name
	/// </summary>
	/// <param name="val">Value name to ehck</param>
	/// <returns></returns>
	bool HasValue(std::wstring val) {
		return this->values.Contains([val](AACEnumValue& v) { return v.name.compare(val) == 0; });
	}

	int64_t GetValue(std::wstring val) {
		int i;
		if (this->values.FindFirstIndex([val](AACEnumValue& v) { return v.name.compare(val) == 0; }, i)) {
			return this->values.Apply(i).value;
		} else {
			return 0;
		}
	}

};
