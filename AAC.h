#pragma once
#include "AA_AST.h"
#include "AAByteCode.h"
#include "AA_literals.h"
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
			memset(argValues, -128, 8);
		}
		CompiledAbstractExpression(AAByteCode code, int argCount, int* argVals) {
			this->bc = code;
			this->argCount = argCount;
			memset(argValues, -128, 8);
			memcpy(this->argValues, argValues, sizeof(int) * argCount);
		}
	};

	struct CompiledConstantTable {
		aa::list<AA_Literal> constValues;
	};

public:

	AAC_Out CompileFromAST(AA_AST* pAbstractTree);

private:

	std::vector<CompiledAbstractExpression> CompileAST(AA_AST_NODE* pNode, CompiledConstantTable& cTable);

	/*
	** AST_NODE -> Bytecode functions
	*/

	std::vector<CompiledAbstractExpression> CompileBinaryOperation(AA_AST_NODE* pNode, CompiledConstantTable& cTable);
	std::vector<CompiledAbstractExpression> CompileUnaryOperation(AA_AST_NODE* pNode, CompiledConstantTable& cTable);
	CompiledAbstractExpression HandleConstPush(CompiledConstantTable& cTable, AA_AST_NODE* pNode);

	/*
	** Helper functions
	*/

	bool IsConstant(AA_AST_NODE_TYPE type);
	std::vector<CompiledAbstractExpression> Merge(std::vector<CompiledAbstractExpression> original, std::vector<CompiledAbstractExpression> add);
	AAByteCode GetBytecodeFromBinaryOperator(std::wstring ws);
	AAByteCode GetBytecodeFromUnaryOperator(std::wstring ws);

	/*
	** Bytecode functions
	*/

	void ToByteCode(std::vector<CompiledAbstractExpression> bytecodes, CompiledConstantTable constTable, AAC_Out& result);
	void ConstTableToByteCode(CompiledConstantTable constTable, aa::bstream& bis);
	void ConvertToBytes(CompiledAbstractExpression expr, aa::bstream& bis);

};
