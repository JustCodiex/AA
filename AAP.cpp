#include "AAP.h"
#include "AA_PT.h"

AAP::AAP() {

	// Allocate memory for the lexer
	m_lexer = new AALexer;

}

void AAP::Release() {

	// Delete lexer
	delete m_lexer;
	m_lexer = 0;

}

std::vector< AA_AST*> AAP::Parse(std::wstring input) {

	// Tokenise the input with the lexer
	std::vector< AALexicalResult> lexed = m_lexer->Analyse(input);

	// Join tokens that can be joined
	m_lexer->Join(lexed);

	// Create parse trees
	return this->CreateParseTrees(lexed);

}

std::vector< AA_AST*> AAP::CreateParseTrees(std::vector<AALexicalResult> lexResult) {

	// Convert lexical analysis to AA_PT_NODEs
	std::vector<AA_PT_NODE*> aa_pt_nodes = AA_PT::ToNodes(lexResult);

	// Apply mathematical arithmetic and binding rules
	AA_PT::ApplyOrderOfOperationBindings(aa_pt_nodes);

	// Apply flow control
	AA_PT::ApplyFlowControlBindings(aa_pt_nodes);

	// Create parse trees
	std::vector<AA_PT*> parseTrees = AA_PT::CreateTrees(aa_pt_nodes);
	std::vector<AA_AST*> abstractSyntaxTrees;

	// Go through all the parse trees we received
	for (size_t i = 0; i < parseTrees.size(); i++) {
		abstractSyntaxTrees.push_back(new AA_AST(parseTrees[i]));
	}

	// Return their equivalent ASTs
	return abstractSyntaxTrees;

}
