#include "AARuntimeTypeEnv.h"

AAStaticTypeEnvironment::AAStaticTypeEnvironment(int count) {
	m_types.reserve(count);
	m_typeNamePointers.reserve(count);
}

AAObjectType* AAStaticTypeEnvironment::LookupType(const uint32_t& index) {
	return m_types[index];
}

AAObjectType* AAStaticTypeEnvironment::LookupType(const std::wstring& ws) {
	return m_types[m_typeNamePointers[ws]];
}

void AAStaticTypeEnvironment::InsertType(const uint32_t uid, AAObjectType* pType) {
	m_types[uid] = pType;
	m_typeNamePointers[pType->GetName()] = uid;
}

void AAStaticTypeEnvironment::Release() {

	for (auto& kv : m_types) {
		delete kv.second;
	}

	m_typeNamePointers.clear();
	m_types.clear();

	delete this;

}
