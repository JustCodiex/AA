#pragma once
#include "AAModifier.h"
#include "AAStackValue.h"
#include "stack.h"

/// <summary>
/// Object type that contains all the metadata around a specific object
/// </summary>
class AAObjectType {

public:

	AAObjectType();

	bool IsTaggedType();

	bool IsInstanceOf(AAObjectType* pBaseType);

private:

	int16_t m_taggedFieldCount;
	size_t* m_taggedFieldTypes;

	AAObjectType* m_baseType;

	friend class AAObject;

};

/// <summary>
/// Object in memory
/// </summary>
class AAObject {

public:

	AAObject(size_t szInMemory);

	void SetType(AAObjectType* type);
	void SetInheritance(AAObject* pBase, AAObjectType* type);

	/// <summary>
	/// Releases the data used by the object AND the object itself
	/// </summary>
	virtual void Release();

	/// <summary>
	/// Returns a string representation of the object
	/// </summary>
	/// <returns></returns>
	virtual std::wstring ToString() const;

	/// <summary>
	/// Deconstructs the class (Must be a tagged class type)
	/// </summary>
	/// <returns>True if it was possible to deconstruct.</returns>
	bool Deconstruct();

	template<typename T>
	T* Offset(size_t offset) const {
		if (m_base) {
			if (offset < m_base->GetSize()) {
				return m_base->Offset<T>(offset);
			} else {
				offset -= m_base->GetSize();
			}
		}
		return reinterpret_cast<T*>(m_data + offset);
	}

	void OffsetChunk(unsigned char** dst, size_t offset, size_t cnkSize);

	const size_t GetSize() const;
	const size_t GetBaseOffset() const;

	const bool IsTypeOf(AAObjectType* type) const;

	AAObjectType* GetType() const;

private:

	size_t m_size;
	unsigned char* m_data;

	AAObject* m_base;
	AAObjectType* m_type;

};

namespace aa {

	/// <summary>
	/// 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="obj"></param>
	/// <param name="offset"></param>
	/// <param name="val"></param>
	template<typename T>
	inline void SetVal(const AAObject* obj, size_t offset, const T& val) {
		*(obj->Offset<T>(offset)) = val;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="obj"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	template<typename T>
	inline const T& GetVal(const AAObject* obj, size_t offset) {
		return *(obj->Offset<T>(offset));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="pObject"></param>
	/// <param name="offset"></param>
	/// <param name="value"></param>
	void SetValue(AAObject* pObject, size_t offset, AAStackValue value);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="pObject"></param>
	/// <param name="type"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	const AAStackValue GetValue(AAObject* pObject, AAPrimitiveType type, size_t offset);

}
