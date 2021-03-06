#include "AAPrimitiveType.h"
#include "AAMemory.h"
#include "AACType.h"
#include "AAIntrPtr.h"
#include "AACEnum.h"
#include "AATuple.h"
#include "AAClosure.h"

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
				return sizeof(AAIntPtr);
			case AAPrimitiveType::string:
			case AAPrimitiveType::refptr:
				return sizeof(AAMemoryPtr);
			case AAPrimitiveType::tuple:
				return sizeof(AATuple);
			case AAPrimitiveType::closure:
				return sizeof(AAClosure);
			default:
				return 0;
			}
		}

		const AAPrimitiveType runtimetype_from_statictype(AACType* pStaticType) {
			if (pStaticType == AACTypeDef::Int32) {
				return AAPrimitiveType::int32;
			} else if (pStaticType == AACTypeDef::UInt32) {
				return AAPrimitiveType::uint32;
			} else if (pStaticType == AACTypeDef::Int16) {
				return AAPrimitiveType::int16;
			} else if (pStaticType == AACTypeDef::UInt16) {
				return AAPrimitiveType::uint16;
			} else if (pStaticType == AACTypeDef::Int64) {
				return AAPrimitiveType::int64;
			} else if (pStaticType == AACTypeDef::UInt64) {
				return AAPrimitiveType::uint64;
			} else if (pStaticType == AACTypeDef::SByte) {
				return AAPrimitiveType::sbyte;
			} else if (pStaticType == AACTypeDef::Byte) {
				return AAPrimitiveType::byte;
			} else if (pStaticType == AACTypeDef::Bool) {
				return AAPrimitiveType::boolean;
			} else if (pStaticType == AACTypeDef::Char) {
				return AAPrimitiveType::wchar;
			} else if (pStaticType == AACTypeDef::Float32) {
				return AAPrimitiveType::real32;
			} else if (pStaticType == AACTypeDef::Float64) {
				return AAPrimitiveType::real64;
			} else if (pStaticType == AACTypeDef::String) {
				return AAPrimitiveType::string;
			} else if (pStaticType == AACTypeDef::IntPtr) {
				return AAPrimitiveType::intptr;
			} else {
				if (pStaticType->isArrayType || pStaticType->isRefType) {
					return AAPrimitiveType::refptr;
				} else if (pStaticType->isEnum) {
					switch (pStaticType->enumSignature->literalType) {
					case AALiteralType::Char:
						return AAPrimitiveType::wchar;
					default:
						return AAPrimitiveType::int32;
					}
				} else if (pStaticType->isTupleType) {
					return AAPrimitiveType::tuple;
				} else if (pStaticType->isLambdaType) {
					return AAPrimitiveType::closure;
				} else {
					return AAPrimitiveType::Undefined;
				}
			}
		}

		bool is_primitive_type(AACType* pStaticType) {
			AAPrimitiveType p = runtimetype_from_statictype(pStaticType);
			if (p != AAPrimitiveType::closure && p != AAPrimitiveType::refptr && p != AAPrimitiveType::Undefined) {
				return true;
			} else {
				return (p == AAPrimitiveType::Undefined) ? false : (pStaticType->isVMType);
			}
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
				return L"intptr";
			case AAPrimitiveType::string:
				return L"string";
			case AAPrimitiveType::refptr:
				return L"refptr";
			case AAPrimitiveType::tuple:
				return L"tuple";
			case AAPrimitiveType::closure:
				return L"closure";
			default:
				return L"nil";
			}
		}

	}

}
