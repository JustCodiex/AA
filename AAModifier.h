#pragma once
#include <string>

/// <summary>
/// The access modifiers
/// </summary>
enum class AAAccessModifier : unsigned char {

	/// <summary>
	/// Anything can access
	/// </summary>
	PUBLIC = 'u',

	/// <summary>
	/// Only the class can access
	/// </summary>
	PRIVATE = 'v',

	/// <summary>
	/// Public to subtypes, private to others
	/// </summary>
	PROTECTED = 'p',

	/// <summary>
	/// Internally visible to module
	/// </summary>
	INTERNAL = 'i',

	/// <summary>
	/// Defined externally
	/// </summary>
	EXTERNAL = 'e',

};


/// <summary>
/// The storage modifiers
/// </summary>
enum class AAStorageModifier : long long {

	/// <summary>
	/// No modifier
	/// </summary>
	NONE = 1 << 0,

	/// <summary>
	/// Can be overwritten
	/// </summary>
	VIRTUAL = 1 << 1,

	/// <summary>
	/// Overrides a function (iff maked virtual)
	/// </summary>
	OVERRIDE = 1 << 2,

	/// <summary>
	/// Tagged class
	/// </summary>
	TAGGED = 1 << 3,

	/// <summary>
	/// Static member
	/// </summary>
	STATIC = 1 << 4,

	/// <summary>
	/// Abstract type
	/// </summary>
	ABSTRACT = 1 << 5,

	/// <summary>
	/// Sealed class type
	/// </summary>
	SEALED = 1 << 6,

};

namespace aa {
	
	namespace modifiers {

		/// <summary>
		/// Verified the access modifier combination is legal
		/// </summary>
		/// <param name="mod">Access modifier</param>
		/// <returns>True if valid</returns>
		bool IsLegalAccessModifierCombination(AAAccessModifier mod);

		/// <summary>
		/// Verified the storage modifier combination is legal
		/// </summary>
		/// <param name="mod">Storage modifier</param>
		/// <returns>True if valid</returns>
		bool IsLegalStorageModifierCombination(AAStorageModifier mod);

		/// <summary>
		/// Verified the access and storage modifier combination is legal
		/// </summary>
		/// <param name="aMod">Access modifier</param>
		/// <param name="sMod">Storage modifier</param>
		/// <returns>True if valid</returns>
		bool IsLegalModifierCombination(AAAccessModifier aMod, AAStorageModifier sMod);

		AAStorageModifier operator|(AAStorageModifier a, AAStorageModifier b);
		AAStorageModifier operator&(AAStorageModifier a, AAStorageModifier b);

		/// <summary>
		/// Check if the modifier contains the flag
		/// </summary>
		/// <param name="modifier">The modifier value to check against</param>
		/// <param name="flag">The flag to check for</param>
		/// <returns></returns>
		bool ContainsFlag(AAStorageModifier modifier, AAStorageModifier flag);

	}

	/// <summary>
	/// Returns pretty name of access modifier
	/// </summary>
	/// <param name="mod">Access modifier</param>
	/// <returns>Name of modifier</returns>
	std::wstring NameofAccessModifier(AAAccessModifier mod);

	/// <summary>
	/// Returns pretty name of storage modifier
	/// </summary>
	/// <param name="mod">Storage modifier</param>
	/// <returns>Name of modifier</returns>
	std::wstring NameofStorageModifier(AAStorageModifier mod);

}
