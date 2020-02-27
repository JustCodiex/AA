#include "AALexer.h"
#include <sstream>
#include <map>

std::map<wchar_t, AAToken> singleTokens = {
	{ ' ', AAToken::whitespace },
	{ '\t', AAToken::whitespace },
	{ '\n', AAToken::whitespace },
	{ ';', AAToken::seperator },
	{ '+', AAToken::OP },
	{ '-', AAToken::OP },
	{ '*', AAToken::OP },
	{ '/', AAToken::OP },
	{ '%', AAToken::OP },
	{ '(', AAToken::seperator },
	{ ')', AAToken::seperator },
};

std::vector<AALexicalResult> AALexer::Analyse(std::wstring input) {

	std::vector< AALexicalResult> results;

	size_t index = 0;
	AAToken current = AAToken::invalid;

	std::wstringstream ws;

	while (index < input.length()) {

		wchar_t character = input[index];
		
		AAToken single;
		if (IsSingleToken(character, single)) {
			
			if (current != AAToken::invalid) {
				results.push_back(AALexicalResult(ws.str(), current));
				ws.str(L"");
			}

			ws << character;
			results.push_back(AALexicalResult(ws.str(), single));
			ws.str(L"");
			current = AAToken::invalid;

		} else if (IsDigit(character)) {
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

bool AALexer::IsDigit(wchar_t character) {
	return character >= (wchar_t)'0' && character <= (wchar_t)'9';
}
