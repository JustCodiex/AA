#include "AAStackValue.h"
#include "AAObject.h"
#include "AAString.h"
#include "astring.h"

AAStackValue AAStackValue::None = AAStackValue();
AAStackValue AAStackValue::Null = AAStackValue(AAPrimitiveType::refptr, AAVal(0));

AAStackValue::AAStackValue() { m_type = AAPrimitiveType::Undefined; }
AAStackValue::AAStackValue(AAMemoryPtr ptr) { m_type = AAPrimitiveType::refptr; m_val = AAVal(ptr); }
AAStackValue::AAStackValue(AAIntPtr iPtr) { m_type = AAPrimitiveType::intptr, m_val = AAVal(iPtr); }
AAStackValue::AAStackValue(signed char b) { m_type = AAPrimitiveType::sbyte; m_val = AAVal(b); }
AAStackValue::AAStackValue(unsigned char b) { m_type = AAPrimitiveType::byte; m_val = AAVal(b); }
AAStackValue::AAStackValue(bool b) { m_type = AAPrimitiveType::boolean; m_val = AAVal(b); }
AAStackValue::AAStackValue(uint16_t us) { m_type = AAPrimitiveType::uint16; m_val = AAVal(us); }
AAStackValue::AAStackValue(int16_t s) { m_type = AAPrimitiveType::int16; m_val = AAVal(s); }
AAStackValue::AAStackValue(uint32_t us) { m_type = AAPrimitiveType::uint32; m_val = AAVal(us); }
AAStackValue::AAStackValue(int32_t s) { m_type = AAPrimitiveType::int32; m_val = AAVal(s); }
AAStackValue::AAStackValue(uint64_t us) { m_type = AAPrimitiveType::uint64; m_val = AAVal(us); }
AAStackValue::AAStackValue(int64_t s) { m_type = AAPrimitiveType::int64; m_val = AAVal(s); }
AAStackValue::AAStackValue(std::wstring ws) { m_type = AAPrimitiveType::string; m_val = AAVal(ws); }
AAStackValue::AAStackValue(float r32) { m_type = AAPrimitiveType::real32; m_val = AAVal(r32); }
AAStackValue::AAStackValue(double r64) { m_type = AAPrimitiveType::real64; m_val = AAVal(r64); }
AAStackValue::AAStackValue(wchar_t w) { m_type = AAPrimitiveType::wchar; m_val = AAVal(w); }
AAStackValue::AAStackValue(AATuple t) { m_type = AAPrimitiveType::tuple; m_val = AAVal(t); }

AAStackValue::AAStackValue(AAPrimitiveType type, AAVal val) {
	m_val = val;
	m_type = type;
}

AAStackValue::AAStackValue(AA_Literal lit) {
	switch (lit.tp) {
	case AALiteralType::Boolean:
		this->m_type = AAPrimitiveType::boolean;
		this->m_val = AAVal(lit.lit.b.val);
		break;
	case AALiteralType::Char:
		this->m_type = AAPrimitiveType::wchar;
		this->m_val = AAVal(lit.lit.c.val);
		break;
	case AALiteralType::Float:
		this->m_type = AAPrimitiveType::real32;
		this->m_val = AAVal(lit.lit.f.val);
		break;
	case AALiteralType::Int:
		this->m_type = AAPrimitiveType::int32;
		this->m_val = AAVal(lit.lit.i.val);
		break;
	case AALiteralType::IntPtr:
		this->m_type = AAPrimitiveType::intptr;
		this->m_val = AAVal(lit.lit.ptr.ptr);
		break;
	case AALiteralType::Null:
		this->m_type = AAPrimitiveType::refptr;
		this->m_val = AAVal(AAMemoryPtr(0));
		break;
	case AALiteralType::String:
		this->m_type = AAPrimitiveType::string;
		this->m_val = AAVal(std::wstring(lit.lit.s.val));
		break;
	default:
		throw std::exception();
	}
}

AAPrimitiveType AAStackValue::get_type() const {
	return this->m_type;
}

