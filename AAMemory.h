#pragma once
#include <unordered_map>

class AAObject;
class AAMemoryStore;

/// <summary>
/// Represents a pointer to memory within the AA memory heap space
/// </summary>
struct AAMemoryPtr {
	uint64_t val; // actual pointer
	AAMemoryStore* pMemStore; // ptr to store
	unsigned char ptrType; // What do we point to? (Object, Array, or String)
	// Could potentially store object specific data here (like type ptr)
	AAMemoryPtr() {
		this->val = 0;
		this->pMemStore = 0;
		this->ptrType = 'U';
	}
	AAMemoryPtr(uint64_t ptr) {
		this->val = ptr;
		this->pMemStore = 0;
		this->ptrType = 'U';
	}
	uint64_t operator()(const AAMemoryPtr& ptr) const {
		return ptr.val;
	}
	bool operator==(const AAMemoryPtr& other) const {
		return this->val == other.val;
	}
	bool operator==(const int& i) const {
		return this->val == i;
	}
	bool operator!=(const int& i) const {
		return !(this->val == i);
	}
	bool operator>(const int& i) const {
		return (int)this->val > i;
	}

	/// <summary>
	/// Get the memory addres in wide-string format
	/// </summary>
	/// <returns>Widestring address representation</returns>
	const std::wstring get_memory_address_wstr() const;

	/// <summary>
	/// Get the memory addres in string format
	/// </summary>
	/// <returns>String address representation</returns>
	const std::string get_memory_address_str() const;

	/// <summary>
	/// Set the store used by the memory pointer
	/// </summary>
	/// <param name="pMemStr">The memory store to use</param>
	void set_store(AAMemoryStore* pMemStr);

	/// <summary>
	/// Returns the object that's being pointed to
	/// </summary>
	/// <returns></returns>
	const AAObject* get_object() const;

};

template <>
struct std::hash<AAMemoryPtr> {
	uint64_t operator()(const AAMemoryPtr& k) const {
		return k.val;
	}
};
