#pragma once
#include "AAVM.h"

/// <summary>
/// Run all regression tests designed for the project
/// </summary>
/// <param name="pAAVM">Virtual machine instance to use</param>
/// <returns></returns>
bool RunRegressionTests(AAVM* pAAVM);

/// <summary>
/// Run a regression test on a file
/// </summary>
/// <param name="pAAVM">Virtual machine</param>
/// <param name="fileinput">The innput</param>
/// <param name="fileoutputBinary">Binary output file</param>
/// <param name="expectedoutput">The expected output</param>
/// <param name="expectParseError">Expects a parse error (Parser testing)</param>
/// <param name="expectCompileError">Expects a compile error (Compiler test)</param>
/// <param name="expectRuntimeError">Expects a runtime error (VM error testing)</param>
/// <returns></returns>
bool RunFileTest(
	AAVM* pAAVM, std::wstring fileinput, std::wstring fileoutput, AAStackValue expectedoutput, 
	bool expectParseError = false, bool expectCompileError = false, bool expectRuntimeError = false
);

/// <summary>
/// Run a regression test on an expression
/// </summary>
/// <param name="pAAVM">Virtual Machine</param>
/// <param name="expression">The input</param>
/// <param name="fileoutputBinary">Binary output file</param>
/// <param name="expectedoutput">The expected output</param>
/// <param name="expectParseError">Expects a parse error (Parser testing)</param>
/// <param name="expectCompileError">Expects a compile error (Compiler test)</param>
/// <param name="expectRuntimeError">Expects a runtime error (VM error testing)</param>
/// <returns></returns>
bool RunExpressionTest(
	AAVM* pAAVM, std::wstring expression, std::wstring fileoutput, AAStackValue expectedoutput,
	bool expectParseError = false, bool expectCompileError = false, bool expectRuntimeError = false
);
