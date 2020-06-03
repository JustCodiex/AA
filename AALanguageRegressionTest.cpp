#include "AALanguageRegressionTest.h"
#include <iostream>

bool VerifyTestResult(AAVM* pAAVM, AAStackValue result, AAStackValue expected, bool compile, bool runtime) {
	
	// Did any error occur?
	bool anyCompileError = pAAVM->HasCompileError();
	bool anyRuntimeError = pAAVM->HasRuntimeError();

	if (anyCompileError || anyRuntimeError) {

		// Check compile error
		if (anyCompileError && compile) {
			std::wcout << L"Test Succeeded (Succesfully detected compile error)" << L"\n\n";
			return true;
		} else if (anyCompileError && !compile) {
			std::wcout << L"Test Failed, unexpected compile error." << L"\n\n";
			return false;
		}

		// Check compile error
		if (anyRuntimeError && runtime) {
			std::wcout << L"Test Succeeded (Succesfully detected runtime error)" << L"\n\n";
			return true;
		} else if (anyRuntimeError && !runtime) {
			std::wcout << L"Test Failed, unexpected runtime error." << L"\n\n";
			return false;
		}

		std::wcout << "Test failed unexpectedly" << L"\n\n";
		return false;

	} else {

		// Did we get the same output
		bool success = false;

		// Is it a string we're testing?
		if (expected.get_type() == AAPrimitiveType::string) {
			
			// Make sure we're working with something valid
			if (result.as_val().is_valid()) {

				std::wstring wsr = result.to_cpp<std::wstring>();
				std::wstring wse = expected.to_cpp<std::wstring>();

				success = result.ToString().compare(expected.ToString()) == 0;

			} else {

				std::wcout << L"Test Failed, received 'null' but expected '" << expected.ToString() << "'" << L"\n\n";
				return false;

			}

		} else { // Numeric value we're comparing

			// Normalise
			result = result.change_type(expected.get_type());

			// Did we have any success?
			success = result.is_same_value(expected);

		}

		// Did we succeed?
		if (success) {
			std::wcout << L"Test Succeeded ('" << result.ToString() << L"' == '" << expected.ToString() << L"')" << L"\n\n";
		} else {
			std::wcout << L"Test Failed, received '" << result.ToString() << L"' but expected '" << expected.ToString() << L"')" << L"\n\n";
		}
		
		// Return result
		return success;

	}

}

bool RunFileTest(AAVM* pAAVM, std::wstring fileinput, std::wstring fileoutput, AAStackValue expectedoutput, bool compile, bool runtime) {

	std::wcout << L"Testing file: " << fileinput << L"\n";
	return VerifyTestResult(pAAVM, 
		pAAVM->CompileAndRunFile(fileinput, 
			L"out\\bin\\" + fileoutput + L".aab", 
			L"out\\op\\" + fileoutput + L".txt", 
			L"out\\unparsed\\" + fileoutput + L".aa"
		), 
		expectedoutput, compile, runtime
	);

}

bool RunExpressionTest(AAVM* pAAVM, std::wstring expression, std::wstring fileoutput, AAStackValue expectedoutput, bool compile, bool runtime) {

	std::wstring fileOutputOpCodes = L"out\\op\\" + fileoutput + L".txt";
	std::wstring fileOutputUnparsed = L"out\\unparsed\\" + fileoutput + L".aa";
	std::wcout << L"Testing expression: '" << expression << L"';" << L" OpCodeFile: \"" << fileOutputOpCodes << L"\"; UnparseFile: '" << fileOutputUnparsed << L"'" << L"\n";
	return VerifyTestResult(pAAVM, pAAVM->CompileAndRunExpression(expression, L"out\\bin\\" + fileoutput + L".aab", fileOutputOpCodes, fileOutputUnparsed), expectedoutput, compile, runtime);

}

