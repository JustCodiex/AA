#pragma once
#include "AA_AST.h"
#include "AAByteCode.h"

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
		int* argValues;
		CompiledAbstractExpression() {
			bc = AAByteCode::NOP;
			argCount = 0;
			argValues = 0;
		}
		CompiledAbstractExpression(AAByteCode code, int argCount, int* argVals) {
			this->bc = code;
			this->argCount = argCount;
			this->argValues = argVals;
		}
	};

public:

	AAC_Out CompileFromAST(AA_AST* pAbstractTree);

private:

	std::vector<CompiledAbstractExpression> CompileAST(AA_AST_NODE* pNode);

	std::vector<CompiledAbstractExpression> Merge(std::vector<CompiledAbstractExpression> original, std::vector<CompiledAbstractExpression> add);

	CompiledAbstractExpression CompileExpression(AA_AST_NODE* pNode);

	bool IsConstant(AA_AST_NODE_TYPE type);

};
