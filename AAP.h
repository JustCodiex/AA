#pragma once
#include "AALexer.h"
#include "AA_AST.h"

// Parser Class
class AAP {

public:

	AAP();

	void Release();

	AA_AST* Parse(std::wstring input);

private:

	AALexer* m_lexer;

};
