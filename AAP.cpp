#include "AAP.h"
#include "AA_PT.h"

AACodePosition AACodePosition::Undetermined = AACodePosition(2147483647, 2147483647);

AAP::AAP() {

	// Allocate memory for the lexer
	m_lexer = new AALexer;

}

void AAP::Release() {

	// Delete lexer
	delete m_lexer;
	m_lexer = 0;

}

AAP_ParseResult AAP::Parse(std::wstring input) {

	// Tokenise the input with the lexer and then parse
	return this->InternalParse(m_lexer->Analyse(input));

}

AAP_ParseResult AAP::Parse(std::wifstream input) {
	
	// Convert file stream to input stream
	std::wistream& wss = input;

	// Tokenise the input with the lexer and then parse
	return this->InternalParse(m_lexer->Analyse(wss));

}

AAP_ParseResult AAP::InternalParse(std::vector<AALexicalResult> res) {

	// Join tokens that can be joined
	m_lexer->Join(res);

	// Create parse trees
	return this->CreateParseTrees(res);

}

AAP_ParseResult AAP::CreateParseTrees(std::vector<AALexicalResult> lexResult) {

	// Result of the parse operation
	AAP_ParseResult result;

	// Convert lexical analysis to AA_PT_NODEs
	std::vector<AA_PT_NODE*> aa_pt_nodes = AA_PT::ToNodes(lexResult);

	// Apply syntax rules
	AA_PT::ApplySyntaxRules(aa_pt_nodes);

	// Create parse trees
	std::vector<AA_PT*> parseTrees = AA_PT::CreateTrees(aa_pt_nodes);
	
	// Instead of continuing with a flawed parse tree we'll just report the error
	if (AA_PT::HasLastErrorMessage()) {

		AA_PT::Error err = AA_PT::GetLastErrorMessage();
		result.firstMsg = AAP_SyntaxErrorMessage(err.errType, err.errMsg, err.errSrc);
		result.success = false;

	} else {

		// Go through all the parse trees we received
		for (size_t i = 0; i < parseTrees.size(); i++) {

			// Create AST
			AA_AST* abstractSyntaxTree = new AA_AST(parseTrees[i]);

			// Add AST to result
			result.result.push_back(abstractSyntaxTree);

			// Clear the parse tree (No longer need it)
			parseTrees[i]->Clear();

		}

		// Mark parse result as success
		result.success = true;

	}

	// Return parse result
	return result;

}

void AAP::ClearTrees(std::vector<AA_AST*> trees) {

	for (AA_AST* pTree : trees) {
		pTree->Clear();
	}

}
