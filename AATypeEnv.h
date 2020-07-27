#pragma once
#include "AADynamicTypeEnviornment.h"

typedef std::wstring AAId;

/// <summary>
/// 
/// </summary>
struct AATypeEnv {

public:

	AATypeEnv();
	AATypeEnv(const AATypeEnv& copy);

	AACType* AddVariable(const AAId& id, AACType* pType);

	void AddInferVariable(const AAId& id); // Must also call AddVariable

	bool UpdateInferredVariableType(const AAId& id, AACType* pType);

	AACType* GetInferredVariable(const AAId& id);

	AACType* Lookup(const AAId& id);

	bool FindType(const AAId& id, AACType*& pType);

private:

	std::map<AAId, AACType*> m_vtenv;
	std::map<AAId, AACType*> m_itenv;

};
