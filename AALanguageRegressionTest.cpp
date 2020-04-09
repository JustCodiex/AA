#include "AALanguageRegressionTest.h"
#include <iostream>

bool VerifyTestResult(AAVM* pAAVM, std::wstring result, std::wstring expected, bool compile, bool runtime) {
	
	// Did any error occur?
	bool anyCompileError = pAAVM->HasCompileError();
	bool anyRuntimeError = pAAVM->HasRuntimeError();

	if (anyCompileError || anyRuntimeError) {

		// Check compile error
		if (anyCompileError && compile) {
			std::wcout << L"Test Succeeded (Succesfully detected compile error)" << std::endl << std::endl;
			return true;
		} else if (anyCompileError && !compile) {
			std::wcout << L"Test Failed, unexpected compile error." << std::endl << std::endl;
			return false;
		}

		// Check compile error
		if (anyRuntimeError && runtime) {
			std::wcout << L"Test Succeeded (Succesfully detected runtime error)" << std::endl << std::endl;
			return true;
		} else if (anyRuntimeError && !runtime) {
			std::wcout << L"Test Failed, unexpected runtime error." << std::endl << std::endl;
			return false;
		}

		std::wcout << "Test failed unexpectedly" << std::endl << std::endl;
		return false;

	} else {

		// Did we get the same output
		bool success = result.compare(expected) == 0;

		// Did we succeed?
		if (success) {
			std::wcout << L"Test Succeeded ('" << result << L"' == '" << expected << L"')" << std::endl << std::endl;
		} else {
			std::wcout << L"Test Failed, received '" << result << L"' but expected '" << expected << L"')" << std::endl << std::endl;
		}
		
		// Return result
		return success;

	}

}

bool RunFileTest(AAVM* pAAVM, std::wstring fileinput, std::wstring fileoutputBinary, std::wstring fileoutputOpCodes, std::wstring expectedoutput, bool compile, bool runtime) {

	std::wcout << L"Testing file: " << fileinput << std::endl;
	return VerifyTestResult(pAAVM, pAAVM->CompileAndRunFile(fileinput, fileoutputBinary, fileoutputOpCodes).ToString(), expectedoutput, compile, runtime);	

}

bool RunExpressionTest(AAVM* pAAVM, std::wstring expression, std::wstring fileoutputBinary, std::wstring fileoutputOpCodes, std::wstring expectedoutput, bool compile, bool runtime) {

	std::wcout << L"Testing expression: '" << expression << L"'" << std::endl;
	return VerifyTestResult(pAAVM, pAAVM->CompileAndRunExpression(expression, fileoutputBinary, fileoutputOpCodes).ToString(), expectedoutput, compile, runtime);
}

void RunArithmeticTests(AAVM* pAAVM, int& s, int& f) {

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"5+5*5;", L"out\\bin\\math_arithmetic_binding.aab", L"out\\op\\math_arithmetic_binding.txt", L"30")) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"5--5;", L"out\\bin\\math_negate4.aab", L"out\\op\\math_negate4.txt", L"10")) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"5+-5;", L"out\\bin\\math_negate3.aab", L"out\\op\\math_negate3.txt", L"0")) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"5+(-5);", L"out\\bin\\math_negate2.aab", L"out\\op\\math_negate2.txt", L"0")) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"-5;", L"out\\bin\\math_negate1.aab", L"out\\op\\math_negate1.txt", L"-5")) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"((((5+6))));", L"out\\bin\\math_paranthesis3.aab", L"out\\op\\math_paranthesis3.txt", L"11")) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"((15+5)*42)-(5/2);", L"out\\bin\\math_paranthesis2.aab", L"out\\op\\math_paranthesis2.txt", L"838")) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"(15+5)*42-(5/2);", L"out\\bin\\math_paranthesis.aab", L"out\\op\\math_paranthesis.txt", L"838")) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"15+5+7;", L"out\\bin\\math_add.aab", L"out\\op\\math_add.txt", L"27")) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"15+5-7;", L"out\\bin\\math_sub.aab", L"out\\op\\math_sub.txt", L"13")) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"15*5;", L"out\\bin\\math_mul.aab", L"out\\op\\math_mul.txt", L"75")) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"15/5;", L"out\\bin\\math_div.aab", L"out\\op\\math_div.txt", L"3")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"15%5;", L"out\\bin\\math_mod.aab", L"out\\op\\math_mod.txt", L"0")) {
		f++;
	} else {
		s++;
	}

}

