#pragma once

// All access modifiers
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
