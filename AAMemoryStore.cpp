#include "AAMemoryStore.h"
#include "AAVal.h"

AAMemoryStore::AAMemoryStore(int chunksz) {
	this->m_chunksz = chunksz;
	this->m_nextPtr = 1;
	this->m_slots.reserve(m_chunksz);
}

void AAMemoryStore::Release() {

	auto itt = this->m_slots.begin(); // I just dont want to lookup the type...
	while ((itt = this->m_slots.begin()) != this->m_slots.end()) {
		this->DeleteAt((*itt).first);
	}

	delete this;

}

size_t AAMemoryStore::NextPointer() {
	return ++m_nextPtr;
}

void AAMemoryStore::DeleteAt(AAMemoryPtr ptr) {

	// Find it
	auto slot = this->m_slots.find(ptr);
	if (slot != this->m_slots.end()) {

		// Delete the object
		AAObject* obj = (*slot).second.obj;
		
		// Delete all values
		delete[] obj->values;

		// Delete the object itself
		delete obj;

		// Get rid of the element
		this->m_slots.erase(slot);

	}

}

void AAMemoryStore::Reference(AAMemoryPtr ptr) {
	this->m_slots[ptr].refcount++;
}

bool AAMemoryStore::Dereference(AAMemoryPtr ptr) {
	this->m_slots[ptr].refcount--;
	return this->m_slots[ptr].refcount == 0; // Return true if object has been dereferenced
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
