#pragma once
#include <string>

/// <summary>
/// Primitive Type for AA values
/// </summary>
enum class AAPrimitiveType : unsigned char { // 1 byte

	Undefined = 0,

	byte, // unsigned char => unsigned byte [0-255]
	sbyte, // signed char => signed byte [-127-127]

	boolean,

	int16,
	uint16,
	int32,
	uint32,
	int64,
	uint64,

	real32,
	real64,

	wchar,
	string,

	refptr, // never expose (AAMemoryPtr)
	intptr, // never expose (void*)

};

struct AACType;

namespace aa {

	namespace runtime {

		/// <summary>
		/// 
		/// </summary>
		/// <param name="type"></param>
		/// <returns></returns>
		const size_t size_of_type(const AAPrimitiveType& type);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="pStaticType"></param>
		/// <returns></returns>
		const AAPrimitiveType runtimetype_from_statictype(AACType* pStaticType);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="pStaticType"></param>
		/// <returns></returns>
		bool is_primitive_type(AACType* pStaticType);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="type"></param>
		/// <returns></returns>
		std::wstring name_of_type(const AAPrimitiveType& type);

	}

}
