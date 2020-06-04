// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <Windows.h>
#include "AAVM.h"
#ifdef _DEBUG
#include "AALanguageRegressionTest.h"
#include "AAProjectRegressionTest.h"
#endif

///////////////////
// Global Versions
///////////////////
const char* VM_Version = "0.5.0"; // VM version

///////////////////
// Global Flags
///////////////////

bool enableRegTests = false; // Run regression tests
bool enableProTests = false; // Run project compile tests
bool outputAssembly = false; // Output an assembly file as well as a binary file
bool outputUnparsed = false; // Output an unparsed variant of the file
bool isCompileInput = false; // The input is a compile target
bool executeOutput = false; // Should we execute our output
bool executeInput = false; // Should we execute the input
bool logCompileTime = false; // Should compile time be logged
bool logExecuteTime = false; // Should execute time be logged
bool logTrace = false; // Should trace be logged when running the VM
bool printOutput = true; // Should execution output be logged
bool pauseOnComplete = false; // Should we pause execution when VM is released?
bool verifyInput = false; // Should the given bytecode (input) be verified before executed?

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

std::wstring getUnparseOutputFile() {
    if (outputUnparsed) {
        return outputFile.substr(0, outputFile.find_last_of('.')) + L"_unparsed.aa";
    } else {
        return L"";
    }
}

void compileandexecute(AAVM* pAAVM) {

    // Compile and run the file
    pAAVM->CompileAndRunFile(inputFile, outputFile, getAssemblyOutputFile(), getUnparseOutputFile());

}

void compileoutput(AAVM* pAAVM) {

    // Compile to file
    //pAAVM->CompileFileToFile(ipu)

}

bool readArgumentToArgument(int i, int argc, wchar_t** argv, std::wstring& out) {
    if (i + 1 < argc && argv[i + 1][0] != '-') {
        out = argv[i + 1];
        return true;
    }
    return false;
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
    printf("Å Virtual Machine. V%s\n", VM_Version);

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
        } else if (wcscmp(argv[i], L"-trace") == 0) {
#if _DEBUG
            logTrace = true;
#elif
            wprintf(L"-trace is not enabled in RELEASE mode");
#endif
        } else if (wcscmp(argv[i], L"-test_regressive") == 0) {
#if _DEBUG
            enableRegTests = true;
#else
            wprintf(L"Language reggression tests not available");
#endif
        } else if (wcscmp(argv[i], L"-test_projects") == 0) {
#if _DEBUG
            enableProTests = true;
#else
            wprintf(L"Project reggression tests not available");
#endif
        } else if (wcscmp(argv[i], L"-unparse") == 0) {
            outputUnparsed = true;
        } else if (wcscmp(argv[i], L"-c") == 0) { // Compile <input>
            isCompileInput = true;
            if (i + 1 < argc && argv[i+1][0] != '-') {
                inputFile = argv[i + 1];
                i++;
            } else {
                wprintf(L"Invalid or missing command argument '-c'\n");
            }
        } else if (wcscmp(argv[i], L"-verify") == 0) { // Verify before executing
            verifyInput = true;
            wprintf(L"'-verify' command found but not implemented (Either not implemented or outdated VM version)\n");
        } else if (wcscmp(argv[i], L"-r") == 0) { // Run <input>
            if (isCompileInput) {
                wprintf(L"Cannot execute non-binary input '-r' (-r and -o or -oae possibly given, which is not valid)\n");
            } else {
                executeInput = true;
                if (!readArgumentToArgument(i, argc, argv, inputFile)) {
                    wprintf(L"Invalid or missing command argument '-c'\n");
                } else {
                    i++;
                }
            }
        } else if (wcscmp(argv[i], L"-o") == 0) { // Output <output>

        } else if (wcscmp(argv[i], L"-oae") == 0) { // Output and run <output>
            if (isCompileInput) {
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    outputFile = argv[i + 1];
                    executeOutput = true;
                    i++;
                } else {
                    wprintf(L"Invalid or missing command argument '-oae'\n");
                }
            } else {
                wprintf(L"Unable to execute non-binary output\n");
            }
        } else {
            wprintf(L"!- Unknown or invalid argument '%s'\n", argv[i]);
        }

        i++;

    }

    // Create the VM we'll be using
    AAVM* VM = AAVM::CreateNewVM(logExecuteTime, logCompileTime, printOutput);

    // Set trace logging
    VM->EnableTraceLogging(logTrace);

    // Set output stream
    VM->SetOutput(&std::cout);

    // Is it compiler input?
    if (isCompileInput) {

        // Determine if we should also execute the output
        if (executeOutput) {
            compileandexecute(VM);
        } else {
            compileoutput(VM);
        }

    } else {

        wprintf(L"Functionality not implemented! (Run code without compiling)\n");

    }

#if _DEBUG

    // Run regression tests
    if (enableRegTests) {
        if (!RunRegressionTests(VM)) {
            wprintf(L"One or more language regression tests failed!\n");
        }
    }

    // Run project regression tests
    if (enableProTests) {
        if (!RunProjectTests(VM)) {
            wprintf(L"One or more project regression tests failed!\n");
        }
    }

#endif

    // Release the virtual machine
    VM->Release();

    // Pause if told to
    if (pauseOnComplete) {

        // Allow user to see output
        system("pause"); // Todo: Make cross-platform

    }

    return EXIT_SUCCESS;

}
