#pragma once
#include "AALexer.h"

// Parser Class
class AAP {

public:

	AAP();

	void Release();

	void Parse(std::wstring input);

private:

	AALexer* m_lexer;

};
