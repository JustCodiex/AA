#pragma once
#include "AAClassCompiler.h"
#include "AAC_Structs.h"
#include "AATypeChecker.h"

// Forward declare compiler
class AAC;

/// <summary>
/// A static environment for static data
/// </summary>
struct AAStaticEnvironment {

	AACNamespace* globalNamespace;
	aa::set<AAFuncSignature*> availableFunctions;
	aa::set<AAClassSignature*> availableClasses;
	aa::set<AACType*> availableTypes;
	aa::set<AACEnumSignature*> availableEnums;

	AAStaticEnvironment() {
		globalNamespace = 0;
	}

	AAStaticEnvironment Union(AACNamespace*& space) const {
		AAStaticEnvironment senv = AAStaticEnvironment(*this);
		senv.availableClasses.UnionWith(space->classes);
		senv.availableFunctions.UnionWith(space->functions);
		senv.availableTypes.UnionWith(space->types);
		senv.availableEnums.UnionWith(space->enums);
		return senv;
	}

};

/// <summary>
/// Class for performaing static analysis on code
/// </summary>
class AAStaticAnalysis {

public:

	AAStaticAnalysis(AAC* pCompiler);

	/// <summary>
	/// Run a static analysis on a list of ASTs
	/// </summary>
	/// <param name="trees">Abstract Syntax Trees to perform static analysis on</param>
	/// <returns></returns>
	AAC_CompileErrorMessage RunStaticAnalysis(std::vector<AA_AST*>& trees);

	/// <summary>
	/// Run a typecheck on a specific <see cref="AA_AST*"/> tree.
	/// </summary>
	/// <param name="pTree">The AST to run typecheck functions on.</param>
	/// <param name="senv">The static environment to use when typechecking (Pass by reference, important! will break using statements if not).</param>
	/// <param name="typeError">Any possible error that the typechecker may have found. Only valid if false is returned.</param>
	/// <returns>True if the typechecker found no type errors. False if the typechecker found a type-error.</returns>
	bool RunTypecheckAnalysis(AA_AST* pTree, AAStaticEnvironment& senv, AATypeChecker::Error& typeError);

	/// <summary>
	/// Reset the static analyser unit
	/// </summary>
	/// <param name="funcs">Preregistered functions</param>
	/// <param name="classes">Preregistered classes</param>
	void Reset(std::vector<AAFuncSignature*> funcs, std::vector<AAClassSignature*> classes, std::vector<AACNamespace*> namespaces);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	AAStaticEnvironment GetResult() { return m_lastStaticEnv; }

	/// <summary>
	/// Verifies if the function's control path is valid, such that the function returns the correct amount of values
	/// </summary>
	/// <param name="sig">The function signature to check. Must have a valid <see cref="AA_AST_NODE"/> attached.</param>
	/// <param name="err">The error message that is filled out IF the method returns false</param>
	/// <returns>True only if the function is following a valid control structure</returns>
	bool VerifyFunctionControlPath(AAFuncSignature* sig, AAStaticEnvironment environment, AAC_CompileErrorMessage& err);

private:

	AAStaticEnvironment NewStaticEnvironment(AACNamespace*& globalDomain);

	bool ExtractGlobalScope(std::vector<AA_AST*>& trees);

	AAC_CompileErrorMessage PreregisterTypes(AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv);

	AAC_CompileErrorMessage FetchStaticDeclerationsFromTrees(std::vector<AA_AST*> trees, AACNamespace* globalDomain, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage FetchStaticDeclerationsFromASTNode(AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv);

	AAC_CompileErrorMessage RegisterFunction(AA_AST_NODE* pNode, AAFuncSignature*& sig, AACNamespace* domain, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage RegisterClass(AA_AST_NODE* pNode, AAClassSignature*& sig, AACNamespace* domain, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage RegisterEnum(AA_AST_NODE* pNode, AACEnumSignature*& sig, AACNamespace* domain, AAStaticEnvironment& senv);

	AAC_CompileErrorMessage VerifyDeclarations(AACNamespace* domain, AAStaticEnvironment environment);
	AAC_CompileErrorMessage VerifyInheritanceCircularDependency(AAClassSignature* signatureA, AAClassSignature* signatureB);

	AAC_CompileErrorMessage ApplyInheritance(AACNamespace* domain, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage ApplyInheritance(AAClassSignature* classSig, AAStaticEnvironment& senv);

	AAC_CompileErrorMessage HandleObjectInheritance(AAClassSignature* sig, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage HandleObjectInheritance(AACEnumSignature* sig, AAStaticEnvironment& senv);

	AAC_CompileErrorMessage HandleInheritanceFrom(AAClassSignature* child, AAClassSignature* super, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage HandleInheritanceFrom(AACEnumSignature* child, AAClassSignature* super, AAStaticEnvironment& senv);

	AACType* GetTypeFromName(std::wstring tName, AACNamespace* domain, AAStaticEnvironment& senv);

	int GetReturnCount(AAFuncSignature* funcSig);

	int VerifyFunctionControlPath(AA_AST_NODE* pNode, AAStaticEnvironment environment, AAC_CompileErrorMessage& err);

private:

	// The object all other objects must inherit from (Keep in mind inheritance has not been implemented yet, 10/04/20)
	AAClassSignature* m_objectInheritFrom;

	aa::set<AAClassSignature*> m_preregisteredClasses;
	aa::set<AAFuncSignature*> m_preregisteredFunctions;
	aa::set<AACNamespace*> m_preregisteredNamespaces;

	AAC* m_compilerPointer;

	AAStaticEnvironment m_lastStaticEnv;

};
