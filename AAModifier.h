#pragma once

/// <summary>
/// The access modifiers
/// </summary>
enum class AAAccessModifier : unsigned char {

	// Anything can access
	PUBLIC = 'u',

	// Only the class can access
	PRIVATE = 'v',

	// Public to subtypes, private to others
	PROTECTED = 'p',

	// Internally visible to module
	INTERNAL = 'i',

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
	/// Overrides a function
	/// </summary>
	OVERRIDE = 'o',

};
