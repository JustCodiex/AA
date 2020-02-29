// Å.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AAVM.h"

int main() {

    AAVM* VM = AANewVM();

    // Set output stream
    VM->SetOutput(&std::cout);

    // Current test case
    VM->Execute(VM->CompileExpressionToFile(L"5+(-5);", L"out\\math_negate2.aab"));

    // Compile and execute
    VM->Execute(VM->CompileExpressionToFile(L"-5;", L"out\\math_negate1.aab")); // Exepcted output = -5
    VM->Execute(VM->CompileExpressionToFile(L"((((5+6))));", L"out\\math_paranthesis3.aab")); // Expected output = 11
    VM->Execute(VM->CompileExpressionToFile(L"((15+5)*42)-(5/2);", L"out\\math_paranthesis2.aab")); // Expected output = 838
    VM->Execute(VM->CompileExpressionToFile(L"(15+5)*42-(5/2);", L"out\\math_paranthesis.aab")); // Expected output = 838
    VM->Execute(VM->CompileExpressionToFile(L"15+5+7;", L"out\\math_add.aab"));
    VM->Execute(VM->CompileExpressionToFile(L"15+5-7;", L"out\\math_sub.aab"));
    VM->Execute(VM->CompileExpressionToFile(L"15*5;", L"out\\math_mul.aab"));
    VM->Execute(VM->CompileExpressionToFile(L"15/5;", L"out\\math_div.aab"));
    VM->Execute(VM->CompileExpressionToFile(L"15%5;", L"out\\math_mod.aab"));

    VM->Release();

    return 1;

}
