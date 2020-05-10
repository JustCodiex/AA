#pragma once
#include <stdint.h>
#include <string>

/// <summary>
/// Type represented in byte format
/// </summary>
struct AAByteType {

	static const int _BasePtrObj = 0; // Value if type is to inherit from Object
	static const int _BasePtrObjNone = UINT16_MAX; // Value if the type is to inherit from nothing

	wchar_t* typeName; // full name of the type

	uint16_t baseTypePtr; // Pointer to base type (0 if only inherit from object)

	unsigned char inheritancePtrCount; // inheritance pointer
	uint16_t* otherInheritancePtr; // other non-base inheritance pointers

	uint32_t constID;

	// Because of how the compiler fetches data, we can collect method pointers and such!

	AAByteType() {
		this->typeName = 0;
		this->baseTypePtr = _BasePtrObjNone;
		this->inheritancePtrCount = 0;
		this->otherInheritancePtr = 0;
		this->constID = 0;
	}

	AAByteType(std::wstring name) {
		this->typeName = new wchar_t[name.length()];
		wcscpy(this->typeName, name.c_str());
		this->baseTypePtr = _BasePtrObjNone;
		this->inheritancePtrCount = 0;
		this->otherInheritancePtr = 0;
		this->constID = 0;
	}

	bool IsEqual(const AAByteType& other) const {
		return IsSameName(other) && this->baseTypePtr == other.baseTypePtr
			&& this->inheritancePtrCount == other.inheritancePtrCount && this->otherInheritancePtr == other.otherInheritancePtr;
	}

	bool IsSameName(const AAByteType& other) const {
		return (this->typeName != 0 && other.typeName != 0 && wcscmp(this->typeName, other.typeName) == 0);
	}

	unsigned char* ToBytes(uint32_t& size) {
		
		// Calculate string sizes
		uint32_t nameLength = wcslen(this->typeName) + 1;
		uint32_t nameByteSize = nameLength * sizeof(wchar_t);
		
		// Calculate byte size
		size = (sizeof(uint16_t) * ((uint32_t)inheritancePtrCount + 1)) + sizeof(uint32_t) + sizeof(unsigned char) + nameByteSize;
		
		// Allocate bytes and write all 0's
		unsigned char* bytes = new unsigned char[size];
		memset(bytes, 0, size);

		// Write data
		memcpy(bytes, typeName, nameByteSize);
		memcpy(bytes + nameByteSize, &baseTypePtr, sizeof(uint16_t));
		memcpy(bytes + nameByteSize + sizeof(uint16_t), &constID, sizeof(uint32_t));
		memcpy(bytes + nameByteSize + sizeof(uint16_t) + sizeof(uint32_t), &inheritancePtrCount, sizeof(unsigned char));
		memcpy(bytes + nameByteSize + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(unsigned char), &otherInheritancePtr, sizeof(uint16_t) * inheritancePtrCount);
		
		// Return byte array
		return bytes;

	}

};
