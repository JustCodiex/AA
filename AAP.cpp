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

	// Tokenise the input with the lexer and then parse
	return this->InternalParse(m_lexer->Analyse(input));

}

std::vector< AA_AST*> AAP::Parse(std::wifstream input) {
	
	// Convert file stream to input stream
	std::wistream& wss = input;

	// Tokenise the input with the lexer and then parse
	return this->InternalParse(m_lexer->Analyse(wss));

}

std::vector< AA_AST*> AAP::InternalParse(std::vector<AALexicalResult> res) {

	// Join tokens that can be joined
	m_lexer->Join(res);

	// Create parse trees
	return this->CreateParseTrees(res);

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
		
		// Create AST
		AA_AST* abstractSyntaxTree = new AA_AST(parseTrees[i]);
		
		// Add AST to result
		abstractSyntaxTrees.push_back(abstractSyntaxTree);
		
		// Clear the parse tree (No longer need it)
		parseTrees[i]->Clear();

	}

	// Return their equivalent ASTs
	return abstractSyntaxTrees;

}

void AAP::ClearTrees(std::vector<AA_AST*> trees) {

	for (AA_AST* pTree : trees) {
		pTree->Clear();
	}

}
