// Å.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AAVM.h"
#include "AALanguageRegressionTest.h"

bool enableRegTests = true;

int main() {

    // Create the VM we'll be using
    AAVM* VM = AAVM::CreateNewVM(false, false, true);

    // Set output stream
    VM->SetOutput(&std::cout);

    // Current test case
    VM->CompileAndRunFile(L"testing\\string1.aa", L"out\\bin\\string1.aab", L"out\\op\\string1.txt");

    // Run regression tests
    if (enableRegTests) {
        if (!RunRegressionTests(VM)) {
            return -1;
        }
    }

    std::cout << sizeof(AAVal) << std::endl;
    std::cout << sizeof(AA_Literal) << std::endl;
    std::cout << sizeof(AA_AnyLiteral) << std::endl;
    std::cout << sizeof(AA_NullLiteral) << std::endl;

    // Release the virtual machine
    VM->Release();

    // Allow user to see output
    system("pause");

    return 1;

}
