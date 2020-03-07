#include "AALexer.h"
#include <map>

std::map<wchar_t, AAToken> singleTokens = {
	{ ';', AAToken::seperator },
	{ ',', AAToken::seperator },
	{ '(', AAToken::seperator },
	{ ')', AAToken::seperator },
	{ '{', AAToken::seperator },
	{ '}', AAToken::seperator },
	{ '[', AAToken::seperator },
	{ ']', AAToken::seperator },
	{ '.', AAToken::accessor },
	{ ':', AAToken::accessor },
	{ '+', AAToken::OP },
	{ '-', AAToken::OP },
	{ '*', AAToken::OP },
	{ '/', AAToken::OP },
	{ '%', AAToken::OP },
	{ '=', AAToken::OP },
	{ '!', AAToken::OP },
	{ '?', AAToken::OP },
	{ '<', AAToken::OP },
	{ '>', AAToken::OP },
	{ '&', AAToken::OP },
};

std::vector<wchar_t> whitespaceCharacters = {
	' ',
	'\t',
	'\n',
};

std::vector<std::wstring> keywords = {
	L"var",
	L"true",
	L"false",
	L"void",
	L"if",
	L"else",
};

std::vector<AALexicalResult> AALexer::Analyse(std::wistream& input) {

	std::vector< AALexicalResult> results;

	int currentLine = 1;
	int currentColumn = 1;

	wchar_t character;
	AAToken current = AAToken::invalid;

	std::wstringstream ws;

	while (input.get(character)) {

		AAToken single;

		if (IsWhitespace(character)) {

			if (character == '\n') {
				currentLine++;
				currentColumn = 0;
			}

			// Push token
			this->DetermineToken(ws, current, results, AACodePosition(currentLine, currentColumn - 1 - ws.str().length()));

			ws.str(L"");
			current = AAToken::invalid;

		} else if (IsSingleToken(character, single)) {

			if (current != AAToken::invalid) {
				this->DetermineToken(ws, current, results, AACodePosition(currentLine, currentColumn - 1 - ws.str().length()));
				ws.str(L"");
			}

			ws << character;
			results.push_back(AALexicalResult(ws.str(), single, AACodePosition(currentLine, currentColumn)));

			ws.str(L"");
			current = AAToken::invalid;

		} else if (IsCharacter(character) && (current == AAToken::identifier || current == AAToken::invalid)) {
			ws << character;
			current = AAToken::identifier;
		} else if (IsDigit(character) && (current == AAToken::intlit || current == AAToken::invalid)) {
			ws << character;
			current = AAToken::intlit;
		} else {
			if (current == AAToken::intlit && character == 'f') {
				results.push_back(AALexicalResult(ws.str(), current, AACodePosition(currentLine, currentColumn))); // TODO: Fix incorrect column
				results.push_back(AALexicalResult(L"f", AAToken::identifier, AACodePosition(currentLine, currentColumn)));
				ws.str(L"");
				current = AAToken::invalid;
			}
		}

		currentColumn++;

	}

	return results;

}

std::vector<AALexicalResult> AALexer::Analyse(std::wstring input) {

	std::wstringstream wss;
	wss << input;

	return this->Analyse(wss);

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

void AALexer::DetermineToken(std::wstringstream& wss, AAToken token, std::vector< AALexicalResult>& results, AACodePosition pos) {

	std::wstring lword = wss.str();
	if (lword.length() > 0) {
		if (IsKeyword(lword)) {
			results.push_back(AALexicalResult(lword, AAToken::keyword, pos));
		} else {
			results.push_back(AALexicalResult(lword, token, pos));
		}
	}

}

void AALexer::Join(std::vector<AALexicalResult>& results) {

	size_t i = 0;

	while (i < results.size()) {
		
		if (results[i].token == AAToken::accessor) {
			if (i > 0 && results[i - 1].token == AAToken::intlit && i + 1 < results.size() && results[i + 1].token == AAToken::intlit) {
				this->JoinDecimal(results, i);
			}
		} else if (results[i].token == AAToken::OP) {
			if (i > 0 && results[i - 1].token == AAToken::OP) {
				AALexicalResult lR = AALexicalResult(results[i - 1].content + results[i].content, AAToken::OP, results[i-1].position);
				if (IsValidJointOperator(lR.content)) {
					results.erase(results.begin() + i - 1, results.begin() + i + 1);
					results.insert(results.begin() + i - 1, lR);
				} else {
					if (lR.content != L"+-" && lR.content != L"--") {
						wprintf(L"Invalid joint operator detected '%s'", lR.content.c_str());
					}
				}
			}
		} else if (results[i].token == AAToken::keyword) {
			if (i > 0 && results[i - 1].token == AAToken::keyword) {
				AALexicalResult lR = AALexicalResult(results[i - 1].content + results[i].content, AAToken::keyword, results[i - 1].position);
				if (IsValidJointKeyword(lR.content)) {
					results.erase(results.begin() + i - 1, results.begin() + i + 1);
					results.insert(results.begin() + i - 1, lR);
				} else {
					wprintf(L"Invalid joint keyword detected '%s'", lR.content.c_str());
				}
			}
		}

		i++;

	}

}

void AALexer::JoinDecimal(std::vector<AALexicalResult>& results, size_t& i) {

	size_t j = i;
	while (j >= 0 && results[j - 1].token == AAToken::intlit)
		j--;

	size_t k = i;
	while (k + 1 < results.size() && results[k + 1].token == AAToken::intlit)
		k++;

	std::vector<AALexicalResult> sub = std::vector<AALexicalResult>(results.begin() + j, results.begin() + k + 1);
	AAToken token = AAToken::doublelit;
	int l = 0;

	if (k + 1 < results.size() && results[k + 1].token == AAToken::identifier && results[k + 1].content == L"f") {
		token = AAToken::floatlit;
		l++;
	}

	AALexicalResult decmialLexRes = this->Merge(sub);
	decmialLexRes.token = token;

	results.erase(results.begin() + j, results.begin() + j + sub.size() + l);
	results.insert(results.begin() + j, decmialLexRes);

	i = j;

}

bool AALexer::IsValidJointKeyword(std::wstring ws) {
	if (ws == L"elseif") {
		return true;
	} else {
		return false;
	}
}

bool AALexer::IsValidJointOperator(std::wstring ws) {
	if (ws == L"<=") {
		return true;
	} else if (ws == L">=") {
		return true;
	} else if (ws == L"==") {
		return true;
	} else if (ws == L"!=") {
		return true;
	} else {
		return false;
	}
}

AALexicalResult AALexer::Merge(std::vector<AALexicalResult> ls) {

	std::wstring content = L"";
	AACodePosition pos = ls[0].position;

	for (size_t i = 0; i < ls.size(); i++)
		content += ls[i].content;

	return AALexicalResult(content, AAToken::invalid, pos);

}
