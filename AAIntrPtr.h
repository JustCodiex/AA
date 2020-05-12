#pragma once
#include <stdint.h>

/// <summary>
/// Integer pointer
/// </summary>
struct AAIntPtr {

	uint64_t ptr;

	AAIntPtr(void* _ptr) {
		this->ptr = 0;
		memcpy(&ptr, &_ptr, sizeof(void*));
	}

	template<typename T>
	T* GetPtr() const {
		void* ptr = 0;
		memcpy(&ptr, &this->ptr, sizeof(void*));
		return (T*)ptr;
	}

};
