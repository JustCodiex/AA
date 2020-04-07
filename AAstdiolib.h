#pragma once
#include "AAVM.h"

#pragma region std

#pragma region io

#pragma region filestream 

void AAFileStream_Open(AAVM* pAAVm, aa::stack<AAVal> args, aa::stack<AAVal>& stack) {

	// Convert argument stack into a list
	aa::list<AAVal> argl = aa::ToArgumentList(args);

	printf("Hello World");

}

#pragma endregion
#pragma endregion
#pragma endregion
