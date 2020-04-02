#pragma once
#include "AAFuncSignature.h"
#include "AAClassCompiler.h"
#include "AACNamespace.h"
#include "AAC_Structs.h"
#include "AATypeChecker.h"
#include "AA_AST.h"

class AAC;

/// <summary>
/// A static environment for static data
/// </summary>
struct AAStaticEnvironment {

	AACNamespace* globalNamespace;
	aa::set<AAFuncSignature> availableFunctions;
	aa::set<AAClassSignature> availableClasses;
	aa::set<std::wstring> availableTypes;

	AAStaticEnvironment() {
		globalNamespace = 0;
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
	/// 
	/// </summary>
	/// <returns></returns>
	bool RunTypecheckAnalysis(AA_AST* pTree, AAStaticEnvironment senv, AATypeChecker::Error& typeError);

	/// <summary>
	/// Reset the static analyser unit
	/// </summary>
	/// <param name="funcs">Preregistered functions</param>
	/// <param name="classes">Preregistered classes</param>
	void Reset(std::vector<AAFuncSignature> funcs, std::vector<AAClassSignature> classes);

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
	bool VerifyFunctionControlPath(AAFuncSignature sig, AAC_CompileErrorMessage& err);

private:

	AAStaticEnvironment NewStaticEnvironment(AACNamespace*& globalDomain);

	bool ExtractGlobalScope(std::vector<AA_AST*>& trees);

	AAC_CompileErrorMessage FetchStaticDeclerationsFromTrees(std::vector<AA_AST*> trees, AACNamespace* globalDomain, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage FetchStaticDeclerationsFromASTNode(AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv);

	AAC_CompileErrorMessage RegisterFunction(AA_AST_NODE* pNode, AAFuncSignature& sig);
	AAC_CompileErrorMessage RegisterClass(AA_AST_NODE* pNode, AAClassSignature& sig);

	int GetReturnCount(AAFuncSignature funcSig);

	int VerifyFunctionControlPath(AA_AST_NODE* pNode, AAC_CompileErrorMessage& err);

private:

	aa::set<AAClassSignature> m_preregisteredClasses;
	aa::set<AAFuncSignature> m_preregisteredFunctions;

	AAC* m_compilerPointer;

	AAStaticEnvironment m_lastStaticEnv;

};
