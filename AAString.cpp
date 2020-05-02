#include "AAString.h"
#include "AAExecute.h"
#include "AAMemoryStore.h"

void AAString_Ctor(AAVM* pAAVm, any_stack& stack) {

	// Convert argument stack into a list
	//aa::list<AAStackValue> argl = aa::ToArgumentList(args);

	// Update internal _str field to be correctly assigned
	//argl.At(0).obj->values[0] = argl.At(1);

	// Push object onto stack
	//stack.Push(argl.First());

	// Exit function
	return;

}

void AAString_Length(AAVM* pAAVm, any_stack& stack) {

	// Convert argument stack into a list
	/*aa::list<AAStackValue> argl = aa::ToArgumentList(args);

	// Make sure it's a string
	if (argl.First().is_string()) {

		// Push to stack
		stack.Push(argl.First().ToString().length());

	} else {
		// throw err
	}*/

	AAStackValue ws = aa::vm::PopSomething(AAPrimitiveType::refptr, stack);
	AAMemoryPtr ptr = ws.to_cpp<AAMemoryPtr>();

	const AAString* pString = dynamic_cast<const AAString*>(ptr.get_object());

	if (pString) {
		aa::vm::PushSomething(AAStackValue(pString->Length()), stack);
	} else {
		aa::vm::PushSomething(AAStackValue(0), stack);
	}

}

void AAString_Concat(AAVM* pAAVm, any_stack& stack) {

	// Get strings to concat
	std::wstring a = (dynamic_cast<const AAString*>(aa::vm::PopSomething(AAPrimitiveType::refptr, stack).to_cpp<AAMemoryPtr>().get_object()))->ToString();
	std::wstring b = (dynamic_cast<const AAString*>(aa::vm::PopSomething(AAPrimitiveType::refptr, stack).to_cpp<AAMemoryPtr>().get_object()))->ToString();

	// Get the concatted strings
	std::wstring c = a + b;

	// Register concatted string
	AAMemoryPtr ptr = pAAVm->GetHeap()->AllocString(c);

	// Push
	aa::vm::PushSomething(AAStackValue(ptr), stack);

}

AAString::AAString(std::wstring ws) : AAObject(0) {

	m_length = ws.length();
	m_content = new wchar_t[m_length + 1];
	_wcsnset(m_content, '\0', m_length+1);
	wcscpy(m_content, ws.c_str());

}

void AAString::Release() {

	if (m_content) {
		delete[]m_content;
		m_content = 0;
	}

	delete this;
}

std::wstring AAString::ToString() const {
	return std::wstring(this->m_content, this->m_length);
}

size_t AAString::Length() const {
	return this->m_length;
}
