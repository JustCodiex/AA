#pragma once
#include "AAVM.h"
#include <iostream>

void AAConsole_PrintLn(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {

	// Convert argument stack into a list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	// Print to console
	std::wcout << argl.At(0).ToString() << std::endl;

	return;

}
