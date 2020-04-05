#pragma once
#include <string>

struct AACNamespace;
struct AAClassSignature;

struct AACType {

	std::wstring name;
	bool isRefType;
	bool isArrayType;
	AACType* encapsulatedType;
	AAClassSignature* classSignature;

	AACType();
	AACType(std::wstring name);
	AACType(AAClassSignature* classSignature);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	AACType* AsArrayType();

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	AACType* GetArrayType();

	/// <summary>
	/// Get the full name of the type (domain name and class name)
	/// </summary>
	/// <returns></returns>
	std::wstring GetFullname(); // Definition in AACNamespace.cpp

	bool operator==(AACType other) {
		bool n = other.name.compare(this->name) == 0;
		bool a = this->isArrayType == other.isArrayType;
		if (isRefType && other.isRefType) {
			return n && a && this->classSignature == other.classSignature;
		} else {
			return n && a;
		}
	}

	bool operator!=(AACType other) {
		return !(*this == other);
	}

	static AACType* Void;
	static AACType* Null;
	static AACType* Any;
	static AACType* ErrorType;
	
};

struct AACTypeDef {
	static AACType* Int32;
	static AACType* Bool;
	static AACType* Float32;
	static AACType* Char;
	static AACType* String;
};