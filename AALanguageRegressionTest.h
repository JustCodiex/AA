#pragma once
#include "AAVM.h"

bool RunRegressionTests(AAVM* pAAVM);

bool RunFileTest(AAVM* pAAVM, std::wstring fileinput, std::wstring fileoutputBinary, std::wstring fileoutputOpCodes, std::wstring expectedoutput);

bool RunExpressionTest(AAVM* pAAVM, std::wstring expression, std::wstring fileoutputBinary, std::wstring fileoutputOpCodes, std::wstring expectedoutput);