void RunVariableDeclerationTests(AAVM* pAAVM, int& s, int& f) {

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"var x = (5+5)*5-2;", L"out\\bin\\variable_var_rhsop2.aab", L"out\\op\\variable_var_rhsop2.txt", L"Null")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"var x = 5+5;", L"out\\bin\\variable_var_rhsop1.aab", L"out\\op\\variable_var_rhsop1.txt", L"Null")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"var x = 5;", L"out\\bin\\variable_var.aab", L"out\\op\\variable_var.txt", L"Null")) {
		f++;
	} else {
		s++;
	}

}

void RunBlockTests(AAVM* pAAVM, int& s, int& f) {

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ float x = 1.5f; x = x + 1.25f; x; }", L"out\\bin\\block_with_type5.aab", L"out\\op\\block_with_type5.txt", L"2.750000")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ bool x = true; !x; }", L"out\\bin\\block_with_type4.aab", L"out\\op\\block_with_type4.txt", L"false")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ float x = 1.5f; x; }", L"out\\bin\\block_with_type3.aab", L"out\\op\\block_with_type3.txt", L"1.500000")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ bool x = true; x; }", L"out\\bin\\block_with_type2.aab", L"out\\op\\block_with_type2.txt", L"true")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ int x = 5; x = x + 5; x; }", L"out\\bin\\block_with_type1.aab", L"out\\op\\block_with_type1.txt", L"10")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ var x = 5; x = x + 5; x; }", L"out\\bin\\block_with_var3.aab", L"out\\op\\block_with_var3.txt", L"10")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ var x = (5+5)*5-2; var y = 20; x - y; }", L"out\\bin\\block_with_var2.aab", L"out\\op\\block_with_var2.txt", L"28")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ var x = (5+5)*5-2; x; }", L"out\\bin\\block_with_var1.aab", L"out\\op\\block_with_var1.txt", L"48")) {
		f++;
	} else {
		s++;
	}

}

void RunFunctionTests(AAVM* pAAVM, int& s, int& f) {

	// Test modulo operator
	if (!RunFileTest(pAAVM, L"testing\\func.aa", L"out\\bin\\func.aab", L"out\\op\\func.txt", L"51")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"int par(int x, int y) { x - y; } par(15+5,par(75,50));", L"out\\bin\\func_decl_test8.aab", L"out\\op\\func_decl_test8.txt", L"-5")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"int par(int x, int y) { x - y; } par(15+5,5);", L"out\\bin\\func_decl_test7.aab", L"out\\op\\func_decl_test7.txt", L"15")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"int par(int x, int y) { x - y; } par(10, 5);", L"out\\bin\\func_decl_test6.aab", L"out\\op\\func_decl_test6.txt", L"5")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"int par(int z) { var x = z; x = x + 28; x; } par(10);", L"out\\bin\\func_decl_test5.aab", L"out\\op\\func_decl_test5.txt", L"38")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"int test() { var x = 0; x = x + 28; x; } test();", L"out\\bin\\func_decl_test4.aab", L"out\\op\\func_decl_test4.txt", L"28")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"int test() { 5; } test();", L"out\\bin\\func_decl_test3.aab", L"out\\op\\func_decl_test3.txt", L"5")) {
		f++;
	} else {
		s++;
	}

	// Test for compile error (a void function returning something)
	if (!RunExpressionTest(pAAVM, L"void test() { 5; }", L"out\\bin\\func_decl_test2.aab", L"out\\op\\func_decl_test2.txt", L"Null", true)) {
		f++;
	} else {
		s++;
	}

	// Test runtime callstack (Expectes some output from code)
	if (!RunExpressionTest(pAAVM, L"void test() {  } void testb() {}", L"out\\bin\\func_decl_test1.aab", L"out\\op\\func_decl_test1.txt", L"Null", false, true)) {
		f++;
	} else {
		s++;
	}

	// Test recursion
	if (!RunFileTest(pAAVM, L"examples\\recursion.aa", L"out\\bin\\recursion.aab", L"out\\op\\recursion.txt", L"24")) {
		f++;
	} else {
		s++;
	}

}

