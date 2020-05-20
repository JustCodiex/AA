#include "AAVTable.h"

void AAVTable::SetFunctionPtr(const uint16_t& vfunc, const uint32_t& classPtr, const uint16_t& vFuncOverride) {
	if (this->m_vtablePtr.find(vfunc) == this->m_vtablePtr.end()) {
		this->m_vtablePtr[vfunc] = std::unordered_map<uint32_t, uint16_t>();
	}
	this->m_vtablePtr[vfunc][classPtr] = vFuncOverride;
}

uint16_t AAVTable::GetFunctionPtr(const uint16_t& vFunc, const uint32_t& Ptr) const {
	return this->m_vtablePtr[vFunc][Ptr];
}
