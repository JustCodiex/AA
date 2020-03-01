#include "AALexer.h"
#include <sstream>
#include <map>

std::map<wchar_t, AAToken> singleTokens = {
	{ ';', AAToken::seperator },
	{ '+', AAToken::OP },
	{ '-', AAToken::OP },
	{ '*', AAToken::OP },
	{ '/', AAToken::OP },
	{ '%', AAToken::OP },
	{ '=', AAToken::OP },
	{ '(', AAToken::seperator },
	{ ')', AAToken::seperator },
	{ '{', AAToken::seperator },
	{ '}', AAToken::seperator },
};

std::vector<wchar_t> whitespaceCharacters = {
	' ',
	'\t',
	'\n',
};

std::vector<std::wstring> keywords = {
	L"var",
};

std::vector<AALexicalResult> AALexer::Analyse(std::wstring input) {

	std::vector< AALexicalResult> results;

	size_t index = 0;
	AAToken current = AAToken::invalid;

	std::wstringstream ws;

	while (index < input.length()) {

		wchar_t character = input[index];
		AAToken single;

		if (IsWhitespace(character)) {

			std::wstring lword = ws.str();
			if (lword.length() > 0) {
				if (IsKeyword(lword)) {
					results.push_back(AALexicalResult(lword, AAToken::keyword));
				} else {
					results.push_back(AALexicalResult(lword, AAToken::identifier));
				}
			}

			ws.str(L"");
			current = AAToken::invalid;

		} else if (IsSingleToken(character, single)) {
			
			if (current != AAToken::invalid) {
				results.push_back(AALexicalResult(ws.str(), current));
				ws.str(L"");
			}

			ws << character;
			results.push_back(AALexicalResult(ws.str(), single));
			ws.str(L"");
			current = AAToken::invalid;

		} else if (IsCharacter(character) && (current == AAToken::identifier || current == AAToken::invalid)) {
			ws << character;
			current = AAToken::identifier;
		} else if (IsDigit(character) && (current == AAToken::intlit || current == AAToken::invalid)) {
			ws << character;
			current = AAToken::intlit;
		}

		index++;

	}

	return results;

}

bool AALexer::IsSingleToken(wchar_t character, AAToken& token) {

	if (singleTokens.find(character) != singleTokens.end()) {
		token = singleTokens[character];
		return true;
	}

	return false;

}

bool AALexer::IsWhitespace(wchar_t character) {

	if (std::find(whitespaceCharacters.begin(), whitespaceCharacters.end(), character) != whitespaceCharacters.end()) {
		return true;
	}

	return false;

}

bool AALexer::IsKeyword(std::wstring word) {

	if (std::find(keywords.begin(), keywords.end(), word) != keywords.end()) {
		return true;
	}

	return false;

}

bool AALexer::IsDigit(wchar_t character) {
	return character >= (wchar_t)'0' && character <= (wchar_t)'9';
}

bool AALexer::IsCharacter(wchar_t character) {
	return !IsDigit(character);
}
