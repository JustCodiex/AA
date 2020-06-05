#include "AAProjectRegressionTest.h"

void TestMathProject(AAVM* pAAVM, int& s, int& f) {

	// Compile the project
	if (!pAAVM->CompileProject(L"examples\\projects\\mathfuncs.aaproject", L"out\\op\\projects\\", L"out\\unparsed\\projects\\")) {
		f++;
	} else {

		// Execute the file
		pAAVM->RunFile(L"examples\\projects\\binary\\mathfuncs.aab");

		// Did we get a runtime error?
		if (pAAVM->HasRuntimeError()) {
			f++;
		} else {
			s++;
		}

	}

}

bool RunProjectTests(AAVM* pAAVM) {

	int fails = 0;
	int successes = 0;

	// Get start time
	clock_t start = clock();

	// Test the math project
	TestMathProject(pAAVM, successes, fails);
	
	// Log regression test results
	std::wcout << L"\nProject regression test report:" << L"\n";
	std::wcout << std::to_wstring(successes + fails) << L" Completed, " << std::to_wstring(successes) << L" succeeded, " << std::to_wstring(fails) << L" failed." << L"\n";
	std::wcout << L"Tests completed in " << std::to_wstring((float)(clock() - start) / CLOCKS_PER_SEC) << L"s" << L"\n\n";

	return fails == 0;

}