void RunArithmeticTests(AAVM* pAAVM, int& s, int& f) {

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"5+5*5;", L"math_arithmetic_binding", 30)) {
		f++;
	} else {
		s++;
	}

	// Test '--' as a binary operation is not legal
	if (!RunExpressionTest(pAAVM, L"5--5;", L"math_negate4", AAStackValue::None, true)) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"5+-5;", L"math_negate3", 0)) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"5+(-5);", L"math_negate2", 0)) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"-5;", L"math_negate1", -5)) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"((((5+6))));", L"math_paranthesis3", 11)) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"((15+5)*42)-(5/2);", L"math_paranthesis2", 838)) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"(15+5)*42-(5/2);", L"math_paranthesis", 838)) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"15+5+7;", L"math_add", 27)) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"15+5-7;", L"math_sub", 13)) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"15*5;", L"math_mul", 75)) {
		f++;
	} else {
		s++;
	}

	// Test division operator
	if (!RunExpressionTest(pAAVM, L"15/5;", L"math_div", 3)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"15%5;", L"math_mod", 0)) {
		f++;
	} else {
		s++;
	}

	// Test compund assignment operators
	if (!RunFileTest(pAAVM, L"examples\\arithmetic\\binary_assignment1.aa", L"binary_assignment1", 0)) {
		f++;
	} else {
		s++;
	}

}

void RunVariableDeclerationTests(AAVM* pAAVM, int& s, int& f) {

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"var x = (5+5)*5-2; x;", L"variable_var_rhsop2", 48)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"var x = 5+5; x;", L"variable_var_rhsop1", 10)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"var x = 5; x;", L"variable_var", 5)) {
		f++;
	} else {
		s++;
	}

}

void RunBlockTests(AAVM* pAAVM, int& s, int& f) {

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ float x = 1.5f; x = x + 1.25f; x; }", L"block_with_type5", 2.750000f)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ bool x = true; !x; }", L"block_with_type4", false)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ float x = 1.5f; x; }", L"block_with_type3", 1.500000f)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ bool x = true; x; }", L"block_with_type2", true)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ int x = 5; x = x + 5; x; }", L"block_with_type1", 10)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ var x = 5; x = x + 5; x; }", L"block_with_var3", 10)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ var x = (5+5)*5-2; var y = 20; x - y; }", L"block_with_var2", 28)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"{ var x = (5+5)*5-2; x; }", L"block_with_var1", 48)) {
		f++;
	} else {
		s++;
	}

}

void RunScopeTests(AAVM* pAAVM, int& s, int& f) {

	// Test scope with single variable
	if (!RunFileTest(pAAVM, L"examples\\scoping\\scope1.aa", L"scope1", 0)) {
		f++;
	} else {
		s++;
	}

}

void RunFunctionTests(AAVM* pAAVM, int& s, int& f) {

	// Test function call with arguments consisting of function calls
	if (!RunFileTest(pAAVM, L"examples\\misc\\func.aa", L"func", 51)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"int par(int x, int y) { x - y; } par(15+5,par(75,50));", L"func_decl_test8", -5)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"int par(int x, int y) { x - y; } par(15+5,5);", L"func_decl_test7", 15)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"int par(int x, int y) { x - y; } par(10, 5);", L"func_decl_test6", 5)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"int par(int z) { var x = z; x = x + 28; x; } par(10);", L"func_decl_test5", 38)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"int test() { var x = 0; x = x + 28; x; } test();", L"func_decl_test4", 28)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"int test() { 5; } test();", L"func_decl_test3", 5)) {
		f++;
	} else {
		s++;
	}

	// Test for compile error (a void function returning something)
	if (!RunExpressionTest(pAAVM, L"void test() { 5; }", L"func_decl_test2", AAStackValue::None, true)) {
		f++;
	} else {
		s++;
	}

	// Test runtime callstack (Expectes some output from code)
	if (!RunExpressionTest(pAAVM, L"void test() {  } void testb() {}", L"func_decl_test1", AAStackValue::None, false, true)) {
		f++;
	} else {
		s++;
	}

	// Test recursion
	if (!RunFileTest(pAAVM, L"examples\\misc\\recursion.aa", L"recursion", 24)) {
		f++;
	} else {
		s++;
	}

}

void RunIfStatementTests(AAVM* pAAVM, int& s, int& f) {

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"if (6 < 5) { 25; } else if (4 < 5) { 27; } else { 30; }", L"if3", 27)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"if (6 < 5) { 25; } else { 30; }", L"if2", 30)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"if (5 < 6) { 25; }", L"if1", 25)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"5 + 15 <= 10;", L"cmp_leq2", false)) {
		f++;
	} else {
		s++;
	}

	// Test modulo operator
	if (!RunExpressionTest(pAAVM, L"5 <= 10;", L"cmp_leq1", true)) {
		f++;
	} else {
		s++;
	}

}

