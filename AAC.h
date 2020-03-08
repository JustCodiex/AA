#pragma once
#include "AA_AST.h"
#include "AAByteCode.h"
#include "AA_literals.h"
#include "AAFuncSignature.h"
#include "bstream.h"
#include "list.h"

// Compiled output
struct AAC_Out {
	unsigned long long length;
	unsigned char* bytes;
	AAC_Out() {
		length = 0;
		bytes = 0;
	}
};

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
		std::vector<CompiledAbstractExpression> procOperations;
		CompiledEnviornmentTable procEnvironment;
		AA_AST_NODE* node;
	};

	struct CompiledSignature {
		std::wstring name;
		int procID;
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
		std::vector<CompiledSignature> exportSignatures;
	};

public:

	AAC_Out CompileFromAbstractSyntaxTrees(std::vector<AA_AST*> trees);

	void SetOpListFile(std::wstring outFile) {
		m_outfile = outFile;
	}

private:

	/*
	** Private compiler methods
	*/

	CompiledProcedure CompileProcedureFromAST(AA_AST* pAbstractTree, CompiledStaticChecks staticData);
	AAC_Out CompileFromProcedures(std::vector<CompiledProcedure> procedures, CompiledStaticChecks staticCompileData, int entryPoint);

	/*
	** Static checkers
	*/

	CompiledStaticChecks RunStaticOperations(std::vector<AA_AST*> trees);
	void TypecheckAST(AA_AST* pTree);
	void CollapseGlobalScope(std::vector<AA_AST*>& trees);
	aa::list<CompiledStaticChecks::SigPointer> RegisterFunctions(AA_AST_NODE* pNode, AA_AST_NODE* pSource);
	CompiledStaticChecks::SigPointer RegisterFunction(AA_AST_NODE* pNode);

	/*
	** AST_NODE -> Bytecode functions
	*/

	std::vector<CompiledAbstractExpression> CompileAST(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	std::vector<CompiledAbstractExpression> CompileBinaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	std::vector<CompiledAbstractExpression> CompileUnaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	std::vector<CompiledAbstractExpression> CompileConditionalBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	std::vector<CompiledAbstractExpression> CompileFunctionCall(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	std::vector<CompiledAbstractExpression> CompileFuncArgs(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	std::vector<CompiledAbstractExpression> CompileForBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	std::vector<CompiledAbstractExpression> CompileWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);
	std::vector<CompiledAbstractExpression> CompileDoWhileBlock(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable, CompiledStaticChecks staticData);

	std::vector<CompiledAbstractExpression> HandleStackPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode, CompiledStaticChecks staticData);

	CompiledAbstractExpression HandleConstPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);
	CompiledAbstractExpression HandleVarPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);
	
	int HandleDecl(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);

	/*
	** Helper functions
	*/

	bool IsConstant(AA_AST_NODE_TYPE type);
	bool IsVariable(AA_AST_NODE_TYPE type);
	bool IsDecleration(AA_AST_NODE_TYPE type);
	std::vector<CompiledAbstractExpression> Merge(std::vector<CompiledAbstractExpression> original, std::vector<CompiledAbstractExpression> add);
	AAByteCode GetBytecodeFromBinaryOperator(std::wstring ws);
	AAByteCode GetBytecodeFromUnaryOperator(std::wstring ws);
	int GetReturnCount(void* funcSig);

	/*
	** FuncSig to procedure mapper
	*/
	std::vector< CompiledSignature> MapProcedureToSignature(CompiledStaticChecks staticCheck, std::vector<AAC::CompiledProcedure> procedureLs);
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

};
