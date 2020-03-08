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

    // Run regression tests
    if (enableRegTests) {
        if (!RunRegressionTests(VM)) {
            return -1;
        }
    }

    // Current test case
        // none    
    
    // Release the virtual machine
    VM->Release();

    // Allow user to see output
    system("pause");

    return 1;

}
