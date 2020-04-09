#pragma once
#include <unordered_map>
#include "AAMemory.h"

struct AAObject;

class AAMemoryStore {

public:

	struct MemorySlot {
		int refcount;
		bool isMutable;
		AAObject* obj;
		MemorySlot() {
			this->refcount = 0;
			this->isMutable = true;
			this->obj = 0;
		}
	};

public:

	AAMemoryStore(int chunksz);

	void Release();

	void Reference(AAMemoryPtr ptr);
	bool Dereference(AAMemoryPtr ptr);

	AAMemoryPtr Alloc(size_t sz);

	AAObject* operator[](const AAMemoryPtr& ptr) const;

private:

	size_t NextPointer();

	void DeleteAt(AAMemoryPtr ptr);

private:

	int m_chunksz;
	size_t m_nextPtr;

	std::unordered_map<AAMemoryPtr, MemorySlot> m_slots; // TODO: Replace with custom structure later

};
