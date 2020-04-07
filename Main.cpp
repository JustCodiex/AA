// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <Windows.h>
#include "AAVM.h"
#include "AALanguageRegressionTest.h"

///////////////////
// Global Flags
///////////////////

bool enableRegTests = false; // Run regression tests
bool outputAssembly = false; // Output an assembly file as well as a binary file
bool isCompileInput = false; // The input is a compile target
bool executeOutput = false; // Should we execute our output
bool logCompileTime = false; // Should compile time be logged
bool logExecuteTime = false; // Should execute time be logged
bool printOutput = true; // Should execution output be logged
bool pauseOnComplete = false; // Should we pause execution when VM is released?

///////////////////
// Global Strings
///////////////////

std::wstring vmpath = L""; // The runtime environment
std::wstring inputFile = L""; // The input file
std::wstring outputFile = L""; // The output file

void setupenvironment() {

    // Set console output
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Set overall locale ==> This should not be constant for public builds
    setlocale(LC_ALL, "en_US.utf8");

}

std::wstring getAssemblyOutputFile() {
    if (outputAssembly) {
        return  outputFile.substr(0, outputFile.find_last_of('.')) + L".txt";
    } else {
        return L"";
    }
}

void compileandexecute(AAVM* pAAVM) {

    // Compile and run the file
    pAAVM->CompileAndRunFile(inputFile, outputFile, getAssemblyOutputFile());

}

void compileoutput(AAVM* pAAVM) {

    // Compile to file
    //pAAVM->CompileFileToFile(ipu)

}

int wmain(int argc, wchar_t** argv) {

    // Set runtime environment
    if (argc > 0) {
        vmpath = std::wstring(argv[0]);
    } else {
        return -1;
    }

    // Setup the environment to work within
    setupenvironment();

    // Print VM data
    printf("Å Virtual Machine. V1.0\n");

    // index
    int i = 1;

    // Run through all the elements
    while(i < argc) {
        
        if (wcscmp(argv[i], L"-ga") == 0) {
            outputAssembly = true;
        } else if (wcscmp(argv[i], L"-lc") == 0) {
            logCompileTime = true;
        } else if (wcscmp(argv[i], L"-le") == 0) {
            logExecuteTime = true;
        } else if (wcscmp(argv[i], L"-silent") == 0) {
            printOutput = false;
        } else if (wcscmp(argv[i], L"-pause") == 0) {
            pauseOnComplete = true;
        } else if (wcscmp(argv[i], L"-test_regressive") == 0) {
            enableRegTests = true;
        } else if (wcscmp(argv[i], L"-c") == 0) {
            isCompileInput = true;
            if (i + 1 < argc && argv[i+1][0] != '-') {
                inputFile = argv[i + 1];
                i++;
            } else {
                wprintf(L"Invalid or missing command argument '-c'");
            }
        } else if (wcscmp(argv[i], L"-oae") == 0) {
            if (isCompileInput) {
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    outputFile = argv[i + 1];
                    executeOutput = true;
                    i++;
                } else {
                    wprintf(L"Invalid or missing command argument '-oae'");
                }
            } else {
                wprintf(L"Unable to execute non-binary output");
            }
        } else {
            wprintf(L"!- Unknown or invalid argument '%s'\n", argv[i]);
        }

        i++;

    }

    // Create the VM we'll be using
    AAVM* VM = AAVM::CreateNewVM(logExecuteTime, logCompileTime, printOutput);

    // Set output stream
    VM->SetOutput(&std::cout);

    // Is it compiler input?
    if (isCompileInput) {

        if (executeOutput) {
            compileandexecute(VM);
        } else {
            //compileoutput(VM);
        }

    } else {

        wprintf(L"Functionality not implemented! (Run code without compiling)\n");

    }

    // Run regression tests
    if (enableRegTests) {
        if (!RunRegressionTests(VM)) {
            wprintf(L"One or more regression tests failed!\n");
            system("pause");
            return -1;
        }
    }

    // Release the virtual machine
    VM->Release();

    // Pause if told to
    if (pauseOnComplete) {

        // Allow user to see output
        system("pause"); // Todo: Make cross-platform

    }

    return EXIT_SUCCESS;

}