void RunLogicTests(AAVM* pAAVM, int& s, int& f) {

	// Test logical and
	if (!RunFileTest(pAAVM, L"examples\\logic\\and1.aa", L"and1", 37)) {
		f++;
	} else {
		s++;
	}

	// Test logical or
	if (!RunFileTest(pAAVM, L"examples\\logic\\or1.aa", L"or1", 66)) {
		f++;
	} else {
		s++;
	}

}

void RunLoopTests(AAVM* pAAVM, int& s, int& f) {

	// Test dowhile loop
	if (!RunFileTest(pAAVM, L"examples\\misc\\dowhile-loop.aa", L"dowhile-loop", 10)) {
		f++;
	} else {
		s++;
	}

	// Test while loop
	if (!RunFileTest(pAAVM, L"examples\\misc\\while-loop.aa", L"while-loop", 10)) {
		f++;
	} else {
		s++;
	}

	// Test for loop
	if (!RunFileTest(pAAVM, L"examples\\misc\\for-loop.aa", L"for-loop", 90)) {
		f++;
	} else {
		s++;
	}

}

void RunClassTests(AAVM* pAAVM, int& s, int& f) {

	// Test class ctor + class method access + 'new' keyword
	if (!RunFileTest(pAAVM, L"examples\\classes\\class1.aa", L"class1", 55)) {
		f++;
	} else {
		s++;
	}

	// Test class for field access
	if (!RunFileTest(pAAVM, L"examples\\classes\\class2.aa", L"class2", 10)) {
		f++;
	} else {
		s++;
	}

	// Test class for field access (2.0)
	if (!RunFileTest(pAAVM, L"examples\\classes\\class3.aa", L"class3", 50)) {
		f++;
	} else {
		s++;
	}

	// Test class for field access (3.0)
	if (!RunFileTest(pAAVM, L"examples\\classes\\class4.aa", L"class4", 40)) {
		f++;
	} else {
		s++;
	}

	// Test simple class inheritance
	if (!RunFileTest(pAAVM, L"examples\\classes\\class5.aa", L"class5", 0)) {
		f++;
	} else {
		s++;
	}

	// Test simple class inheritance
	if (!RunFileTest(pAAVM, L"examples\\classes\\class6.aa", L"class6", 0)) {
		f++;
	} else {
		s++;
	}

	// Test pattern matching
	if (!RunFileTest(pAAVM, L"examples\\classes\\class7.aa", L"class7", 10)) {
		f++;
	} else {
		s++;
	}

}

void RunArrayTests(AAVM* pAAVM, int& s, int& f) {

	// Test array decleration
	if (!RunFileTest(pAAVM, L"examples\\array\\array1.aa", L"array1", AAStackValue::None)) {
		f++;
	} else {
		s++;
	}

	// Test array get from index
	if (!RunFileTest(pAAVM, L"examples\\array\\array2.aa", L"array2", 0)) {
		f++;
	} else {
		s++;
	}

	// Test update array value at index
	if (!RunFileTest(pAAVM, L"examples\\array\\array3.aa", L"array3", 11)) {
		f++;
	} else {
		s++;
	}

}

void RunTextTests(AAVM* pAAVM, int& s, int& f) {

	/** Char tests **/

	// Test char
	if (!RunFileTest(pAAVM, L"examples\\string\\char1.aa", L"char1", (wchar_t)'A')) {
		f++;
	} else {
		s++;
	}

	/** String tests **/

	// Test string
	if (!RunFileTest(pAAVM, L"examples\\string\\string1.aa", L"string1", std::wstring(L"This is a string - Hello from AA"))) {
		f++;
	} else {
		s++;
	}

	if (!RunFileTest(pAAVM, L"examples\\string\\string2.aa", L"string2", 5)) {
		f++;
	} else {
		s++;
	}

	if (!RunFileTest(pAAVM, L"examples\\string\\string3.aa", L"string3", std::wstring(L"Hello World"))) {
		f++;
	} else {
		s++;
	}

	if (!RunFileTest(pAAVM, L"examples\\string\\string4.aa", L"string4", std::wstring(L"Hello World"))) {
		f++;
	} else {
		s++;
	}

	if (!RunFileTest(pAAVM, L"examples\\string\\string5.aa", L"string5", std::wstring(L"Hello World"))) {
		f++;
	} else {
		s++;
	}

}

