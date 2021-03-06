#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "AACodePosition.h"

enum class AAToken {
	invalid,
	whitespace,
	identifier,
	keyword,
	seperator,
	OP, // operator
	intlit,
	floatlit,
	doublelit,
	charlit,
	stringlit,
	stringOP, // special operator on string literal
	comment,
	accessor,
	escape,
	quote,
};

struct AALexicalResult {
	std::wstring content;
	AAToken token;
	AACodePosition position;
	AALexicalResult(std::wstring ws, AAToken token, AACodePosition pos) {
		this->content = ws;
		this->token = token;
		this->position = pos;
	}
};

class AALexer {

public:

	std::vector<AALexicalResult> Analyse(std::wistream& input);
	std::vector<AALexicalResult> Analyse(std::wstring input);

	void Join(std::vector<AALexicalResult>& results);

private:

	bool IsDigit(wchar_t character);
	bool IsCharacter(wchar_t character);
	bool IsWhitespace(wchar_t character);
	bool IsKeyword(std::wstring w);
	bool IsSingleToken(wchar_t character, AAToken& token);

	bool IsIdentifier(wchar_t character, AAToken currentToken);

	void DetermineToken(std::wstringstream& wss, AAToken token, std::vector< AALexicalResult>& results, AACodePosition pos);

	void JoinDecimal(std::vector<AALexicalResult>& results, size_t& i);

	bool IsValidJointKeyword(std::wstring ws);
	bool IsValidJointOperator(std::wstring ws);

	AAToken FirstNonWhitespaceBefore(std::vector<AALexicalResult> tokens, int i, int& o);
	AAToken FirstNonWhitespaceAfter(std::vector<AALexicalResult> tokens, int i, int& o);

	AALexicalResult Merge(std::vector<AALexicalResult> ls);

};
