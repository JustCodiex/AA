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

private:

	AAStaticEnvironment NewStaticEnvironment(AACNamespace*& globalDomain);

	void ExtractGlobalScope(std::vector<AA_AST*>& trees);

	AAC_CompileErrorMessage FetchStaticDeclerationsFromTrees(std::vector<AA_AST*> trees, AACNamespace* globalDomain, AAStaticEnvironment& senv);
	AAC_CompileErrorMessage FetchStaticDeclerationsFromASTNode(AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv);

	AAFuncSignature RegisterFunction(AA_AST_NODE* pNode);
	AAClassSignature RegisterClass(AA_AST_NODE* pNode);

	int GetReturnCount(AAFuncSignature funcSig);

private:

	aa::set<AAClassSignature> m_preregisteredClasses;
	aa::set<AAFuncSignature> m_preregisteredFunctions;

	AAC* m_compilerPointer;

	AAStaticEnvironment m_lastStaticEnv;

};
