#pragma once
#include "AALexer.h"
#include "AA_AST.h"

// Parser Class
class AAP {

public:

	AAP();

	void Release();

	std::vector< AA_AST*> Parse(std::wstring input);
	std::vector< AA_AST*> Parse(std::wifstream input);

	void ClearTrees(std::vector<AA_AST*> trees);

private:

	std::vector< AA_AST*> InternalParse(std::vector<AALexicalResult> res);
	std::vector< AA_AST *> CreateParseTrees(std::vector<AALexicalResult> lexResult);

private:

	AALexer* m_lexer;

};
