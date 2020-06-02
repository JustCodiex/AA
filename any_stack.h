#pragma once
#include <cstring>

/// <summary>
/// Stack object that can contain any element within a given stack size
/// </summary>
class any_stack {

public:

	/// <summary>
	/// Allocate stack
	/// </summary>
	/// <param name="alloc_size">Size in bytes of stack</param>
	/// <returns></returns>
	any_stack(size_t alloc_size) {
		m_top = 0;
		m_size = alloc_size;
		m_bytes = new unsigned char[m_size];
	}

	/// <summary>
	/// Removes the top element on the stack
	/// </summary>
	/// <typeparam name="TValue"></typeparam>
	/// <returns>Element as value</returns>
	template <typename TValue>
	TValue Pop() {
		m_top -= sizeof(TValue);
		return *reinterpret_cast<TValue*>(m_bytes + m_top);
	}

	/// <summary>
	/// Pop the specified amount of bytes
	/// </summary>
	/// <param name="popSz">Amount of bytes to pop</param>
	void Pop(size_t popSz) {
		m_top -= popSz;
	}

	/// <summary>
	/// Push a new element unto the stack
	/// </summary>
	/// <typeparam name="TValue">The type of the value to push</typeparam>
	/// <param name="element">The value to push</param>
	template <typename TValue>
	bool Push(const TValue element) {
		if (this->get_allocated() >= this->get_pointer() + sizeof(TValue)) {
			memcpy(m_bytes + m_top, &element, sizeof(TValue));
			m_top += sizeof(TValue);
			return true;
		} else {
			return false;
		}
	}

	/// <summary>
	/// Get a pointer to the current element at the top of the stack
	/// </summary>
	/// <typeparam name="TValue"></typeparam>
	/// <returns>Pointer to element at the top of stack</returns>
	template<typename TValue>
	TValue* Top() {
		return reinterpret_cast<TValue*>(m_bytes + m_top - sizeof(TValue));
	}

	/// <summary>
	/// Releases all resources used by the stack and the stack itself
	/// </summary>
	void Release(bool releaseSelf) {
		
		// Make sure the array points to something
		if (m_bytes) {

			// Reset stack helpers
			m_size = 0;
			m_top = 0;

			// Clear memory
			delete[] m_bytes;

			// Only do this if asked
			if (releaseSelf) {
				delete this;
			} else {
				m_bytes = 0;
			}

		}

	}

	/// <summary>
	/// Determines if the stack is empty
	/// </summary>
	/// <returns>True iff stack is empty</returns>
	bool is_empty() { return m_top == 0; }

	/// <summary>
	/// Get the pointer
	/// </summary>
	/// <returns>Value of internal top pointer</returns>
	const size_t get_pointer() const { return m_top; }

	/// <summary>
	/// Get allocated amount (actual size in bytes)
	/// </summary>
	/// <returns>allocated byte size in bytes</returns>
	const size_t get_allocated() const { return m_size; }

private:

	size_t m_top;
	size_t m_size;
	unsigned char* m_bytes;

};
