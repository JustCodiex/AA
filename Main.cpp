// Å.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AAVM.h"

int main() {

    AAVM* VM = AANewVM();
    AAC* compiler = VM->GetCompiler();
    AAP* parser = VM->GetParser();

    AA_AST* tree = parser->Parse(L"15+5+7;");
    AAC_Out bytecode = compiler->CompileFromAST(tree);

    VM->Release();

    return 1;

}
