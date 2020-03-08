#include "AAVal.h"

AAVal AAVal::Null = AAVal(AA_Literal(AA_AnyLiteral::AA_AnyLiteral(), AALiteralType::Null));

std::wstring AAVal::ToString() {
	switch (this->litVal.tp) {
	case AALiteralType::Boolean:
		return (this->litVal.lit.b.val) ? L"true" : L"false";
	case AALiteralType::Float:
		return std::to_wstring(this->litVal.lit.f.val);
	case AALiteralType::Int:
		return std::to_wstring(this->litVal.lit.i.val);
	case AALiteralType::String:
		return std::wstring(this->litVal.lit.s.val);
	default:
		return L"Null";
	}
}
