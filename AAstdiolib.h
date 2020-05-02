#pragma once
#include "AAVM.h"

#pragma region std

#pragma region io

#pragma region filestream 

void AAFileStream_Open(AAVM* pAAVm, any_stack& stack);
void AAFileStream_Close(AAVM* pAAVm, any_stack& stack);
void AAFileStream_Write(AAVM* pAAVm, any_stack& stack);

#pragma endregion
#pragma endregion
#pragma endregion