std::wstring AAStackValue::ToString() {
	switch (m_type) {
	case AAPrimitiveType::boolean:
		return (m_val.Raw<bool>()) ? L"true" : L"false";
	case AAPrimitiveType::byte:
		return std::to_wstring(m_val.Raw<unsigned char>());
	case AAPrimitiveType::int16:
		return std::to_wstring(m_val.Raw<int16_t>());
	case AAPrimitiveType::int32:
		return std::to_wstring(m_val.Raw<int32_t>());
	case AAPrimitiveType::int64:
		return std::to_wstring(m_val.Raw<int64_t>());
	case AAPrimitiveType::intptr:
		return aa::wstring_hex(m_val.Raw<AAIntPtr>().ptr);
	case AAPrimitiveType::real32:
		return std::to_wstring(m_val.Raw<float>());
	case AAPrimitiveType::real64:
		return std::to_wstring(m_val.Raw<double>());
	case AAPrimitiveType::string:
		return m_val.Raw<std::wstring>();
	case AAPrimitiveType::refptr: {
		if (m_val.is_valid()) {
			AAMemoryPtr ptr = m_val.Raw<AAMemoryPtr>();
			if (ptr.ptrType == 'S') {
				const AAString* pStr = dynamic_cast<const AAString*>(ptr.get_object());
				if (pStr) {
					return pStr->ToString();
				} else {
					return aa::wstring_hex(ptr.val);
				}
			} else {
				return aa::wstring_hex(ptr.val);
			}
		} else {
			return aa::wstring_hex(0);
		}
	}
	case AAPrimitiveType::sbyte:
		return std::to_wstring(m_val.Raw<char>());
	case AAPrimitiveType::uint16:
		return std::to_wstring(m_val.Raw<uint16_t>());
	case AAPrimitiveType::uint32:
		return std::to_wstring(m_val.Raw<uint32_t>());
	case AAPrimitiveType::uint64:
		return std::to_wstring(m_val.Raw<uint64_t>());
	case AAPrimitiveType::wchar:
		return std::wstring(1, m_val.Raw<wchar_t>());
	case AAPrimitiveType::tuple:
		return m_val.Raw<AATuple>().ToString();
	default:
		return L"nil";
	}
}

bool AAStackValue::is_string() const { return m_type == AAPrimitiveType::string; }

bool AAStackValue::is_nil() const { return m_type == AAPrimitiveType::Undefined; }

AAVal AAStackValue::as_val() const { return m_val; }

bool AAStackValue::is_same_value(const AAStackValue& value) const {

	if (value.m_type == this->m_type) {
		// TODO: Change more here so we can properly determine this
		return value.m_val.Equals(this->m_val, aa::runtime::size_of_type(m_type));
	} else {
		return false;
	}

}

AAStackValue AAStackValue::change_type(AAPrimitiveType type) { // TODO: This merely changes the type --> It does not convert it properly

	if (this->m_type != type) {
		if (type == AAPrimitiveType::boolean) {
			if (this->m_type == AAPrimitiveType::byte || this->m_type == AAPrimitiveType::sbyte) {
				return !(this->to_cpp<unsigned char>() == 0);
			} else if (this->m_type == AAPrimitiveType::wchar || this->m_type == AAPrimitiveType::int16 || this->m_type == AAPrimitiveType::uint16) {
				return !(this->to_cpp<int16_t>() == 0);
			} else if (this->m_type == AAPrimitiveType::int32 || this->m_type == AAPrimitiveType::uint32) {
				return !(this->to_cpp<int32_t>() == 0);
			} else if (this->m_type == AAPrimitiveType::int64 || this->m_type == AAPrimitiveType::uint64) {
				return !(this->to_cpp<int64_t>() == 0);
			} else {
				return false;
			}
		} else if (type == AAPrimitiveType::real32) {
			if (this->m_type == AAPrimitiveType::int32 || this->m_type == AAPrimitiveType::uint32) {
				return AAStackValue(this->to_cpp<float>());
			}
			// TODO: Add more conversion
		} else if (type == AAPrimitiveType::wchar) {
			if (this->m_type == AAPrimitiveType::int16) {
				return AAStackValue(this->to_cpp<wchar_t>());
			}
			// TODO: Add more conversion
		} else if (type == AAPrimitiveType::string) {
			if (this->m_type == AAPrimitiveType::refptr) {
				return AAStackValue(AAPrimitiveType::string, this->m_val);
			}
		}
	}

	return AAStackValue(*this);

}
