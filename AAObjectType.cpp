#include "AAObjectType.h"

AAObjectType::AAObjectType() {
	this->m_typeID = 0;
	this->m_taggedFieldCount = -1; // Must be -1 if not a tagged class
	this->m_taggedFields = 0;
	this->m_baseType = 0;
	this->m_baseTypeIndex = 0;
	this->m_typename = L"Object";
	this->m_vtable = 0; // Null by default
	this->m_otherInheritCount = 0;
	this->m_otherInheritPtrs = 0;
	this->m_size = 0;
}

AAObjectType::AAObjectType(std::wstring _typename, const uint16_t& size, const uint32_t& typeID, const uint16_t& baseID) {
	this->m_typeID = typeID;
	this->m_baseType = 0;
	this->m_baseTypeIndex = baseID;
	this->m_taggedFieldCount = -1; // Must be -1 if not a tagged class
	this->m_taggedFields = 0;
	this->m_typename = _typename;
	this->m_vtable = 0; // Null by default
	this->m_otherInheritCount = 0;
	this->m_otherInheritPtrs = 0;
	this->m_size = size;
}

uint32_t AAObjectType::GetTypeID() const {
	return this->m_typeID;
}

void AAObjectType::SetInheritance(const unsigned char& count, uint16_t* intPtrs) {
	this->m_otherInheritCount = count;
	this->m_otherInheritPtrs = intPtrs;
}

const size_t AAObjectType::GetSize() const {
	return this->m_size;
}

bool AAObjectType::IsTaggedType() {
	return this->m_taggedFieldCount >= 0;
}

bool AAObjectType::IsInstanceOf(AAObjectType* pBaseType) {
	if (m_baseType) {
		return m_baseType == pBaseType || m_baseType->IsInstanceOf(pBaseType);
	} else {
		return this == pBaseType;
	}
}

const std::wstring AAObjectType::GetName() const {
	return m_typename;
}

void AAObjectType::SetVTable(AAVTable* pVtable) {
	m_vtable = pVtable;
}

const bool AAObjectType::HasVTable() const {
	return m_vtable != 0;
}

void AAObjectType::SetTaggedCount(const uint16_t& count) {
	
	// Set count
	m_taggedFieldCount = (uint16_t)count;
	
	// Allocate memory
	m_taggedFields = new TaggedField[m_taggedFieldCount];

}

void AAObjectType::SetTaggedField(const size_t& index, uint16_t fId, unsigned char fType, uint32_t fRefType) {
	m_taggedFields[index] = TaggedField(fId, fType, fRefType);
}

const int32_t AAObjectType::GetTaggedCount() const {
	return m_taggedFieldCount;
}

void AAObjectType::GetTaggedField(const int32_t& index, uint16_t& fId, unsigned char& fType) {
	fId = m_taggedFields[index].fieldId;
	fType = m_taggedFields[index].fieldType;
}

void AAObjectType::GetTaggedField(const int32_t& index, uint16_t& fId, unsigned char& fType, uint32_t fRefType) {
	fId = m_taggedFields[index].fieldId;
	fType = m_taggedFields[index].fieldType;
	fRefType = m_taggedFields[index].fieldRefTypePtr;
}
