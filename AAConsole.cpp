#include "AAConsole.h"
#include "AAExecute.h"
#include <iostream>

void AAConsole_PrintLn(AAVM* pAAVm, any_stack& stack) {
	AAStackValue top = stack.Pop<AAStackValue>();
	if (top.get_type() == AAPrimitiveType::refptr) {
		std::wcout << top.ToString() << L"\n";
	} else {
		printf("");
	}
}
