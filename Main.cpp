// Å.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AAVM.h"

int main() {

    AAVM* VM = AANewVM();

    // Set output stream
    VM->SetOutput(&std::cout);

    // Compile and execute
    VM->Execute(VM->CompileExpressionToFile(L"15+5+7;", L"out\\math_add.aab"));
    VM->Execute(VM->CompileExpressionToFile(L"15+5-7;", L"out\\math_sub.aab"));
    VM->Execute(VM->CompileExpressionToFile(L"15*5;", L"out\\math_mul.aab"));
    VM->Execute(VM->CompileExpressionToFile(L"15/5;", L"out\\math_div.aab"));
    VM->Execute(VM->CompileExpressionToFile(L"15%5;", L"out\\math_mod.aab"));

    VM->Release();

    return 1;

}
