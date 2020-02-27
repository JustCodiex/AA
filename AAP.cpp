#include "AAP.h"
#include "AA_PT.h"

AAP::AAP() {

	m_lexer = new AALexer;

}

void AAP::Release() {

	// Delete lexer
	delete m_lexer;
	m_lexer = 0;

}

AA_AST* AAP::Parse(std::wstring input) {

	// Tokenise the input with the lexer
	std::vector< AALexicalResult> lexed = m_lexer->Analyse(input);

	// Build a parse tree
	AA_PT parseTree = AA_PT(lexed);

	// Build AST
	AA_AST* asTree = new AA_AST(&parseTree);

	// Return AST
	return asTree;

}
