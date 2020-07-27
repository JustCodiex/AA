#pragma once
#include "AA_AST.h"
#include "AAC_Structs.h"
#include <unordered_map>
#include <functional>

namespace aa {

	/// <summary>
	/// Represents a compile-time variable (See AAVars.h and AAVars.Cpp for use)
	/// </summary>
	struct Var {
		
		int vid;
		bool isMutable;
		
		Var() { this->vid = -1; this->isMutable = false; }
		Var(int v, bool m) { this->vid = v; this->isMutable = m; }

		operator int() const { return vid; }
		
		bool operator==(Var other) const { return vid == other.vid; }
		bool operator!=(Var other) const { return vid != other.vid; }
	
	};

	typedef std::unordered_map<std::wstring, Var> VarsEnviornment;
}



class AAVars {

public:

	/// <summary>
	/// Runs a vars run and handles bound and unbound variables - as well as scoping - on the tree. Additionally, it verifies the use of variables
	/// </summary>
	/// <param name="pTree"></param>
	/// <returns></returns>
	bool Vars(AA_AST* pTree);

	/// <summary>
	/// Retrieve the last error found in the vars run
	/// </summary>
	/// <returns></returns>
	const AAC_CompileErrorMessage& GetError() { return m_error; }

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

	bool VarsAssignment(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

	bool VarsUnary(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

	bool VarsLambdaExpression(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

	bool VarsNode(aa::VarsEnviornment& venv, AA_AST_NODE* pScope);

private:

	AAC_CompileErrorMessage m_error;

};
