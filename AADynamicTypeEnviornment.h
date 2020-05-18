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
	/// Find type in environment
	/// </summary>
	/// <param name="format">The formal string format to find</param>
	/// <param name="typeMapper">The mapping function that maps a string representation of a type into it's full typename</param>
	/// <returns>The found type. If not found, ErrorType</returns>
	AACType* FindType(std::wstring format, TypeMapFunction typeMapper);

	/// <summary>
	/// Add type to environment
	/// </summary>
	/// <param name="format">The formal string format to add</param>
	/// <param name="typeMapper">The mapping function that maps a string representation of a type into it's full typename</param>
	/// <returns>The static type represnetation of the formal string</returns>
	AACType* AddType(std::wstring format, TypeMapFunction typeMapper);

	/// <summary>
	/// Find or add the type if it was not found
	/// </summary>
	/// <param name="format">The formal string format to find or add if not found</param>
	/// <param name="typeMapper">The mapping function that maps a string representation of a type into it's full typename</param>
	/// <returns>The found or added type</returns>
	AACType* FindOrAddTypeIfNotFound(std::wstring format, TypeMapFunction typeMapper);

	/// <summary>
	/// Add a type to the dynamic environment if not already found
	/// </summary>
	/// <param name="format">The formal string format to add if not found</param>
	/// <param name="typeMapper">The mapping function that maps a string representation of a type into it's full typename</param>
	void AddTypeIfNotFound(std::wstring format, TypeMapFunction typeMapper);

	/// <summary>
	/// Is the given type considered to be a valid dynamic type
	/// </summary>
	/// <param name="pType">The type to check for dynamic validity</param>
	/// <returns>True if valid and dynamic. False if none of the conditions hold true</returns>
	bool IsValidDynamicType(AACType* pType);

	/// <summary>
	/// Unpack the formalized tuple into a list of types of which the tuple is made of.
	/// </summary>
	/// <param name="format">The formal tuple format to unpack</param>
	/// <param name="typeMapper">The mapping function that maps a string representation of a type into it's full typename</param>
	/// <returns>List of types making up the tuple</returns>
	aa::list<AACType*> UnpackTuple(std::wstring format, TypeMapFunction typeMapper);

	/// <summary>
	/// Releases sthe dynamic environment
	/// </summary>
	void Release();

	/// <summary>
	/// Wipes clean all the registered types
	/// </summary>
	void WipeClean();

private:

	std::map<std::wstring, AACType*> m_registeredTypes;

};

namespace aa {
	namespace type {

		/// <summary>
		/// Formalize a AA_AST_NODE into it's respective tuple variant.
		/// </summary>
		/// <param name="pTupleNode">The node to convert into a tuple (Must be either of type tupleval or tupletypeidentifier)</param>
		/// <param name="typenameFunction">The mapping function that maps a string representation of a type into it's full typename</param>
		/// <returns>AST node in formalized tuple format</returns>
		std::wstring FormalizeTuple(AA_AST_NODE* pTupleNode, TypeMapFunction typenameFunction);

		/// <summary>
		/// Formalize a list of types into a tuple representation
		/// </summary>
		/// <param name="types">Tuple list</param>
		/// <returns>Formalized tuple representation of input list</returns>
		std::wstring FormalizeTuple(aa::list<AACType*> types);

	}
}
