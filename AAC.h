#pragma once
#include "AA_AST.h"
#include "AALower.h"
#include "AAOptimiser.h"
#include "AAByteCode.h"
#include "AAByteType.h"
#include "AA_literals.h"
#include "AAStaticAnalysis.h"
#include "bstream.h"
#include "list.h"

class AAVM;

// Compiler Class
class AAC {

public:


public:

	AAC(AAVM* pVM);

	/// <summary>
	/// Setup internal compiler utilities
	/// </summary>
	void SetupCompiler();

	AAC_CompileResult CompileFromAbstractSyntaxTrees(aa::list<AA_AST*> trees);

	void SetOpListFile(std::wstring outFile) {
		m_outfile = outFile;
	}

	void SetUnparseFile(std::wstring outFile) {
		m_unparsefile = outFile;
	}

	void SetEntrypointFunction(std::wstring func) {
		m_overrideEntryPoint = func;
	}

	AAClassCompiler* GetClassCompilerInstance() { return m_classCompiler; }

	int GetNextProcID() { return ++m_currentProcID; }

	void AddVMClass(AAClassSignature* cc);
	void AddVMFunction(AAFuncSignature* sig);
	void AddVMNamespace(AACNamespace* dom);

	aa::list<AAByteType> GetTypedata() const { return m_byteTypes; }

	const unsigned char* GetCompilerVersion() const { return m_version; }

private:

	typedef CompiledAbstractExpression Instruction;
	typedef aa::list<Instruction> Instructions;

	/*
	** Private compiler methods
	*/

	CompiledProcedure CompileProcedureFromASTNode(AA_AST_NODE* pASTNode, AAStaticEnvironment staticData);
	aa::list<CompiledProcedure> CompileProcedureFromASTRootNode(AA_AST_NODE* pAstRootNode, AAStaticEnvironment staticData);
	AAC_Out CompileFromProcedures(aa::list<CompiledProcedure> procedures, AAStaticEnvironment staticCompileData, int entryPoint);
	aa::list<AAByteType> CompileTypedata(AAStaticEnvironment staticCompileData);
	aa::list<AACType*> FetchTypedata(AACNamespace* pNamespace);

	/*
	** Static checkers
	*/

	AAC_CompileErrorMessage RunStaticOperations(aa::list<AA_AST*>& trees, AAStaticEnvironment& staicData);
	
	/*
	** AST_NODE -> Bytecode functions
	*/

	Instructions CompileAST(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	Instructions CompileLambda(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);

	Instructions CompileBinaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	Instructions CompileUnaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	
	Instructions CompileAccessorOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	Instructions CompileEnumAccessorOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	Instructions CompileTupleAccessorOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);

	Instructions CompileFunctionCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	Instructions CompileFuncArgs(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	Instructions CompileLambdaCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);

	Instructions CompileCondition(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	Instructions CompileConditionalBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	Instructions CompileForBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	Instructions CompileWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	Instructions CompileDoWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);

	Instructions CompileNewStatement(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);

	Instructions CompilePatternBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	Instructions CompilePatternCondition(aa::list<Instruction> match, int scopeStart, AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);

	Instructions HandleCtorCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& ctable, AAStaticEnvironment staticData);
	Instructions HandleMemberCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);

	Instructions HandleStackPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode, AAStaticEnvironment staticData);
	Instructions HandleIndexPush(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, AAStaticEnvironment staticData);
	Instructions HandleTuplePush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode, AAStaticEnvironment staticData);

	Instructions HandlePop(AA_AST_NODE* pNode);

	Instruction HandleVarPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);
	Instruction HandleFieldPush(AA_AST_NODE* pNode, AAStaticEnvironment staticData);
	Instruction HandleConstPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);
	Instruction HandleConstPush(CompiledEnviornmentTable& ctable, AA_Literal lit);
	Instruction HandleConstPush(CompiledEnviornmentTable& cTable, bool bLit);

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

	AAByteType ConvertTypeToBytes(AACType* pCType, aa::list<AACType*>& typeList);
	void CompileClassToBytes(AAByteType& type, AACType* pCType, aa::list<AACType*>& typeList);

	AAByteVTable* ConvertClassVTableToBinary(AAClassSignature* pClass);

	void EnterScope(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);
	void ExitScope(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);

	/*
	** Bytecode functions
	*/

	void ConstTableToByteCode(CompiledEnviornmentTable constTable, aa::bstream& bis);
	void ConvertToBytes(Instruction expr, aa::bstream& bis);

	/*
	** Compiler State Functions
	*/

	void ResetCompilerInternals();

private:

	unsigned char m_version[10] = { 'v', '0', '0', '.', '0', '1', '.', '4', '0', '\0' };

	int m_currentProcID;
	std::wstring m_outfile;
	std::wstring m_unparsefile;

	AAVM* m_pAAVM;

	std::vector<AAClassSignature*> m_preregisteredClasses;
	std::vector<AAFuncSignature*> m_preregisteredFunctions;
	std::vector<AACNamespace*> m_preregisteredNamespaces;

	aa::list<AAByteType> m_byteTypes;

	AAClassCompiler* m_classCompiler;
	AAStaticAnalysis* m_staticAnalyser;

	int m_foundEntryPoint; // Found procedure matching index (if none is found, the global scope is executed)
	std::wstring m_overrideEntryPoint; // The designated override entry point

};
