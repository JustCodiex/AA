#include "AAConsole.h"
#include "AAExecute.h"
#include <iostream>

void AAConsole_PrintLn(AAVM* pAAVm, any_stack& stack) {
	std::wcout << stack.Pop<AAStackValue>().ToString() << L"\n";
}
