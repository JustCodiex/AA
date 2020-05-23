#include "AATuple.h"
#include "AAIntrPtr.h"
#include "AAMemory.h"
#include "astring.h"
#include <sstream>

AATuple::AATuple(const aa::array<AAPrimitiveType>& types, const aa::array<AAVal>& values) {
	tupleValues = aa::array<TupleValue>(types.length());
	for (size_t i = 0; i < types.length(); i++) {
		tupleValues[i] = TupleValue(types[i], values[i]);
	}
}

const int AATuple::Size() const {
	return (int)tupleValues.length();
}

const AAPrimitiveType AATuple::TypeAt(const size_t& index) const {
	return tupleValues[index].type;
}

const AAVal AATuple::ValueAt(const size_t& index) const {
	return tupleValues[index].val;
}

#pragma region val to string converter
std::wstring __convert_val_to_string(AAPrimitiveType p, AAVal v) {
	switch (p) {
	case AAPrimitiveType::byte:
		return std::to_wstring(v.Raw<unsigned char>());
	case AAPrimitiveType::sbyte:
		return std::to_wstring(v.Raw<signed char>());
	case AAPrimitiveType::boolean:
		return (v.Raw<bool>()) ? L"true" : L"false";
	case AAPrimitiveType::int16:
		return std::to_wstring(v.Raw<int16_t>());
	case AAPrimitiveType::uint16:
		return std::to_wstring(v.Raw<uint16_t>());
	case AAPrimitiveType::int32:
		return std::to_wstring(v.Raw<int32_t>());
	case AAPrimitiveType::uint32:
		return std::to_wstring(v.Raw<uint32_t>());
	case AAPrimitiveType::int64:
		return std::to_wstring(v.Raw<int64_t>());
	case AAPrimitiveType::uint64:
		return std::to_wstring(v.Raw<uint64_t>());
	case AAPrimitiveType::real32:
		return std::to_wstring(v.Raw<float_t>());
	case AAPrimitiveType::real64:
		return std::to_wstring(v.Raw<double_t>());
	case AAPrimitiveType::wchar:
		return std::to_wstring(v.Raw<wchar_t>());
	case AAPrimitiveType::string:
	case AAPrimitiveType::refptr:
		return v.Raw<AAMemoryPtr>().get_memory_address_wstr();
	case AAPrimitiveType::intptr:
		return aa::wstring_hex(v.Raw<AAIntPtr>().ptr);
	case AAPrimitiveType::tuple:
		return v.Raw<AATuple>().ToString();
	case AAPrimitiveType::__TRUEANY:
		return L"_";
	case AAPrimitiveType::Undefined:
	default:
		break;
	}
	return L"Undefined";
}
#pragma endregion

const std::wstring AATuple::ToString() const {

	// String to build
	std::wstringstream wss;

	// Add opener
	wss << L"(";

	// Loop through all types
	for (size_t i = 0; i < tupleValues.length(); i++) {
		wss << __convert_val_to_string(this->TypeAt(i), this->ValueAt(i));
		if (i < tupleValues.length() - 1) {
			wss << ",";
		}
	}

	// Add ender
	wss << L")";

	// Return the string
	return wss.str();

}

bool AATuple::MatchTuple(AATuple matchon, AATuple matchwith) {

	if (matchwith.Size() == matchon.Size()) {
		for (int i = 0; i < matchwith.Size(); i++) {
			if (matchwith.TypeAt(i) == matchon.TypeAt(i)) {
				if (!matchwith.ValueAt(i).Equals(matchon.ValueAt(i), matchwith.Size())) {
					return false;
				}
			} else {
				if (matchon.TypeAt(i) != AAPrimitiveType::__TRUEANY) {
					return false;
				}
			}
		}
		return true;
	} else {
		return false;
	}

}
