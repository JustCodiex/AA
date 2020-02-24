#include "AAP.h"

AAP::AAP() {

	m_lexer = new AALexer;

}

void AAP::Release() {

	// Delete lexer
	delete m_lexer;
	m_lexer = 0;

}

void AAP::Parse(std::wstring input) {

	std::vector< AALexicalResult> lexed = m_lexer->Analyse(input);

}
