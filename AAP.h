#pragma once
#include "AALexer.h"
#include "AA_AST.h"

// Error message
struct AAP_SyntaxErrorMessage {
	int errorType;
	AACodePosition errorSource;
	const char* errorMsg;
	AAP_SyntaxErrorMessage() {
		errorType = INT_MAX;
		errorSource = AACodePosition(0, 0);
		errorMsg = 0;
	}
	AAP_SyntaxErrorMessage(int eType, const char* eMsg) {
		errorMsg = eMsg;
		errorType = eType;
		errorSource = AACodePosition(0, 0);
	}
	AAP_SyntaxErrorMessage(int eType, const char* eMsg, AACodePosition eSrc) {
		errorMsg = eMsg;
		errorType = eType;
		errorSource = eSrc;
	}
};

// The compiler result
struct AAP_ParseResult {
	std::vector< AA_AST*> result;
	bool success;
	AAP_SyntaxErrorMessage firstMsg;
	AAP_ParseResult() {
		success = false;
		firstMsg = AAP_SyntaxErrorMessage();
	}
};

/// <summary>
/// Parser class for parsing code into a AST
/// </summary>
class AAP {

public:

	AAP();

	/// <summary>
	/// Release all resources used by the parser
	/// </summary>
	void Release();

	/// <summary>
	/// Parse code directly from text input
	/// </summary>
	/// <param name="input"></param>
	/// <returns>Parsing result</returns>
	AAP_ParseResult Parse(std::wstring input);
	
	/// <summary>
	/// Parse code using a file input
	/// </summary>
	/// <param name="input">File input stream</param>
	/// <returns>Parsing result</returns>
	AAP_ParseResult Parse(std::wifstream input);

	/// <summary>
	/// Clear all trees (Release them)
	/// </summary>
	/// <param name="trees"></param>
	void ClearTrees(std::vector<AA_AST*> trees);

	/// <summary>
	/// Clear errors found (in case a second parsing is reuqested
	/// </summary>
	void ClearError();

private:

	AAP_ParseResult InternalParse(std::vector<AALexicalResult> res);
	AAP_ParseResult CreateParseTrees(std::vector<AALexicalResult> lexResult);

private:

	AALexer* m_lexer;

};
