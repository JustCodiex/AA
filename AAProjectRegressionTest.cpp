#include "AAProjectRegressionTest.h"

void TestMathProject(AAVM* pAAVM, int& s, int& f) {

	if (!pAAVM->CompileProject(L"examples\\projects\\mathfuncs.aaproject", L"out\\op\\projects\\mathfuncs\\", L"out\\unparsed\\projects\\mathfuncs\\")) {
		f++;
	} else {
		s++;
	}

}

bool RunProjectTests(AAVM* pAAVM) {

	int fail = 0;
	int success = 0;

	// Test the math project
	TestMathProject(pAAVM, success, fail);

	return fail == 0;

}
