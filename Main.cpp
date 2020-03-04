// Å.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AAVM.h"

int main() {

    AAVM* VM = AANewVM();

    // Set output stream
    VM->SetOutput(&std::cout);

    // Current test case
    

    // Compile and execute
    VM->Execute(VM->CompileExpressionToFile(L"int par(int x, int y) { x - y; } par(10, 5);", L"out\\func_decl_test6.aab")); // Expected output: 5
    VM->Execute(VM->CompileExpressionToFile(L"int par(int z) { var x = z; x = x + 28; x; } par(10);", L"out\\func_decl_test5.aab")); // Expected output: 38
    VM->Execute(VM->CompileExpressionToFile(L"int test() { var x = 0; x = x + 28; x; } test();", L"out\\func_decl_test4.aab")); // Expected output: 28
    VM->Execute(VM->CompileExpressionToFile(L"int test() { 5; } test();", L"out\\func_decl_test3.aab")); // Expected output: 5
    VM->Execute(VM->CompileExpressionToFile(L"void test() { 5; }", L"out\\func_decl_test2.aab")); // Expected output: none (Not run, otherwise should return error AT COMPILE TIME)
    VM->Execute(VM->CompileExpressionToFile(L"void test() {  } void testb() {}", L"out\\func_decl_test1.aab")); // Expected output: none
    VM->Execute(VM->CompileExpressionToFile(L"{ float x = 1.5f; x = x + 1.25f; x; }", L"out\\block_with_type5.aab")); // Expected output: 2.75
    VM->Execute(VM->CompileExpressionToFile(L"{ bool x = true; !x; }", L"out\\block_with_type4.aab")); // Expected output: false
    VM->Execute(VM->CompileExpressionToFile(L"{ float x = 1.5f; x; }", L"out\\block_with_type3.aab")); // Expected output: 1.5
    VM->Execute(VM->CompileExpressionToFile(L"{ bool x = true; x; }", L"out\\block_with_type2.aab")); // Expected output: true
    VM->Execute(VM->CompileExpressionToFile(L"{ int x = 5; x = x + 5; x; }", L"out\\block_with_type1.aab")); // Expected output: 10
    VM->Execute(VM->CompileExpressionToFile(L"{ var x = 5; x = x + 5; x; }", L"out\\block_with_var3.aab")); // Expected output: 10
    VM->Execute(VM->CompileExpressionToFile(L"{ var x = (5+5)*5-2; var y = 20; x - y; }", L"out\\block_with_var2.aab")); // Expected output: 28
    VM->Execute(VM->CompileExpressionToFile(L"{ var x = (5+5)*5-2; x; }", L"out\\block_with_var1.aab")); // Expected output: 48
    VM->Execute(VM->CompileExpressionToFile(L"var x = (5+5)*5-2;", L"out\\variable_var_rhsop2.aab")); // Expected output: none
    VM->Execute(VM->CompileExpressionToFile(L"var x = 5+5;", L"out\\variable_var_rhsop1.aab")); // Expected output: none
    VM->Execute(VM->CompileExpressionToFile(L"var x = 5;", L"out\\variable_var.aab")); // Expected output: none
    VM->Execute(VM->CompileExpressionToFile(L"5+5*5;", L"out\\math_arithmetic_binding.aab")); // Expected output = 30
    VM->Execute(VM->CompileExpressionToFile(L"5--5;", L"out\\math_negate4.aab")); // Expected output = 10
    VM->Execute(VM->CompileExpressionToFile(L"5+-5;", L"out\\math_negate3.aab")); // Expected output = 0
    VM->Execute(VM->CompileExpressionToFile(L"5+(-5);", L"out\\math_negate2.aab")); // Expected output = 0
    VM->Execute(VM->CompileExpressionToFile(L"-5;", L"out\\math_negate1.aab")); // Exepcted output = -5
    VM->Execute(VM->CompileExpressionToFile(L"((((5+6))));", L"out\\math_paranthesis3.aab")); // Expected output = 11
    VM->Execute(VM->CompileExpressionToFile(L"((15+5)*42)-(5/2);", L"out\\math_paranthesis2.aab")); // Expected output = 838
    VM->Execute(VM->CompileExpressionToFile(L"(15+5)*42-(5/2);", L"out\\math_paranthesis.aab")); // Expected output = 838
    VM->Execute(VM->CompileExpressionToFile(L"15+5+7;", L"out\\math_add.aab")); // Expected output = 27
    VM->Execute(VM->CompileExpressionToFile(L"15+5-7;", L"out\\math_sub.aab")); // Expected output = 13
    VM->Execute(VM->CompileExpressionToFile(L"15*5;", L"out\\math_mul.aab")); // Expected output = 75
    VM->Execute(VM->CompileExpressionToFile(L"15/5;", L"out\\math_div.aab")); // Expected output = 3
    VM->Execute(VM->CompileExpressionToFile(L"15%5;", L"out\\math_mod.aab")); // Expected output = 0

    // Release the virtual machine
    VM->Release();

    // Allow user to see output
    system("pause");

    return 1;

}
