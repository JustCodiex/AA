#pragma once
#include <string>
#include "list.h"

struct AACNamespace;
struct AAClassSignature;
struct AACEnumSignature;

/// <summary>
/// AA Compile-Time Static Type
/// </summary>
struct AACType {

	std::wstring name;
	bool isRefType;
	bool isArrayType;
	bool isTupleType;
	bool isEnum;
	bool isVMType;
	AAClassSignature* classSignature;
	AACEnumSignature* enumSignature;

	aa::list<AACType*> encapsulatedTypes;

	uint32_t constantID; // The unique ID associated with the type

	AACType();
	AACType(std::wstring name);
	AACType(AAClassSignature* classSignature);
	AACType(AACEnumSignature* enumSignature);
	AACType(std::wstring name, aa::list<AACType*> tupleSignature); // sets isTupleType to true!

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

	/// <summary>
	/// Get the first encapsulated type
	/// </summary>
	/// <returns>Returns the first encapsulated type (ErrorType if none)</returns>
	AACType* GetEncapsulatedType();

	/// <summary>
	/// Get the n'th encapsulated type
	/// </summary>
	/// <param name="index">The index of the type</param>
	/// <returns></returns>
	AACType* GetEncapsulatedType(size_t index);

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
	
	/// <summary>
	/// Type used to mark a reference to self in an exported context (See AAVM.Cpp for use)
	/// </summary>
	static AACType* ExportReferenceType;

};

struct AACTypeDef {

	/// <summary>
	/// Signed Short (16-bit AKA 2 byte) primitive type
	/// </summary>
	static AACType* Int16;

	/// <summary>
	/// Signed Integer (32-bit AKA 4 byte) primitive type
	/// </summary>
	static AACType* Int32;

	/// <summary>
	/// Signed Long (64-bit AKA 8 byte) primitive type
	/// </summary>
	static AACType* Int64;

	/// <summary>
	/// Unsigned Integer (16-bit AKA 2 byte) primitive type
	/// </summary>
	static AACType* UInt16;

	/// <summary>
	/// Unsigned Integer (32-bit AKA 4 byte) primitive type
	/// </summary>
	static AACType* UInt32;

	/// <summary>
	/// Unsigned Integer (64-bit AKA 8 byte) primitive type
	/// </summary>
	static AACType* UInt64;

	/// <summary>
	/// Boolean type
	/// </summary>
	static AACType* Bool;

	/// <summary>
	/// Unsigned Byte (8-bit AKA 1 byte) primitive type
	/// </summary>
	static AACType* Byte;

	/// <summary>
	/// Signed Byte (8-bit AKA 1 byte) primitive type
	/// </summary>
	static AACType* SByte;

	/// <summary>
	/// Float (32-bit AKA 4 byte) primitive type
	/// </summary>
	static AACType* Float32;

	/// <summary>
	/// Double (64-bit AKA 8 byte) primitive type
	/// </summary>
	static AACType* Float64;

	/// <summary>
	/// Unsigned wide char, 16-bit (2 byte)
	/// </summary>
	static AACType* Char;

	/// <summary>
	/// String type (Definition in AAVM.Cpp)
	/// </summary>
	static AACType* String;

	/// <summary>
	/// Integer pointer (Raw C++ pointer) 8 bytes, primitive type
	/// </summary>
	static AACType* IntPtr;

};
