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
enum class AAStorageModifier : unsigned char {

	/// <summary>
	/// No modifier
	/// </summary>
	NONE = '\0',

	/// <summary>
	/// Can be overwritten
	/// </summary>
	VIRTUAL = 'v',

	/// <summary>
	/// Overrides a function (iff maked virtual)
	/// </summary>
	OVERRIDE = 'o',

	/// <summary>
	/// Tagged class
	/// </summary>
	TAGGED = 't',

	/// <summary>
	/// Static member
	/// </summary>
	STATIC = 's',

	/// <summary>
	/// Abstract type
	/// </summary>
	ABSTRACT = 'a',

	/// <summary>
	/// Sealed class type
	/// </summary>
	SEALED = 'z',

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