void RunNamespaceTests(AAVM* pAAVM, int& s, int& f) {

	// Test namespaces are fetched correctly (Including the 'using X from Y' directive)
	if (!RunFileTest(pAAVM, L"examples\\namespaces\\namespace1.aa", L"namespace1", 42)) {
		f++;
	} else {
		s++;
	}

	// Nested namespace testing + simple 'using' directive
	if (!RunFileTest(pAAVM, L"examples\\namespaces\\namespace2.aa", L"namespace2", 42)) {
		f++;
	} else {
		s++;
	}

}

void RunIOTests(AAVM* pAAVM, int& s, int& f) {

	// IO functionality (and the using std::io) system
	if (!RunFileTest(pAAVM, L"examples\\std\\io1.aa", L"io1", 0)) {
		f++;
	} else {
		s++;
	}

}

void RunEnumTests(AAVM* pAAVM, int& s, int& f) {

	// Test if enums with functions can work
	if (!RunFileTest(pAAVM, L"examples\\enum\\enum1.aa", L"enum1", 0)) {
		f++;
	} else {
		s++;
	}

}

void RunTupleTests(AAVM* pAAVM, int& s, int& f) {

	// Test a simple tuple creation
	if (!RunFileTest(pAAVM, L"examples\\tuple\\tuple1.aa", L"tuple1", 6)) {
		f++;
	} else {
		s++;
	}

	// Test a tuple variant of generating fibonacci numbers
	if (!RunFileTest(pAAVM, L"examples\\tuple\\tuple2.aa", L"tuple2", 385)) {
		f++;
	} else {
		s++;
	}

	// Test tuple pattern matching
	if (!RunFileTest(pAAVM, L"examples\\tuple\\tuple3.aa", L"tuple3", -57)) {
		f++;
	} else {
		s++;
	}

	// Test tuple pattern matching (with variables)
	if (!RunFileTest(pAAVM, L"examples\\tuple\\tuple4.aa", L"tuple4", 66)) {
		f++;
	} else {
		s++;
	}

	// Test tuple pattern matching (with variables and more cases
	if (!RunFileTest(pAAVM, L"examples\\tuple\\tuple5.aa", L"tuple5", 144)) {
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

	// Run scope tests
	RunScopeTests(pAAVM, successes, fails);

	// Run text tests
	RunTextTests(pAAVM, successes, fails);

	// Run function tests
	RunFunctionTests(pAAVM, successes, fails);

	// Run logic tests
	RunLogicTests(pAAVM, successes, fails);

	// Run if-statement tests
	RunIfStatementTests(pAAVM, successes, fails);

	// Run loop tests
	RunLoopTests(pAAVM, successes, fails);

	// Run array tests
	RunArrayTests(pAAVM, successes, fails);

	// Run tuple tests
	RunTupleTests(pAAVM, successes, fails);

	// Run class tests
	RunClassTests(pAAVM, successes, fails);

	// Run namespace tests
	RunNamespaceTests(pAAVM, successes, fails);

	// Run IO tests
	//RunIOTests(pAAVM, successes, fails);

	// Run enum tests
	RunEnumTests(pAAVM, successes, fails);

	// Log regression test results
	std::wcout << L"Regression test report:" << L"\n";
	std::wcout << std::to_wstring(successes + fails) << L" Completed, " << std::to_wstring(successes) << L" succeeded, " << std::to_wstring(fails) << L" failed." << L"\n";
	std::wcout << L"Tests completed in " << std::to_wstring((float)(clock() - start) / CLOCKS_PER_SEC) << L"s" << L"\n\n";

	// Disable logging
	pAAVM->EnableCompilerLog(cmplog);
	pAAVM->EnableExecutionTiming(execlog);
	pAAVM->EnableTopStackLogging(stacklog);

	// Return result of tests
	return fails == 0;

}
