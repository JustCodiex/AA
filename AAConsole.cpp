#include "AAConsole.h"
#include "AAExecute.h"
#include <iostream>

void AAConsole_PrintLn(AAVM* pAAVm, any_stack& stack) {
	std::wcout << aa::vm::PopSomething(AAPrimitiveType::refptr, stack).ToString() << std::endl;
}
