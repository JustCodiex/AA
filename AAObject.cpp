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
	if (m_data) {
		delete[] m_data;
		m_data = 0;
	}

	// Delete self
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
	m_typename = L"Object";
}

AAObjectType::AAObjectType(std::wstring _typename) {
	m_baseType = 0;
	m_taggedFieldCount = -1; // Must be -1 if not a tagged class
	m_taggedFieldTypes = 0;
	m_typename = _typename;
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

#pragma endregion

namespace aa {

	void SetValue(AAObject* pObject, size_t offset, AAStackValue value) {
		switch (value.get_type()) {
		case AAPrimitiveType::boolean:
			*pObject->Offset<bool>(offset) = value.to_cpp<bool>();
			break;
		case AAPrimitiveType::byte:
			*pObject->Offset<unsigned char>(offset) = value.to_cpp<unsigned char>();
			break;
		case AAPrimitiveType::int16:
			*pObject->Offset<int16_t>(offset) = value.to_cpp<int16_t>();
			break;
		case AAPrimitiveType::int32:
			*pObject->Offset<int32_t>(offset) = value.to_cpp<int32_t>();
			break;
		case AAPrimitiveType::int64:
			*pObject->Offset<int64_t>(offset) = value.to_cpp<int64_t>();
			break;
		case AAPrimitiveType::intptr:
			*pObject->Offset<AAIntPtr>(offset) = value.to_cpp<AAIntPtr>();
			break;
		case AAPrimitiveType::real32:
			*pObject->Offset<float_t>(offset) = value.to_cpp<float_t>();
			break;
		case AAPrimitiveType::real64:
			*pObject->Offset<double_t>(offset) = value.to_cpp<double_t>();
			break;
		case AAPrimitiveType::string:
		case AAPrimitiveType::refptr:
			*pObject->Offset<AAMemoryPtr>(offset) = value.to_cpp<AAMemoryPtr>();
			break;
		case AAPrimitiveType::sbyte:
			*pObject->Offset<char>(offset) = value.to_cpp<char>();
			break;
		case AAPrimitiveType::uint16:
			*pObject->Offset<uint16_t>(offset) = value.to_cpp<uint16_t>();
			break;
		case AAPrimitiveType::uint32:
			*pObject->Offset<uint32_t>(offset) = value.to_cpp<uint32_t>();
			break;
		case AAPrimitiveType::uint64:
			*pObject->Offset<uint64_t>(offset) = value.to_cpp<uint64_t>();
			break;
		case AAPrimitiveType::wchar:
			*pObject->Offset<wchar_t>(offset) = value.to_cpp<wchar_t>();
			break;
		case AAPrimitiveType::Undefined:
		default:
			break;
		}
	}

	const AAStackValue GetValue(AAObject* pObject, AAPrimitiveType type, size_t offset) {
		switch (type) {
		case AAPrimitiveType::boolean:
			return AAStackValue(*pObject->Offset<bool>(offset));
		case AAPrimitiveType::byte:
			return AAStackValue(*pObject->Offset<unsigned char>(offset));
		case AAPrimitiveType::int16:
			return AAStackValue(*pObject->Offset<int16_t>(offset));
		case AAPrimitiveType::int32:
			return AAStackValue(*pObject->Offset<int32_t>(offset));
		case AAPrimitiveType::int64:
			return AAStackValue(*pObject->Offset<int64_t>(offset));
		case AAPrimitiveType::intptr:
			return AAStackValue(*pObject->Offset<AAIntPtr>(offset));
		case AAPrimitiveType::real32:
			return AAStackValue(*pObject->Offset<float_t>(offset));
		case AAPrimitiveType::real64:
			return AAStackValue(*pObject->Offset<double_t>(offset));
		case AAPrimitiveType::string:
		case AAPrimitiveType::refptr:
			return AAStackValue(*pObject->Offset<AAMemoryPtr>(offset));
		case AAPrimitiveType::sbyte:
			return AAStackValue(*pObject->Offset<signed char>(offset));
		case AAPrimitiveType::uint16:
			return AAStackValue(*pObject->Offset<uint16_t>(offset));
		case AAPrimitiveType::uint32:
			return AAStackValue(*pObject->Offset<uint32_t>(offset));
		case AAPrimitiveType::uint64:
			return AAStackValue(*pObject->Offset<uint64_t>(offset));
		case AAPrimitiveType::wchar:
			return AAStackValue(*pObject->Offset<wchar_t>(offset));
		case AAPrimitiveType::Undefined:
		default:
			return AAStackValue::None;
		}
	}

}
