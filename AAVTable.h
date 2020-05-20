#pragma once
#include <unordered_map>

/// <summary>
/// Virtual table
/// </summary>
class AAVTable {

public:

	void SetFunctionPtr(const uint16_t& vfunc, const uint32_t& classPtr, const uint16_t& vFuncOverride);

	uint16_t GetFunctionPtr(const uint16_t& vFunc, const uint32_t& Ptr) const;

private:

	mutable std::unordered_map<uint16_t, std::unordered_map<uint32_t, uint16_t>> m_vtablePtr;

};
