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

// Parser Class
class AAP {

public:

	AAP();

	void Release();

	AAP_ParseResult Parse(std::wstring input);
	AAP_ParseResult Parse(std::wifstream input);

	void ClearTrees(std::vector<AA_AST*> trees);

private:

	AAP_ParseResult InternalParse(std::vector<AALexicalResult> res);
	AAP_ParseResult CreateParseTrees(std::vector<AALexicalResult> lexResult);

private:

	AALexer* m_lexer;

};