void RunIfStatementTests(AAVM* pAAVM, int& s, int& f) {

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"if (6 < 5) { 25; } else if (4 < 5) { 27; } else { 30; }", L"out\\bin\\if3.aab", L"out\\op\\if3.txt", L"27")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"if (6 < 5) { 25; } else { 30; }", L"out\\bin\\if2.aab", L"out\\op\\if2.txt", L"30")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"if (5 < 6) { 25; }", L"out\\bin\\if1.aab", L"out\\op\\if1.txt", L"25")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"5 + 15 <= 10;", L"out\\bin\\cmp_leq2.aab", L"out\\op\\cmp_leq2.txt", L"false")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"5 <= 10;", L"out\\bin\\cmp_leq1.aab", L"out\\op\\cmp_leq1.txt", L"true")) {
		f++;
	} else {
		s++;
	}

}


void RunLoopTests(AAVM* pAAVM, int& s, int& f) {

	// Test dowhile loop
	if (!RunFileTest(pAAVM, L"examples\\dowhile-loop.aa", L"out\\bin\\dowhile-loop.aab", L"out\\op\\dowhile-loop.txt", L"10")) {
		f++;
	} else {
		s++;
	}

	// Test while loop
	if (!RunFileTest(pAAVM, L"examples\\while-loop.aa", L"out\\bin\\while-loop.aab", L"out\\op\\while-loop.txt", L"10")) {
		f++;
	} else {
		s++;
	}

	// Test for loop
	if (!RunFileTest(pAAVM, L"examples\\for-loop.aa", L"out\\bin\\for-loop.aab", L"out\\op\\for-loop.txt", L"90")) {
		f++;
	} else {
		s++;
	}

}

void RunClassTests(AAVM* pAAVM, int& s, int& f) {

	// Test class ctor + class method access + 'new' keyword
	if (!RunFileTest(pAAVM, L"testing\\class1.aa", L"out\\bin\\class1.aab", L"out\\op\\class1.txt", L"55")) {
		f++;
	} else {
		s++;
	}

	// Test class for field access
	if (!RunFileTest(pAAVM, L"testing\\class2.aa", L"out\\bin\\class2.aab", L"out\\op\\class2.txt", L"10")) {
		f++;
	} else {
		s++;
	}

	// Test class for field access (2.0)
	if (!RunFileTest(pAAVM, L"testing\\class3.aa", L"out\\bin\\class3.aab", L"out\\op\\class3.txt", L"50")) {
		f++;
	} else {
		s++;
	}

	// Test class for field access (3.0)
	if (!RunFileTest(pAAVM, L"testing\\class4.aa", L"out\\bin\\class4.aab", L"out\\op\\class4.txt", L"40")) {
		f++;
	} else {
		s++;
	}

}

void RunArrayTests(AAVM* pAAVM, int& s, int& f) {

	// Test array decleration
	if (!RunFileTest(pAAVM, L"testing\\array1.aa", L"out\\bin\\array1.aab", L"out\\op\\array1.txt", L"Null")) {
		f++;
	} else {
		s++;
	}

	// Test array get from index
	if (!RunFileTest(pAAVM, L"testing\\array2.aa", L"out\\bin\\array2.aab", L"out\\op\\array2.txt", L"0")) {
		f++;
	} else {
		s++;
	}

	// Test update array value at index
	if (!RunFileTest(pAAVM, L"testing\\array3.aa", L"out\\bin\\array3.aab", L"out\\op\\array3.txt", L"11")) {
		f++;
	} else {
		s++;
	}

}

