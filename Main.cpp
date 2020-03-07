// Å.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AAVM.h"

int main() {

    AAVM* VM = AAVM::CreateNewVM(false, false);

    // Set output stream
    VM->SetOutput(&std::cout);

    // Current test case
    //VM->CompileAndRunFile(L"examples\\for-loop.aa", L"out\\bin\\for-loop.aab", L"out\\op\\for-loop.txt"); // Expected output: 90

    // Compile and execute
    VM->CompileAndRunFile(L"examples\\recursion.aa", L"out\\bin\\recursion.aab", L"out\\op\\recursion.txt"); // Expected output: 24
    VM->CompileAndRunExpression(L"if (6 < 5) { 25; } else if (4 < 5) { 27; } else { 30; }", L"out\\bin\\if3.aab", L"out\\op\\if3.txt"); // Expected output: 27
    VM->CompileAndRunExpression(L"if (6 < 5) { 25; } else { 30; }", L"out\\bin\\if2.aab", L"out\\op\\if2.txt"); // Expected output: 30
    VM->CompileAndRunExpression(L"if (5 < 6) { 25; }", L"out\\bin\\if1.aab", L"out\\op\\if1.txt"); // Expected output: 25
    VM->CompileAndRunExpression(L"5 + 15 <= 10;", L"out\\bin\\cmp_leq2.aab", L"out\\op\\cmp_leq2.txt"); // Expected output: false
    VM->CompileAndRunExpression(L"5 <= 10;", L"out\\bin\\cmp_leq1.aab", L"out\\op\\cmp_leq1.txt"); // Expected output: true
    VM->CompileAndRunFile(L"testing\\func.aa", L"out\\bin\\func.aab", L"out\\op\\func.txt"); // Expected output: 51
    VM->CompileAndRunExpression(L"int par(int x, int y) { x - y; } par(15+5,par(75,50));", L"out\\bin\\func_decl_test8.aab", L"out\\op\\func_decl_test8.txt"); // Expected output: -5
    VM->CompileAndRunExpression(L"int par(int x, int y) { x - y; } par(15+5,5);", L"out\\bin\\func_decl_test7.aab", L"out\\op\\func_decl_test7.txt"); // Expected output: 15
    VM->CompileAndRunExpression(L"int par(int x, int y) { x - y; } par(10, 5);", L"out\\bin\\func_decl_test6.aab", L"out\\op\\func_decl_test6.txt"); // Expected output: 5
    VM->CompileAndRunExpression(L"int par(int z) { var x = z; x = x + 28; x; } par(10);", L"out\\bin\\func_decl_test5.aab", L"out\\op\\func_decl_test5.txt"); // Expected output: 38
    VM->CompileAndRunExpression(L"int test() { var x = 0; x = x + 28; x; } test();", L"out\\bin\\func_decl_test4.aab", L"out\\op\\func_decl_test4.txt"); // Expected output: 28
    VM->CompileAndRunExpression(L"int test() { 5; } test();", L"out\\bin\\func_decl_test3.aab", L"out\\op\\func_decl_test3.txt"); // Expected output: 5
    VM->CompileAndRunExpression(L"void test() { 5; }", L"out\\bin\\func_decl_test2.aab", L"out\\op\\func_decl_test2.txt"); // Expected output: none (Not run, otherwise should return error AT COMPILE TIME)
    VM->CompileAndRunExpression(L"void test() {  } void testb() {}", L"out\\bin\\func_decl_test1.aab", L"out\\op\\func_decl_test1.txt"); // Expected output: none
    VM->CompileAndRunExpression(L"{ float x = 1.5f; x = x + 1.25f; x; }", L"out\\bin\\block_with_type5.aab", L"out\\op\\block_with_type5.txt"); // Expected output: 2.75
    VM->CompileAndRunExpression(L"{ bool x = true; !x; }", L"out\\bin\\block_with_type4.aab", L"out\\op\\block_with_type4.txt"); // Expected output: false
    VM->CompileAndRunExpression(L"{ float x = 1.5f; x; }", L"out\\bin\\block_with_type3.aab", L"out\\op\\block_with_type3.txt"); // Expected output: 1.5
    VM->CompileAndRunExpression(L"{ bool x = true; x; }", L"out\\bin\\block_with_type2.aab", L"out\\op\\block_with_type2.txt"); // Expected output: true
    VM->CompileAndRunExpression(L"{ int x = 5; x = x + 5; x; }", L"out\\bin\\block_with_type1.aab", L"out\\op\\block_with_type1.txt"); // Expected output: 10
    VM->CompileAndRunExpression(L"{ var x = 5; x = x + 5; x; }", L"out\\bin\\block_with_var3.aab", L"out\\op\\block_with_var3.txt"); // Expected output: 10
    VM->CompileAndRunExpression(L"{ var x = (5+5)*5-2; var y = 20; x - y; }", L"out\\bin\\block_with_var2.aab", L"out\\op\\block_with_var2.txt"); // Expected output: 28
    VM->CompileAndRunExpression(L"{ var x = (5+5)*5-2; x; }", L"out\\bin\\block_with_var1.aab", L"out\\op\\block_with_var1.txt"); // Expected output: 48
    VM->CompileAndRunExpression(L"var x = (5+5)*5-2;", L"out\\bin\\variable_var_rhsop2.aab", L"out\\op\\variable_var_rhsop2.txt"); // Expected output: none
    VM->CompileAndRunExpression(L"var x = 5+5;", L"out\\bin\\variable_var_rhsop1.aab", L"out\\op\\variable_var_rhsop1.txt"); // Expected output: none
    VM->CompileAndRunExpression(L"var x = 5;", L"out\\bin\\variable_var.aab", L"out\\op\\variable_var.txt"); // Expected output: none
    VM->CompileAndRunExpression(L"5+5*5;", L"out\\bin\\math_arithmetic_binding.aab", L"out\\op\\math_arithmetic_binding.txt"); // Expected output = 30
    VM->CompileAndRunExpression(L"5--5;", L"out\\bin\\math_negate4.aab", L"out\\op\\math_negate4.txt"); // Expected output = 10
    VM->CompileAndRunExpression(L"5+-5;", L"out\\bin\\math_negate3.aab", L"out\\op\\math_negate3.txt"); // Expected output = 0
    VM->CompileAndRunExpression(L"5+(-5);", L"out\\bin\\math_negate2.aab", L"out\\op\\math_negate2.txt"); // Expected output = 0
    VM->CompileAndRunExpression(L"-5;", L"out\\bin\\math_negate1.aab", L"out\\op\\math_negate1.txt"); // Exepcted output = -5
    VM->CompileAndRunExpression(L"((((5+6))));", L"out\\bin\\math_paranthesis3.aab", L"out\\op\\math_paranthesis3.txt"); // Expected output = 11
    VM->CompileAndRunExpression(L"((15+5)*42)-(5/2);", L"out\\bin\\math_paranthesis2.aab", L"out\\op\\math_paranthesis2.txt"); // Expected output = 838
    VM->CompileAndRunExpression(L"(15+5)*42-(5/2);", L"out\\bin\\math_paranthesis.aab", L"out\\op\\math_paranthesis.txt"); // Expected output = 838
    VM->CompileAndRunExpression(L"15+5+7;", L"out\\bin\\math_add.aab", L"out\\op\\math_add.txt"); // Expected output = 27
    VM->CompileAndRunExpression(L"15+5-7;", L"out\\bin\\math_sub.aab", L"out\\op\\math_sub.txt"); // Expected output = 13
    VM->CompileAndRunExpression(L"15*5;", L"out\\bin\\math_mul.aab", L"out\\op\\math_mul.txt"); // Expected output = 75
    VM->CompileAndRunExpression(L"15/5;", L"out\\bin\\math_div.aab", L"out\\op\\math_div.txt"); // Expected output = 3
    VM->CompileAndRunExpression(L"15%5;", L"out\\bin\\math_mod.aab", L"out\\op\\math_mod.txt"); // Expected output = 0

    // Release the virtual machine
    VM->Release();

    // Allow user to see output
    system("pause");

    return 1;

}
