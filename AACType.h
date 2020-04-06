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

	/// <summary>
	/// Void type (Not a real type)
	/// </summary>
	static AACType* Void;
	
	/// <summary>
	/// Null type (Not a real type)
	/// </summary>
	static AACType* Null;
	
	/// <summary>
	/// Any type is allowed
	/// </summary>
	static AACType* Any;
	
	/// <summary>
	/// Error type - returned when the type is incorrect (Abort ASAP when detected)
	/// </summary>
	static AACType* ErrorType;
	
};

struct AACTypeDef {

	/// <summary>
	/// Integer (32-bit AKA 4 byte) primitive type
	/// </summary>
	static AACType* Int32;

	/// <summary>
	/// Boolean type
	/// </summary>
	static AACType* Bool;

	/// <summary>
	/// Float (32-bit AKA 4 byte) primitive type
	/// </summary>
	static AACType* Float32;

	/// <summary>
	/// Unsigned char, 8-bit (1 byte)
	/// </summary>
	static AACType* Char;

	/// <summary>
	/// String type (Definition in AAVM.Cpp)
	/// </summary>
	static AACType* String;

};
