#pragma once
#include "AA_AST.h"
#include "AAByteCode.h"
#include "AA_literals.h"
#include "AAStaticAnalysis.h"
#include "bstream.h"
#include "list.h"

class AAVM;

// Compiler Class
class AAC {

public:

	struct CompiledAbstractExpression {
		AAByteCode bc;
		int argCount;
		int argValues[8];
		CompiledAbstractExpression() {
			bc = AAByteCode::NOP;
			argCount = 0;
			memset(argValues, -1, sizeof(argValues));
		}
		CompiledAbstractExpression(AAByteCode code, int argCount, int* argVals) {
			this->bc = code;
			this->argCount = argCount;
			memset(argValues, -1, sizeof(argValues));
			memcpy(this->argValues, argValues, sizeof(int) * argCount);
		}
	};

	struct CompiledEnviornmentTable {
		aa::list<AA_Literal> constValues;
		aa::list<std::wstring> identifiers;
	};

	struct CompiledProcedure {
		aa::list<CompiledAbstractExpression> procOperations;
		CompiledEnviornmentTable procEnvironment;
		AA_AST_NODE* node;
		CompiledProcedure() { node = 0; }
	};

public:

	AAC(AAVM* pVM);

	void SetupCompiler();

	AAC_CompileResult CompileFromAbstractSyntaxTrees(std::vector<AA_AST*> trees);

	void SetOpListFile(std::wstring outFile) {
		m_outfile = outFile;
	}

	AAClassCompiler* GetClassCompilerInstance() { return m_classCompiler; }

	int GetNextProcID() { return ++m_currentProcID; }

	void AddVMClass(AAClassSignature* cc);
	void AddVMFunction(AAFuncSignature* sig);
	void AddVMNamespace(AACNamespace* dom);

private:

	/*
	** Private compiler methods
	*/

	CompiledProcedure CompileProcedureFromASTNode(AA_AST_NODE* pASTNode, AAStaticEnvironment staticData);
	aa::list<CompiledProcedure> CompileProcedureFromASTRootNode(AA_AST_NODE* pAstRootNode, AAStaticEnvironment staticData);
	AAC_Out CompileFromProcedures(aa::list<CompiledProcedure> procedures, AAStaticEnvironment staticCompileData, int entryPoint);

	/*
	** Static checkers
	*/

	AAC_CompileErrorMessage RunStaticOperations(std::vector<AA_AST*>& trees, AAStaticEnvironment& staicData);
	
	/*
	** AST_NODE -> Bytecode functions
	*/

	aa::list<CompiledAbstractExpression> CompileAST(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> CompileBinaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> CompileUnaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> CompileAccessorOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> CompileEnumAccessorOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> CompileConditionalBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> CompileFunctionCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> CompileFuncArgs(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> CompileForBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> CompileWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> CompileDoWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> CompileNewStatement(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);

	aa::list<CompiledAbstractExpression> CompilePatternBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> CompilePatternCondition(aa::list<CompiledAbstractExpression> match, AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);

	aa::list<CompiledAbstractExpression> HandleCtorCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& ctable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> HandleStackPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> HandleMemberCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	aa::list<CompiledAbstractExpression> HandleIndexPush(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);

	CompiledAbstractExpression HandleVarPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);
	CompiledAbstractExpression HandleFieldPush(AA_AST_NODE* pNode, AAStaticEnvironment staticData);
	CompiledAbstractExpression HandleConstPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);
	CompiledAbstractExpression HandleConstPush(CompiledEnviornmentTable& ctable, AA_Literal lit);

	int HandleDecl(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);

	/*
	** Helper functions
	*/

	bool IsConstant(AA_AST_NODE_TYPE type);
	bool IsVariable(AA_AST_NODE_TYPE type);
	bool IsDecleration(AA_AST_NODE_TYPE type);
	bool IsEnumeration(AA_AST_NODE_TYPE type);
	AAByteCode GetBytecodeFromBinaryOperator(std::wstring ws, AA_AST_NODE_TYPE lhsType);
	AAByteCode GetBytecodeFromUnaryOperator(std::wstring ws);

	// Check if the stack will return as many values as it says it will 
	bool VerifyFunctionCallstack(aa::list<CompiledAbstractExpression> body, int expected, int args, AAStaticEnvironment staticData);
	int CalcStackSzAfterOperation(CompiledAbstractExpression op, AAStaticEnvironment staticData);

	/*
	** Bytecode functions
	*/

	void ConstTableToByteCode(CompiledEnviornmentTable constTable, aa::bstream& bis);
	void ConvertToBytes(CompiledAbstractExpression expr, aa::bstream& bis);

	/*
	** Compiler State Functions
	*/

	void ResetCompilerInternals();

private:

	int m_currentProcID;
	std::wstring m_outfile;

	AAVM* m_pAAVM;

	std::vector<AAClassSignature*> m_preregisteredClasses;
	std::vector<AAFuncSignature*> m_preregisteredFunctions;
	std::vector<AACNamespace*> m_preregisteredNamespaces;

	AAClassCompiler* m_classCompiler;
	AAStaticAnalysis* m_staticAnalyser;

};
