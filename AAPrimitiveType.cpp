#include "AAPrimitiveType.h"
#include "AAMemory.h"
#include "AACType.h"

namespace aa {

	namespace runtime {

		const size_t size_of_type(const AAPrimitiveType& type) {
			switch (type) {
			case AAPrimitiveType::boolean:
				return 1;
			case AAPrimitiveType::byte:
			case AAPrimitiveType::sbyte:
				return 1;
			case AAPrimitiveType::wchar:
			case AAPrimitiveType::uint16:
			case AAPrimitiveType::int16:
				return 2;
			case AAPrimitiveType::uint32:
			case AAPrimitiveType::int32:
			case AAPrimitiveType::real32:
				return 4;
			case AAPrimitiveType::uint64:
			case AAPrimitiveType::int64:
			case AAPrimitiveType::real64:
				return 8;
			case AAPrimitiveType::intptr:
				return sizeof(uint32_t);
			case AAPrimitiveType::string:
			case AAPrimitiveType::refptr:
				return sizeof(AAMemoryPtr);
			default:
				return 0;
			}
		}

		const AAPrimitiveType runtimetype_from_statictype(AACType* pStaticType) {
			if (pStaticType == AACTypeDef::Int32) {
				return AAPrimitiveType::int32;
			} else if (pStaticType == AACTypeDef::Bool) {
				return AAPrimitiveType::boolean;
			} else if (pStaticType == AACTypeDef::Char) {
				return AAPrimitiveType::wchar;
			} else if (pStaticType == AACTypeDef::Float32) {
				return AAPrimitiveType::real32;
			} else if (pStaticType == AACTypeDef::String) {
				return AAPrimitiveType::string;
			} else {
				return AAPrimitiveType::Undefined;
			}
		}

		bool is_primitive_type(AACType* pStaticType) {
			return runtimetype_from_statictype(pStaticType) != AAPrimitiveType::Undefined;
		}

		std::wstring name_of_type(const AAPrimitiveType& type) {
			switch (type) {
			case AAPrimitiveType::boolean:
				return L"bool";
			case AAPrimitiveType::byte:
				return L"byte";
			case AAPrimitiveType::sbyte:
				return L"sbyte";
			case AAPrimitiveType::wchar:
				return L"wide-char";
			case AAPrimitiveType::uint16:
				return L"uint16";
			case AAPrimitiveType::int16:
				return L"int16";
			case AAPrimitiveType::uint32:
				return L"uint32";
			case AAPrimitiveType::int32:
				return L"int32";
			case AAPrimitiveType::real32:
				return L"real32";
			case AAPrimitiveType::uint64:
				return L"uint64";
			case AAPrimitiveType::int64:
				return L"int64";
			case AAPrimitiveType::real64:
				return L"real64";
			case AAPrimitiveType::intptr:
				return L"intpte";
			case AAPrimitiveType::string:
				return L"string";
			case AAPrimitiveType::refptr:
				return L"refptr";
			default:
				return L"nil";
			}
		}

	}

}
