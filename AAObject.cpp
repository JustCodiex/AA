#include "AAObject.h"
#include "astring.h"
#include "AAVM.h"

#pragma region AAObject

AAObject::AAObject(size_t size) {
	m_size = size;
	m_data = new unsigned char[size];
	m_base = 0;
	m_type = 0;
	memset(m_data, 0, size);
}

void AAObject::SetType(AAObjectType* type) {
	this->m_base = 0;
	this->m_type = type;
}

void AAObject::SetInheritance(AAObject* pBase, AAObjectType* type)  {
	this->m_type = type;
	this->m_base = pBase;
}

void AAObject::Release() {

	// We also release the base
	if (m_base) {
		m_base->Release();
		m_base = 0;
	}

	// Delete data and self
	delete[] m_data;
	delete this;

}

std::wstring AAObject::ToString() const {
	return L"Object";
}

const size_t AAObject::GetSize() const {
	return this->m_size;
}

const size_t AAObject::GetBaseOffset() const {
	if (m_base) {
		return m_base->GetBaseOffset() + m_base->GetSize();
	} else {
		return 0;
	}
}

const bool AAObject::IsTypeOf(AAObjectType* type) const {
	return (m_type) ? (m_type == type) : false;
}

AAObjectType* AAObject::GetType() const {
	return m_type;
}

bool AAObject::Deconstruct() {
	if (m_type->IsTaggedType()) {
		return true;
	} else {
		return false;
	}
}

void AAObject::OffsetChunk(unsigned char** dst, size_t offset, size_t cnkSize) {
	if (m_base) {
		if (offset < m_base->GetSize()) {
			return m_base->OffsetChunk(dst, offset, cnkSize);
		} else {
			offset -= m_base->GetSize();
		}
	}
	memcpy(*dst, m_data + offset, cnkSize);
}

#pragma endregion

#pragma region AAObjectType 

AAObjectType::AAObjectType() {
	m_taggedFieldCount = -1; // Must be -1 if not a tagged class
	m_taggedFieldTypes = 0;
	m_baseType = 0;

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

#pragma endregion

namespace aa {

	void SetValue(AAObject* pObject, size_t offset, AAStackValue value) {

	}

	const AAStackValue GetValue(AAObject* pObject, AAPrimitiveType type, size_t offset) {
		AAStackValue val;
		return val;
	}

}
