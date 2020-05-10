#pragma once
#include "AAObject.h"
#include <unordered_map>

/// <summary>
/// 
/// </summary>
class AAStaticTypeEnvironment {

public:

	AAStaticTypeEnvironment(int count);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	AAObjectType* LookupType(const uint32_t& index);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="ws"></param>
	/// <returns></returns>
	AAObjectType* LookupType(const std::wstring& ws);

	/// <summary>
	/// Releases the type data
	/// </summary>
	void Release();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="type"></param>
	void InsertType(const uint32_t uid, AAObjectType* pType);

private:

	std::unordered_map<uint32_t, AAObjectType*> m_types;
	std::unordered_map<std::wstring, uint32_t> m_typeNamePointers;

};
