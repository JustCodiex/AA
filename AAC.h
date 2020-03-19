#pragma once
#include "AA_AST.h"
#include "AAByteCode.h"
#include "AA_literals.h"
#include "AAC_Structs.h"
#include "AATypeChecker.h"
#include "AAFuncSignature.h"
#include "AAClassCompiler.h"
#include "bstream.h"
#include "list.h"

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

	struct CompiledStaticChecks {
		struct SigPointer {
			AAFuncSignature funcSig;
			AA_AST_NODE* node;
			int procID;
			SigPointer(AAFuncSignature s, AA_AST_NODE* n) {
				funcSig = s;
				node = n;
				procID = -1;
			}
		};
		aa::list<SigPointer> registeredFunctions;
		aa::list<CompiledClass> registeredClasses;
		aa::list<std::wstring> registeredTypes;
		aa::list<AAFuncSignature> GetSignatures() {
			aa::list<AAFuncSignature> sigs;
			for (size_t i = 0; i < registeredFunctions.Size(); i++) {
				sigs.Add(registeredFunctions.At(i).funcSig);
			}
			return sigs;
		}
	};

public:

	void SetupCompiler();

	AAC_CompileResult CompileFromAbstractSyntaxTrees(std::vector<AA_AST*> trees);

	void SetOpListFile(std::wstring outFile) {
		m_outfile = outFile;
	}

	AAClassCompiler* GetClassCompilerInstance() { return m_classCompiler; }

private:

	/*
	** Private compiler methods
	*/

	CompiledProcedure CompileProcedureFromAST(AA_AST* pAbstractTree, CompiledStaticChecks staticData);
	CompiledProcedure CompileProcedureFromASTNode(AA_AST_NODE* pASTNode, CompiledStaticChecks);
	AAC_Out CompileFromProcedures(std::vector<CompiledProcedure> procedures, CompiledStaticChecks staticCompileData, int entryPoint);

	/*
	** Static checkers
	*/

	AAC_CompileErrorMessage RunStaticOperations(std::vector<AA_AST*> trees, CompiledStaticChecks& staicData);
	bool TypecheckAST(AA_AST* pTree, CompiledStaticChecks staticData, AATypeChecker::Error& typeError);
	void CollapseGlobalScope(std::vector<AA_AST*>& trees);
	
	aa::list<CompiledStaticChecks::SigPointer> RegisterFunctions(AA_AST_NODE* pNode);

	CompiledStaticChecks::SigPointer RegisterFunction(AA_AST_NODE* pNode);
	CompiledClass RegisterClass(AA_AST_NODE* pNode);

	/*
	** AST_NODE -> Bytecode functions
	*/

	aa::list<CompiledAbstractExpression> CompileAST(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> CompileBinaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> CompileUnaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> CompileAccessorOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> CompileConditionalBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> CompileFunctionCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> CompileFuncArgs(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> CompileForBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> CompileWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> CompileDoWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> CompileNewStatement(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);

	aa::list<CompiledAbstractExpression> HandleCtorCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& ctable, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> HandleStackPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> HandleMemberCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	aa::list<CompiledAbstractExpression> HandleIndexPush(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);

	CompiledAbstractExpression HandleConstPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);
	CompiledAbstractExpression HandleVarPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);
	CompiledAbstractExpression HandleFieldPush(AA_AST_NODE* pNode, CompiledStaticChecks staticData);

	int HandleDecl(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);

	/*
	** Helper functions
	*/

	bool IsConstant(AA_AST_NODE_TYPE type);
	bool IsVariable(AA_AST_NODE_TYPE type);
	bool IsDecleration(AA_AST_NODE_TYPE type);
	AAByteCode GetBytecodeFromBinaryOperator(std::wstring ws, AA_AST_NODE_TYPE lhsType);
	AAByteCode GetBytecodeFromUnaryOperator(std::wstring ws);
	int GetReturnCount(void* funcSig);

	/*
	** FuncSig to procedure mapper
	*/
	int FindBestFunctionMatch(CompiledStaticChecks staticCheck, AA_AST_NODE* pNode, int& argCount);

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

	AAClassCompiler* m_classCompiler;

};
