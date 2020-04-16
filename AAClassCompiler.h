#pragma once
#include "AAClassSignature.h"
#include "list.h"

class AAClassCompiler {

public:

	void RedefineFunDecl(std::wstring className, AA_AST_NODE* pFuncDeclNode);

	AAClassSignature* FindClassFromCtor(std::wstring ctorname, aa::list<AAClassSignature*> classes);
	AAFuncSignature* FindBestCtor(AAClassSignature* pCC);

	void CorrectReferences(AAClassSignature* cc, std::vector<AA_AST_NODE*> pFuncBodies);

	size_t CalculateMemoryUse(AAClassSignature* cc);

	bool AddInheritanceCall(AAClassSignature* cc, AA_AST_NODE* pCtorDeclNode);
	bool AddInheritanceCallNode(AAFuncSignature* ctor, AA_AST_NODE* pCtorDeclNode);

	bool AutoTaggedClass(AAClassSignature* taggedClassSignature);

	static bool HasField(AAClassSignature* cc, std::wstring fieldname, int& fieldID);

private:

	void CorrectFuncFieldReferences(AA_AST_NODE* pNode, AAClassSignature* cc);

	void UpdateIdentifierToThisFieldReference(AA_AST_NODE* pNode, int fieldId);

};
