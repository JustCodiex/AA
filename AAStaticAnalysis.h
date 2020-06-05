#pragma once
#include "AAClassCompiler.h"
#include "AAC_Structs.h"
#include "AATypeChecker.h"
#include "AAVars.h"

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
	/// Get the result of the static analysis in a static environment representation
	/// </summary>
	/// <returns></returns>
	AAStaticEnvironment GetResult() const { return m_lastStaticEnv; }

	/// <summary>
	/// Has a global scope been found in latest analysis
	/// </summary>
	/// <returns></returns>
	bool HasGlobalScope() const { return m_foundGlobalScope; }

	/// <summary>
	/// Verifies if the function's control path is valid, such that the function returns the correct amount of values
	/// </summary>
	/// <param name="sig">The function signature to check. Must have a valid <see cref="AA_AST_NODE"/> attached.</param>
	/// <param name="err">The error message that is filled out IF the method returns false</param>
	/// <returns>True only if the function is following a valid control structure</returns>
	bool VerifyFunctionControlPath(AAFuncSignature* sig, AAStaticEnvironment environment, AAC_CompileErrorMessage& err);

	/// <summary>
	/// Runs a vars check on the entire tree (Scope rules)
	/// </summary>
	/// <param name="vars">Vars object to handle tree vars run</param>
	/// <param name="pTree">Pointer to tree to fix</param>
	/// <returns>True if no variable problems occured - false if any scope error occured</returns>
	bool Vars(AAVars vars, AA_AST* pTree);

	/// <summary>
	/// Find the entry point to use when executing the program
	/// </summary>
	/// <param name="trees">The trees to find entry point in</param>
	/// <param name="senv">The static environment to retrieve functions from (Looked for here before running elsewhere)</param>
	/// <param name="overrideEntry">The name of the entry function to use (none = main)</param>
	/// <param name="foundEntry">The found entry</param>
	/// <returns>True if a valid entry point could be found; false if no entry point could be found</returns>
	bool FindEntryPoint(std::vector<AA_AST*> trees, AAStaticEnvironment senv, std::wstring overrideEntry, int& foundEntry);

private:

	AAStaticEnvironment NewStaticEnvironment(AACNamespace*& globalDomain);

	bool ExtractGlobalScope(std::vector<AA_AST*>& trees);

	/*
	* Type pre-registering
	*/

	AAC_CompileErrorMessage PreregisterTypes(AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv);

	AAC_CompileErrorMessage FetchStaticDeclerationsFromTrees(AACNamespace* globalDomain, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage FetchStaticDeclerationsFromASTNode(AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv);

	AAC_CompileErrorMessage RegisterFunction(AA_AST_NODE* pNode, AAFuncSignature*& sig, AACNamespace* domain, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage RegisterClass(AA_AST_NODE* pNode, AAClassSignature*& sig, AACNamespace* domain, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage RegisterEnum(AA_AST_NODE* pNode, AACEnumSignature*& sig, AACNamespace* domain, AAStaticEnvironment& senv);

	AAC_CompileErrorMessage VerifyDeclarations(AACNamespace* domain, AAStaticEnvironment environment);

	/*
	* Inheritance methods
	*/

	AAC_CompileErrorMessage VerifyInheritanceCircularDependency(AAClassSignature* signatureA, AAClassSignature* signatureB);

	AAC_CompileErrorMessage ApplyInheritance(AACNamespace* domain, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage ApplyInheritance(AAClassSignature* classSig, AAStaticEnvironment& senv);

	AAC_CompileErrorMessage RegisterVirtualFunctions(AAClassSignature* classSig, AAFuncSignature* funcSig) const;

	AAC_CompileErrorMessage HandleObjectInheritance(AAClassSignature* sig, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage HandleObjectInheritance(AACEnumSignature* sig, AAStaticEnvironment& senv);

	AAC_CompileErrorMessage HandleInheritanceFrom(AAClassSignature* child, AAClassSignature* super, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage HandleInheritanceFrom(AACEnumSignature* child, AAClassSignature* super, AAStaticEnvironment& senv);

	bool CanInheritFunction(AAClassSignature* pSubSig, AAClassSignature* pBaseSig, AAFuncSignature* pToInherit, AAC_CompileErrorMessage& compileErr);

	/*
	* Type methods
	*/

	std::wstring TypeIdentifierToString(AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv);

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
	AADynamicTypeEnvironment* m_dynamicTypeEnvironment;

	std::vector<AA_AST*>* m_workTrees;
	size_t m_currentTreeIndex;

	uint32_t m_typeIndex;

	bool m_foundGlobalScope;

};
