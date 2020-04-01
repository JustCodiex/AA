#pragma once
#include "AAFuncSignature.h"
#include "AAClassCompiler.h"
#include "AACNamespace.h"
#include "AAC_Structs.h"
#include "AATypeChecker.h"
#include "AA_AST.h"

/// <summary>
/// A static environment for static data
/// </summary>
struct AAStaticEnvironment {



};

/// <summary>
/// Class for performaing static analysis on code
/// </summary>
class AAStaticAnalysis {

public:

	/// <summary>
	/// Run a static analysis on a list of ASTs
	/// </summary>
	/// <param name="trees">Abstract Syntax Trees to perform static analysis on</param>
	/// <returns></returns>
	AAC_CompileErrorMessage RunStaticAnalysis(std::vector<AA_AST*> trees);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	AAC_CompileErrorMessage RunTypecheckAnalysis();

	/// <summary>
	/// Reset the static analyser unit
	/// </summary>
	/// <param name="funcs">Preregistered functions</param>
	/// <param name="classes">Preregistered classes</param>
	void Reset(std::vector<AAFuncSignature> funcs, std::vector<AAClassSignature> classes);

private:

	AAC_CompileErrorMessage FetchStaticDeclerations(std::vector<AA_AST*> trees);

private:

	aa::set<AAClassSignature> m_preregisteredClasses;
	aa::set<AAFuncSignature> m_preregisteredFunctions;

};
