#include "AAVal.h"
#include <sstream>
#include <iomanip>

AAVal AAVal::Null = AAVal(AA_Literal(AA_AnyLiteral::AA_AnyLiteral(), AALiteralType::Null));

std::wstring AAVal::ToString() {
	if (this->obj) {
		int addr = (int)&this->obj;
		std::wstringstream ws;
		ws << L"0x" << std::setfill((wchar_t)'0') << std::setw(sizeof(int) * 2) << std::hex << addr;
		return ws.str();
	}
	switch (this->litVal.tp) {
	case AALiteralType::Boolean:
		return (this->litVal.lit.b.val) ? L"true" : L"false";
	case AALiteralType::Float:
		return std::to_wstring(this->litVal.lit.f.val);
	case AALiteralType::Int:
		return std::to_wstring(this->litVal.lit.i.val);
	case AALiteralType::String:
		return std::wstring(this->litVal.lit.s.val);
	case AALiteralType::Char:
		return std::wstring(1, this->litVal.lit.c.val);
	default:
		return L"Null";
	}
} 

// Allocates a new AAObject
AAObject* AllocAAO(size_t sz) {

	// Create new AAO
	AAObject* aao = new AAObject;

	// Set size of object
	aao->valCount = sz / sizeof(AAVal);

	// Allocate variables tied to the object
	aao->values = new AAVal[aao->valCount];

	// Return the object
	return aao;

}
