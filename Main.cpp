// Å.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AAVM.h"
#include "AALanguageRegressionTest.h"

bool enableRegTests = true;

int main() {

    AAVM* VM = AAVM::CreateNewVM(false, false, true);

    // Set output stream
    VM->SetOutput(&std::cout);

    // Current test case
    VM->CompileAndRunFile(L"testing\\array2.aa", L"out\\bin\\array2.aab", L"out\\op\\array2.txt");
    VM->CompileAndRunFile(L"testing\\array1.aa", L"out\\bin\\array1.aab", L"out\\op\\array1.txt");

    // Run regression tests
    if (enableRegTests) {
        if (!RunRegressionTests(VM)) {
            return -1;
        }
    }

    // Release the virtual machine
    VM->Release();

    // Allow user to see output
    system("pause");

    return 1;

}