void RunTextTests(AAVM* pAAVM, int& s, int& f) {

	/** Char tests **/

	// Test char
	if (!RunFileTest(pAAVM, L"testing\\char1.aa", L"out\\bin\\char1.aab", L"out\\op\\char1.txt", L"A")) {
		f++;
	} else {
		s++;
	}

	/** String tests **/

	// Test string
	if (!RunFileTest(pAAVM, L"testing\\string1.aa", L"out\\bin\\string1.aab", L"out\\op\\string1.txt", L"This is a string - Hello from AA")) {
		f++;
	} else {
		s++;
	}

	if (!RunFileTest(pAAVM, L"testing\\string2.aa", L"out\\bin\\string2.aab", L"out\\op\\string2.txt", L"5")) {
		f++;
	} else {
		s++;
	}

	if (!RunFileTest(pAAVM, L"testing\\string3.aa", L"out\\bin\\string3.aab", L"out\\op\\string3.txt", L"Hello World")) {
		f++;
	} else {
		s++;
	}

	if (!RunFileTest(pAAVM, L"testing\\string4.aa", L"out\\bin\\string4.aab", L"out\\op\\string4.txt", L"Hello World")) {
		f++;
	} else {
		s++;
	}

	if (!RunFileTest(pAAVM, L"testing\\string5.aa", L"out\\bin\\string5.aab", L"out\\op\\string5.txt", L"Hello World")) {
		f++;
	} else {
		s++;
	}

}

void RunNamespaceTests(AAVM* pAAVM, int& s, int& f) {

	// Test namespaces are fetched correctly (Including the 'using X from Y' directive)
	if (!RunFileTest(pAAVM, L"testing\\namespace1.aa", L"out\\bin\\namespace1.aab", L"out\\op\\namespace1.txt", L"42")) {
		f++;
	} else {
		s++;
	}

	// Nested namespace testing + simple 'using' directive
	if (!RunFileTest(pAAVM, L"testing\\namespace2.aa", L"out\\bin\\namespace2.aab", L"out\\op\\namespace2.txt", L"42")) {
		f++;
	} else {
		s++;
	}

}

void RunIOTests(AAVM* pAAVM, int& s, int& f) {

	// Test namespaces are fetched correctly (Including the 'using X from Y' directive)
	if (!RunFileTest(pAAVM, L"testing\\io1.aa", L"out\\bin\\io1.aab", L"out\\op\\io1.txt", L"0")) {
		f++;
	} else {
		s++;
	}

}

bool RunRegressionTests(AAVM* pAAVM) {

	bool cmplog, execlog, stacklog;

	cmplog = pAAVM->IsCompilerLoggingEnabled();
	execlog = pAAVM->IsExecutionTimeLoggingEnabled();
	stacklog = pAAVM->IsTopStackLoggingEnabled();

	int fails, successes;
	fails = 0;
	successes = 0;

	// Enable logging
	pAAVM->EnableCompilerLog(true);
	pAAVM->EnableExecutionTiming(true);
	pAAVM->EnableTopStackLogging(false);

	// Get start time
	clock_t start = clock();

	std::wcout << L"Running regression tests for AA:" << std::endl << std::endl;

	// Run arithmetic test
	RunArithmeticTests(pAAVM, successes, fails);

	// Run var decleration tests
	RunVariableDeclerationTests(pAAVM, successes, fails);

	// Run block tests
	RunBlockTests(pAAVM, successes, fails);

	// Run text tests
	RunTextTests(pAAVM, successes, fails);

	// Run array tests
	RunArrayTests(pAAVM, successes, fails);

	// Run function tests
	RunFunctionTests(pAAVM, successes, fails);

	// Run if-statement tests
	RunIfStatementTests(pAAVM, successes, fails);

	// Run loop tests
	RunLoopTests(pAAVM, successes, fails);

	// Run class tests
	RunClassTests(pAAVM, successes, fails);

	// Run namespace tests
	RunNamespaceTests(pAAVM, successes, fails);

	// Run IO tests
	RunIOTests(pAAVM, successes, fails);

	// Log regression test results
	std::wcout << L"Regression test report:" << std::endl;
	std::wcout << std::to_wstring(successes + fails) << L" Completed, " << std::to_wstring(successes) << L" succeeded, " << std::to_wstring(fails) << L" failed." << std::endl;
	std::wcout << L"Tests completed in " << std::to_wstring((float)(clock() - start) / CLOCKS_PER_SEC) << L"s" << std::endl << std::endl;

	// Disable logging
	pAAVM->EnableCompilerLog(cmplog);
	pAAVM->EnableExecutionTiming(execlog);
	pAAVM->EnableTopStackLogging(stacklog);

	// Return result of tests
	return fails == 0;

}
