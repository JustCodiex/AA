#include "AAMemoryStore.h"
#include "AAObject.h"
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

	// Finally, release ourselves
	delete this;

}

uint64_t AAMemoryStore::NextPointer() {
	return ++m_nextPtr;
}

void AAMemoryStore::DeleteAt(AAMemoryPtr ptr) {

	// Find it
	auto slot = this->m_slots.find(ptr);
	if (slot != this->m_slots.end()) {

		// Release the object and all it's underlying resources
		(*slot).second.obj->Release();

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
	ptr.set_store(this);
	ptr.ptrType = 'O';

	// Set slot
	MemorySlot slot;
	slot.refcount = 1;
	slot.isMutable = true;
	slot.obj = new AAObject(sz);

	// Assign slot
	this->m_slots[ptr] = slot;

	// Return the ptr
	return ptr;

}

AAMemoryPtr AAMemoryStore::AllocArray(AAPrimitiveType primitiveType, uint32_t dimCount, uint32_t* dimensionLengths) {

	// Get next ptr
	AAMemoryPtr ptr = this->NextPointer();
	ptr.set_store(this);
	ptr.ptrType = 'A';

	// Set slot
	MemorySlot slot;
	slot.refcount = 1;
	slot.isMutable = true;
	slot.obj = new AAArray(primitiveType, dimCount, dimensionLengths);

	// Assign slot
	this->m_slots[ptr] = slot;

	// Return ptr
	return ptr;

}

AAMemoryPtr AAMemoryStore::AllocString(std::wstring str) {

	// Get next ptr
	AAMemoryPtr ptr = this->NextPointer();
	ptr.set_store(this);
	ptr.ptrType = 'S';

	// Set slot
	MemorySlot slot;
	slot.refcount = 1;
	slot.isMutable = true;
	slot.obj = new AAString(str);

	// Assign slot
	this->m_slots[ptr] = slot;

	// Return ptr
	return ptr;


}

AAObject* AAMemoryStore::Object(const AAMemoryPtr& ptr) const {
	auto slot = this->m_slots.find(ptr);
	if (slot != this->m_slots.end()) {
		return (*slot).second.obj;
	} else {
		return NULL;
	}
}

AAArray* AAMemoryStore::Array(const AAMemoryPtr& ptr) const {
	auto slot = this->m_slots.find(ptr);
	if (slot != this->m_slots.end()) {
		return dynamic_cast<AAArray*>((*slot).second.obj);
	} else {
		return NULL;
	}
}

AAString* AAMemoryStore::String(const AAMemoryPtr& ptr) const {
	auto slot = this->m_slots.find(ptr);
	if (slot != this->m_slots.end()) {
		return dynamic_cast<AAString*>((*slot).second.obj);
	} else {
		return NULL;
	}
}
