#include "AAMemoryStore.h"
#include "AAVal.h"

AAMemoryStore::AAMemoryStore(int chunksz) {
	this->m_chunksz = chunksz;
	this->m_nextPtr = 1;
	this->m_slots.reserve(m_chunksz);
}

void AAMemoryStore::Release() {

	// Destroy stuff here

	delete this;

}

size_t AAMemoryStore::NextPointer() {
	return ++m_nextPtr;
}

void AAMemoryStore::Reference(AAMemoryPtr ptr) {
	this->m_slots[ptr].refcount++;
}

bool AAMemoryStore::Dereference(AAMemoryPtr ptr) {
	this->m_slots[ptr].refcount--;
	return this->m_slots[ptr].refcount == 0;
}

AAMemoryPtr AAMemoryStore::Alloc(size_t sz) {

	// Get the next pointer
	AAMemoryPtr ptr = this->NextPointer();

	// Set slot
	MemorySlot slot;
	slot.refcount = 1;
	slot.isMutable = true;
	slot.obj = AllocAAO(sz);

	// Assign slot
	this->m_slots[ptr] = slot;

	// Return the ptr
	return ptr;

}

AAObject* AAMemoryStore::operator[](const AAMemoryPtr& ptr) const {
	auto slot = this->m_slots.find(ptr);
	if (slot != this->m_slots.end()) {
		return (*slot).second.obj;
	} else {
		return NULL;
	}
}
