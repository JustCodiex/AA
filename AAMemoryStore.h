#pragma once
#include "AAMemory.h"
#include "AAArray.h"
#include "AAString.h"
#include "AAPrimitiveType.h"
#include "AARuntimeTypeEnv.h"

class AAMemoryStore {

public:

	struct MemorySlot {
		int refcount;
		AAObject* obj;
		MemorySlot() {
			this->refcount = 0;
			this->obj = 0;
		}
	};

public:

	AAMemoryStore(int chunksz);

	void Release();

	void Reference(AAMemoryPtr ptr);
	bool Dereference(AAMemoryPtr ptr);

	void SetStaticTypeEnvironment(AAStaticTypeEnvironment* pObjects); // This might not be a needed function afterall!

	AAMemoryPtr Alloc(size_t sz);
	AAMemoryPtr AllocArray(AAPrimitiveType primitiveType, uint32_t dimCount, uint32_t* dimensionLengths);
	AAMemoryPtr AllocString(std::wstring str);

	AAObject* Object(const AAMemoryPtr& ptr) const;
	AAArray* Array(const AAMemoryPtr& ptr) const;
	AAString* String(const AAMemoryPtr& ptr) const;

private:

	uint64_t NextPointer();

	void DeleteAt(AAMemoryPtr ptr);

private:

	int m_chunksz;
	uint64_t m_nextPtr;

	AAStaticTypeEnvironment* m_pObjectTypeEnv;

	std::unordered_map<AAMemoryPtr, MemorySlot> m_slots; // TODO: Replace with custom structure later

};
