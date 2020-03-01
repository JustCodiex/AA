#pragma once
#include <vector>
#include <string>

enum class AAToken {
	invalid,
	whitespace,
	identifier,
	keyword,
	seperator,
	OP, // operator
	intlit, // literal
	comment,
};

struct AALexicalResult {
	std::wstring content;
	AAToken token;
	AALexicalResult(std::wstring ws, AAToken token) {
		this->content = ws;
		this->token = token;
	}
};

class AALexer {

public:

	std::vector<AALexicalResult> Analyse(std::wstring input);

private:

	bool IsDigit(wchar_t character);
	bool IsCharacter(wchar_t character);
	bool IsWhitespace(wchar_t character);
	bool IsKeyword(std::wstring w);
	bool IsSingleToken(wchar_t character, AAToken& token);

};
