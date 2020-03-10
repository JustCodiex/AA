#pragma once
#include "AA_AST.h"
#include "AAFuncSignature.h"
#include "list.h"

struct CompiledClassMethod {
	AAFuncSignature sig;
	AA_AST_NODE* source;
	int procID;
	bool isCtor;
};

struct CompiledClass {
	std::wstring name;
	aa::list<CompiledClassMethod> methods;
};

class AAClassCompiler {

public:

	void RedefineFunDecl(std::wstring className, AA_AST_NODE* pFuncDeclNode);

private:

};
