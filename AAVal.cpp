#include "AAVal.h"
#include <sstream>
#include <iomanip>
#include "AAVM.h"
#include "astring.h"
#include "AAMemoryStore.h"

AAVal AAVal::Null = AAVal(AA_Literal(AA_AnyLiteral::AA_AnyLiteral(), AALiteralType::Null));

std::wstring AAVal::ToString() {
	if (this->ptr.val) {
		return aa::wstring_hex(this->ptr.val);
	} else {
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

void AAO_ToString(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {
	
	// Convert to argument list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	// Is it a pointer value
	if (argl.First().ptr != 0) {

		// Push hexadecimal string representation of memory address onto pointer (Not the actual memory address)
		stack.Push(AAVal(aa::wstring_hex(argl.First().ptr.val)));

	} else {

		// Push string representation of primitive unto stack
		stack.Push(AAVal(argl.First().ToString()));

	}

}

void AAO_NewObject(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {

	// Convert to argument list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	// Push string representation of primitive unto stack
	stack.Push(argl.First());

}
