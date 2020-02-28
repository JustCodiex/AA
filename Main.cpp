// Å.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AAVM.h"

int main() {

    AAVM* VM = AANewVM();

    // Compile and execute
    VM->Execute(VM->CompileExpressionToFile(L"15+5+7;", L"out\\math1.aab"));

    VM->Release();

    return 1;

}
