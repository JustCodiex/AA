#include "AAMemoryStore.h"
#include "astring.h"

void AAMemoryPtr::set_store(AAMemoryStore* pMemStr) {
	this->pMemStore = pMemStr;
}

const AAObject* AAMemoryPtr::get_object() const {
	return this->pMemStore->Object(*this);
}

const std::wstring AAMemoryPtr::get_memory_address_wstr() const {
	return aa::wstring_hex(this->val);
}

const std::string AAMemoryPtr::get_memory_address_str() const {
	return aa::string_hex(this->val);
}
