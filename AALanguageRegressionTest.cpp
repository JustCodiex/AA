#include "AALanguageRegressionTest.h"
#include <iostream>

bool RunFileTest(AAVM* pAAVM, std::wstring fileinput, std::wstring fileoutputBinary, std::wstring fileoutputOpCodes, std::wstring expectedoutput) {

	std::wcout << L"Testing file: " << fileinput << std::endl;

	AAVal v = pAAVM->CompileAndRunFile(fileinput, fileoutputBinary, fileoutputOpCodes);
	bool success = v.ToString() == expectedoutput;

	if (success) {
		std::wcout << L"Test Succeeded ('" << v.ToString() << L"' == '" << expectedoutput << L"')" << std::endl;
	} else {
		std::wcout << L"Test Failed, received '" << v.ToString() << L"' but expected '" << expectedoutput << L"')" << std::endl;
	}

	std::wcout << std::endl;

	return success;

}

bool RunExpressionTest(AAVM* pAAVM, std::wstring expression, std::wstring fileoutputBinary, std::wstring fileoutputOpCodes, std::wstring expectedoutput) {

	std::wcout << L"Testing expression: '" << expression << L"'" << std::endl;

	AAVal v = pAAVM->CompileAndRunExpression(expression, fileoutputBinary, fileoutputOpCodes);
	bool success = v.ToString() == expectedoutput;

	if (success) {
		std::wcout << L"Test Succeeded ('" << v.ToString() << L"' == '" << expectedoutput << L"')" << std::endl;
	} else {
		std::wcout << L"Test Failed, received '" << v.ToString() << L"' but expected '" << expectedoutput << L"')" << std::endl;
	}

	std::wcout << std::endl;

	return success;

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
	if (!RunExpressionTest(pAAVM, L"var x = (5+5)*5-2;", L"out\\bin\\variable_var_rhsop2.aab", L"out\\op\\variable_var_rhsop2.txt", L"Nil")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"var x = 5+5;", L"out\\bin\\variable_var_rhsop1.aab", L"out\\op\\variable_var_rhsop1.txt", L"Nil")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"var x = 5;", L"out\\bin\\variable_var.aab", L"out\\op\\variable_var.txt", L"Nil")) {
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

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"void test() { 5; }", L"out\\bin\\func_decl_test2.aab", L"out\\op\\func_decl_test2.txt", L"Nil")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"void test() {  } void testb() {}", L"out\\bin\\func_decl_test1.aab", L"out\\op\\func_decl_test1.txt", L"Nil")) {
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

	// Test modulo operator
	if (!RunFileTest(pAAVM, L"examples\\dowhile-loop.aa", L"out\\bin\\dowhile-loop.aab", L"out\\op\\dowhile-loop.txt", L"10")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunFileTest(pAAVM, L"examples\\while-loop.aa", L"out\\bin\\while-loop.aab", L"out\\op\\while-loop.txt", L"10")) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunFileTest(pAAVM, L"examples\\for-loop.aa", L"out\\bin\\for-loop.aab", L"out\\op\\for-loop.txt", L"90")) {
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
	pAAVM->EnableCompilerLog(false);
	pAAVM->EnableExecutionTiming(true);
	pAAVM->EnableTopStackLogging(false);

	std::wcout << L"Running regression tests for AA:" << std::endl << std::endl;

	// Run arithmetic test
	RunArithmeticTests(pAAVM, successes, fails);

	// Run var decleration tests
	RunVariableDeclerationTests(pAAVM, successes, fails);

	// Run block tests
	RunBlockTests(pAAVM, successes, fails);

	// Run function tests
	RunFunctionTests(pAAVM, successes, fails);

	// Run if-statement tests
	RunIfStatementTests(pAAVM, successes, fails);

	// Run if-statement tests
	RunLoopTests(pAAVM, successes, fails);

	std::wcout << L"Regression test report:" << std::endl;
	std::wcout << std::to_wstring(successes + fails) << L" Completed, " << std::to_wstring(successes) << L" succeeded, " << std::to_wstring(fails) << L" failed." << std::endl << std::endl;

	// Disable logging
	pAAVM->EnableCompilerLog(cmplog);
	pAAVM->EnableExecutionTiming(execlog);
	pAAVM->EnableTopStackLogging(stacklog);

	return true;

}
