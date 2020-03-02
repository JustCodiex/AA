#pragma once
#include "AA_AST.h"
#include "AAByteCode.h"
#include "AA_literals.h"
#include "bstream.h"
#include "list.h"
#include <set>

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
	};

public:

	CompiledProcedure CompileProcedureFromAST(AA_AST* pAbstractTree);
	AAC_Out CompileFromProcedures(std::vector<CompiledProcedure> procedures);

	AAC_Out CompileFromAbstractSyntaxTrees(std::vector<AA_AST*> trees);

private:

	void TypecheckAST(AA_AST* pTree);

	std::vector<CompiledAbstractExpression> CompileFunctions(AA_AST_NODE* pNode);
	std::vector<CompiledAbstractExpression> CompileAST(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable);

	/*
	** AST_NODE -> Bytecode functions
	*/

	std::vector<CompiledAbstractExpression> CompileBinaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable);
	std::vector<CompiledAbstractExpression> CompileUnaryOperation(AA_AST_NODE* pNode, CompiledEnviornmentTable& cTable);
	std::vector<CompiledAbstractExpression> HandleStackPush(CompiledEnviornmentTable& cTable, AA_AST_NODE* pNode);
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

	/*
	** Bytecode functions
	*/

	void ToByteCode(std::vector<CompiledAbstractExpression> bytecodes, CompiledEnviornmentTable constTable, AAC_Out& result);
	void ConstTableToByteCode(CompiledEnviornmentTable constTable, aa::bstream& bis);
	void ConvertToBytes(CompiledAbstractExpression expr, aa::bstream& bis);

};
