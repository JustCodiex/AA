#pragma once
#include "AACNamespace.h"
#include "AA_AST.h"
#include "list.h"
#include <map>
#include <functional>

class AATypeChecker; // Forward declaration for the type-checker

/// <summary>
/// A function to map a string name to the correct type
/// </summary>
typedef std::function<AACType* (std::wstring)> TypeMapFunction;

/// <summary>
/// Environment for keeping track of dynamically found and created types (tuples, arrays, and generics [anything using the encapsulated types])
/// </summary>
class AADynamicTypeEnvironment {

public:

	AADynamicTypeEnvironment();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="format"></param>
	/// <param name="typeMapper"></param>
	/// <returns></returns>
	AACType* FindType(std::wstring format, TypeMapFunction typeMapper);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="format"></param>
	/// <param name="typeMapper"></param>
	/// <returns></returns>
	AACType* AddType(std::wstring format, TypeMapFunction typeMapper);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="format"></param>
	/// <returns></returns>
	AACType* FindOrAddTypeIfNotFound(std::wstring format, TypeMapFunction typeMapper);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="format"></param>
	/// <returns></returns>
	void AddTypeIfNotFound(std::wstring format, TypeMapFunction typeMapper);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="pType"></param>
	/// <returns></returns>
	bool IsValidDynamicType(AACType* pType);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="format"></param>
	/// <param name="typeMapper"></param>
	/// <returns></returns>
	aa::list<AACType*> UnpackTuple(std::wstring format, TypeMapFunction typeMapper);

	/// <summary>
	/// 
	/// </summary>
	void Release();

	/// <summary>
	/// 
	/// </summary>
	void WipeClean();

private:

	std::map<std::wstring, AACType*> m_registeredTypes;

};

namespace aa {
	namespace type {

		/// <summary>
		/// 
		/// </summary>
		/// <param name="pTupleNode"></param>
		/// <param name="typenameFunction"></param>
		/// <returns></returns>
		std::wstring FormalizeTuple(AA_AST_NODE* pTupleNode, TypeMapFunction typenameFunction);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="types"></param>
		/// <returns></returns>
		std::wstring FormalizeTuple(aa::list<AACType*> types);

	}
}
