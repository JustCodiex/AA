#pragma once
#include "AA_AST.h"
#include <unordered_map>
#include <functional>

namespace aa {
	typedef std::unordered_map<std::wstring, int> VarsEnviornment;
}

class AAVars {

public:

	/// <summary>
	/// Runs a vars run and handles bound and unbound variables - as well as scoping - on the tree
	/// </summary>
	/// <param name="pTree"></param>
	/// <returns></returns>
	bool Vars(AA_AST* pTree);

private:

	bool VarsScope(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

	bool VarsFunction(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

	bool VarsClass(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

	bool VarsEnum(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

	bool VarsNamespace(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

	bool VarsBranch(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

	bool VarsForLoop(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

	bool VarsMatchCase(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

	bool VarsMatchCondition(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

	bool VarsNode(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

};
