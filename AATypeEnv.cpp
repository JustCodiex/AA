#include "AATypeEnv.h"

AATypeEnv::AATypeEnv() {}

AATypeEnv::AATypeEnv(const AATypeEnv& copy) {
	this->m_vtenv = std::map<AAId, AACType*>(copy.m_vtenv);
	this->m_itenv = std::map<AAId, AACType*>(copy.m_itenv);
}

AACType* AATypeEnv::AddVariable(const AAId& id, AACType* pType) {
	return (this->m_vtenv[id] = pType);
}

void AATypeEnv::AddInferVariable(const AAId& id) {
	this->m_itenv[id] = AACType::Any;
	this->AddVariable(id, AACType::Any);
}

bool AATypeEnv::UpdateInferredVariableType(const AAId& id, AACType* pType) {
	if (this->m_itenv[id] == AACType::Any) {
		this->m_itenv[id] = pType;
		return true;
	} else {
		return this->m_itenv[id] == pType;
	}
}

AACType* AATypeEnv::GetInferredVariable(const AAId& id) {
	return this->m_itenv[id];
}

bool AATypeEnv::FindType(const AAId& id, AACType*& pType) {
	if (id.compare(L"_") == 0) {
		pType = AACType::Any;
		return true;
	}
	if (this->m_vtenv.find(id) != this->m_vtenv.end()) {
		pType = this->m_vtenv[id];
		return true;
	} else {
		pType = AACType::ErrorType;
		return false;
	}
}

AACType* AATypeEnv::Lookup(const AAId& id) {
	AACType* pType = 0;
	if (!this->FindType(id, pType)) {
		wprintf(L"failed to find type %s", id.c_str());
	}
	return pType;
}
