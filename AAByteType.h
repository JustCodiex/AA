#pragma once
#include "array.h"
#include <stdint.h>
#include <string>

/// <summary>
/// Type variant for a byte type (class, enum, interface, or object
/// </summary>
enum class AAByteTypeVariant : unsigned char {

	Val = 1 << 0, // Value (primitive) type

	Ref = 1 << 1, // Reference (class) type

	Enum = 1 << 2, // Enum type

	Interface = 1 << 3, // Interface type

	Object = 1 << 4, // Static class/object type

};

typedef std::pair<uint32_t, uint16_t> _vtablefunc;
typedef std::pair<uint16_t, aa::array<_vtablefunc>> _vtableelem;
typedef aa::array<_vtableelem> _vtable;

struct AAByteVTable {
	_vtable vtable;
};

/// <summary>
/// Type represented in byte format
/// </summary>
struct AAByteType {

	static const int _BasePtrObj = 0; // Value if type is to inherit from Object
	static const int _BasePtrObjNone = UINT16_MAX; // Value if the type is to inherit from nothing

	wchar_t* typeName; // full name of the type
	AAByteTypeVariant typeVariant; // Type variant

	uint16_t baseTypePtr; // Pointer to base type (0 if only inherit from object)

	unsigned char inheritancePtrCount; // inheritance pointer
	uint16_t* otherInheritancePtr; // other non-base inheritance pointers

	uint32_t constID; // constant identifier of the type

	uint16_t unmanagedSize; // The unmanaged size

	AAByteVTable* vtable; // The VTable associated with the type

	// Because of how the compiler fetches data, we can collect method pointers and such!

	AAByteType() {
		this->typeName = 0;
		this->typeVariant = AAByteTypeVariant::Ref; // Use reference as default
		this->baseTypePtr = _BasePtrObjNone;
		this->inheritancePtrCount = 0;
		this->otherInheritancePtr = 0;
		this->constID = 0;
		this->vtable = 0;
		this->unmanagedSize = 0;
	}

	AAByteType(std::wstring name) {
		this->typeName = new wchar_t[name.length()];
		wcscpy(this->typeName, name.c_str());
		this->typeVariant = AAByteTypeVariant::Ref; // Use reference as default
		this->baseTypePtr = _BasePtrObjNone;
		this->inheritancePtrCount = 0;
		this->otherInheritancePtr = 0;
		this->constID = 0;
		this->vtable = 0;
		this->unmanagedSize = 0;
	}

	bool IsEqual(const AAByteType& other) const {
		return IsSameName(other) && this->baseTypePtr == other.baseTypePtr
			&& this->inheritancePtrCount == other.inheritancePtrCount && this->otherInheritancePtr == other.otherInheritancePtr;
	}

	bool IsSameName(const AAByteType& other) const {
		return (this->typeName != 0 && other.typeName != 0 && wcscmp(this->typeName, other.typeName) == 0);
	}

	unsigned char* ToBytes(uint32_t& size) const;

};
