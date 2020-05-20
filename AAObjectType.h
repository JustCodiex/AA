#pragma once
#include "AAVTable.h"
#include <stdint.h>
#include <string>

/// <summary>
/// Object type that contains all the metadata around a specific object
/// </summary>
class AAObjectType {

public:

	AAObjectType();

	AAObjectType(std::wstring _typename, const uint16_t& size, const uint32_t& typeID, const uint16_t& baseID);

	/// <summary>
	/// Get the constant unique ID given to the type (at runtime)
	/// </summary>
	/// <returns></returns>
	uint32_t GetTypeID() const;

	/// <summary>
	/// Set the inheritance type IDs
	/// </summary>
	/// <param name="count">Amount of types</param>
	/// <param name="intPtrs">Type indices</param>
	void SetInheritance(const unsigned char& count, uint16_t* intPtrs);

	/// <summary>
	/// Get the unmanaged size of the object
	/// </summary>
	/// <returns></returns>
	const size_t GetSize() const;

	/// <summary>
	/// Is a tagged class type (Can it be deconstructed)
	/// </summary>
	/// <returns></returns>
	bool IsTaggedType();

	/// <summary>
	/// Is type an instance of another type (sub type)
	/// </summary>
	/// <param name="pBaseType"></param>
	/// <returns></returns>
	bool IsInstanceOf(AAObjectType* pBaseType);

	/// <summary>
	/// Retrieve the name of the type
	/// </summary>
	/// <returns>Full name of type</returns>
	const std::wstring GetName() const;

	/// <summary>
	/// Set the VTable associated with the type
	/// </summary>
	/// <param name="pVtable">The type</param>
	void SetVTable(AAVTable* pVtable);

	/// <summary>
	/// Does the object type have a VTable?
	/// </summary>
	/// <returns>True if VTable is found</returns>
	const bool HasVTable() const;

private:

	std::wstring m_typename;
	uint32_t m_typeID;

	uint16_t m_size;

	int16_t m_taggedFieldCount;
	size_t* m_taggedFieldTypes;

	uint16_t m_baseTypeIndex;
	AAObjectType* m_baseType;

	unsigned char m_otherInheritCount;
	uint16_t* m_otherInheritPtrs;

	AAVTable* m_vtable;

	friend class AAObject;

};
