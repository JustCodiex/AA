#include "AAMemoryStore.h"
#include "AAObject.h"
#include "AAVal.h"

AAMemoryStore::AAMemoryStore(int chunksz) {
	this->m_chunksz = chunksz;
	this->m_nextPtr = 1;
	this->m_slots.reserve(m_chunksz);
	this->m_pObjectTypeEnv = 0;
}

void AAMemoryStore::Release() {

	// Release all the memory used
	for (auto& slot : this->m_slots) {
		this->DeleteAt(slot.first);
	}

	// Clear the slots
	this->m_slots.clear();

	// Release the type environment
	if (m_pObjectTypeEnv) {
		m_pObjectTypeEnv->Release();
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

void AAMemoryStore::SetStaticTypeEnvironment(AAStaticTypeEnvironment* pTypeEnv) {
	m_pObjectTypeEnv = pTypeEnv;
}
