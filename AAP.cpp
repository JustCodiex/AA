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

	// If any error, abort now
	if (AA_PT::HasLastErrorMessage()) {
		result.success = false;
		result.firstMsg.errorMsg = AA_PT::GetLastErrorMessage().errMsg;
		result.firstMsg.errorSource = AA_PT::GetLastErrorMessage().errSrc;
		result.firstMsg.errorType = AA_PT::GetLastErrorMessage().errType;
		return result;
	}

	// Use unflattening tool to apply syntax rules
	AA_PT_Unflatten::ApplySyntaxRules(aa_pt_nodes);

	// Create parse tree
	AA_PT* pParseTree = AA_PT::CreateParseTree(aa_pt_nodes);
	
	// Instead of continuing with a flawed parse tree we'll just report the error
	if (AA_PT::HasLastErrorMessage()) {

		AA_PT::Error err = AA_PT::GetLastErrorMessage();
		result.firstMsg = AAP_SyntaxErrorMessage(err.errType, err.errMsg, err.errSrc);
		result.success = false;

	} else {

		// Do we succeed?
		bool isSucces = true;

		// Get the parse tree
		AA_AST* AST = new AA_AST(pParseTree);

		// If no error
		if (!AST->HasError()) {

			// Add AST to result
			result.result.Add(AST);

			// Clear the parse tree (No longer need it)
			pParseTree->Clear();

		} else {

			// Set error message
			AA_AST::Error err = AST->GetError();
			result.firstMsg = AAP_SyntaxErrorMessage(err.errType, err.errMsg, err.errSrc);
			result.success = false;

			// Not a success
			isSucces = false;

		}

		// Mark parse result as success
		result.success = isSucces;

	}

	// Return parse result
	return result;

}

void AAP::ClearTrees(aa::list<AA_AST*> trees) {

	// Clear all trees
	trees.ForEach(
		[](AA_AST*& tree) {
			tree->Clear();
		}
	);

}

void AAP::ClearError() {

	// Reset last error
	AA_PT::ResetLastError();

}
