#pragma once
#include "AA_AST.h"
#include "AAFuncSignature.h"
#include "list.h"

struct CompiledClassMethod {
	AAFuncSignature sig;
	AA_AST_NODE* source;
	int procID;
	bool isCtor;
	bool isPublic;
	CompiledClassMethod() {
		sig = AAFuncSignature();
		source = 0;
		procID = -1;
		isCtor = false;
		isPublic = true;
	}
};

struct CompiledClassOperator {
	std::wstring op;
	CompiledClassMethod method;
	CompiledClassOperator() {
		op = L"";
		method = CompiledClassMethod();
	}
	CompiledClassOperator(std::wstring op, CompiledClassMethod method) {
		this->op = op;
		this->method = method;
	}
};

struct CompiledClassField {
	std::wstring name;
	std::wstring type;
	int fieldID;
	bool isPublic;
	CompiledClassField() {
		name = L"";
		type = L"";
		fieldID = -1;
		isPublic = true;
	}
};

struct CompiledClass {
	std::wstring name;
	aa::list<CompiledClassMethod> methods;
	aa::list<CompiledClassOperator> operators;
	aa::list<CompiledClassField> fields;
	size_t classByteSz;
	CompiledClass() {
		name = L"";
		classByteSz = 0;
	}
};

class AAClassCompiler {

public:

	void RedefineFunDecl(std::wstring className, AA_AST_NODE* pFuncDeclNode);

	CompiledClass FindClassFromCtor(std::wstring ctorname, aa::list<CompiledClass> classes);
	CompiledClassMethod FindBestCtor(CompiledClass* pCC);

	void CorrectReferences(CompiledClass* cc, std::vector<AA_AST_NODE*> pFuncBodies);

	size_t CalculateMemoryUse(CompiledClass cc);

	static bool HasField(CompiledClass cc, std::wstring fieldname, int& fieldID);

private:

	void CorrectFuncFieldReferences(AA_AST_NODE* pNode, CompiledClass* cc);

	void UpdateIdentifierToThisFieldReference(AA_AST_NODE* pNode, int fieldId);

};
