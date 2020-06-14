#pragma once
#include "AAClassSignature.h"
#include "AAAutoCodeGenerator.h"

class AAClassCompiler {

public:

	void RedefineFunDecl(std::wstring className, AA_AST_NODE* pFuncDeclNode);

	AAClassSignature* FindClassFromCtor(std::wstring ctorname, aa::list<AAClassSignature*> classes);
	AAFuncSignature* FindBestCtor(AAClassSignature* pCC);

	/// <summary>
	/// Correct local variable references, for example:
	/// class C {
	///		int field;
	///		void test() {
	///			field = 5; // This is translated to "this.field = 5;"
	///		}
	///	}
	/// </summary>
	/// <param name="fields">List of known fields</param>
	/// <param name="pFuncSignatures">List of functions to correct</param>
	void CorrectReferences(aa::list<std::wstring> fields, aa::set<AAFuncSignature*> pFuncSignatures);

	size_t CalculateMemoryUse(AAClassSignature* cc);

	bool AddInheritanceCall(AAClassSignature* cc, AA_AST_NODE* pCtorDeclNode);
	bool AddInheritanceCallNode(AAFuncSignature* ctor, AA_AST_NODE* pCtorDeclNode);

	bool AutoTaggedClass(AAClassSignature* taggedClassSignature, AA_AST_NODE*& pOutClassFuncCtor);

	bool AutoConstructor(AAClassSignature* pClassSignature, AA_AST_NODE*& pOutClassCtor);

	static bool HasField(AAClassSignature* cc, std::wstring fieldname, int& fieldID);

private:

	std::vector<aa::compiler::generator::__name_type> GetTaggedFields(AA_AST_NODE* pClassSource);

	void CorrectFuncFieldReferences(aa::list<std::wstring> fields, aa::list<std::wstring> args, AA_AST_NODE* pNode);

	void UpdateIdentifierToThisFieldReference(AA_AST_NODE* pNode, int fieldId);

};
