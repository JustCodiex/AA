// Å.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AAVM.h"
#include "AAB2F.h"

int main() {

    AAVM* VM = AANewVM();
    AAC* compiler = VM->GetCompiler();
    AAP* parser = VM->GetParser();

    AA_AST* tree = parser->Parse(L"15+5+7;");
    AAC_Out bytecode = compiler->CompileFromAST(tree);

    aa::dump_bytecode(L"out\\math1.aab", bytecode);

    VM->Execute(bytecode.bytes, bytecode.length);

    VM->Release();

    return 1;

}
