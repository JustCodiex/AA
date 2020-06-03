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
	{ '\\', AAToken::escape },
	{ '\'', AAToken::quote },
	{ '\"', AAToken::quote },
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
	{ '|', AAToken::OP },
	{ '~', AAToken::OP },
	{ '^', AAToken::OP },
	{ '#', AAToken::OP },
	{ '$', AAToken::OP },
};

std::vector<wchar_t> whitespaceCharacters = {
	' ',
	'\t',
	'\n',
};

std::vector<std::wstring> keywords = {
	
	// Inferred value type keyword (var => variable, typechecker will solve this)
	L"var",
	//L"val", // <-- contextualise
	
	// Boolean values (true and false)
	L"true",
	L"false",

	// Void-Types (null, void, Any)
	L"null",
	L"void",
	L"Any",

	// Flow-Control
	L"if",
	L"else",
	L"for",
	L"foreach",
	L"while",
	L"do",
	L"match",
	L"case",
	L"break",
	L"return",
	L"continue",
	L"yield", // <-- contextualise

	// Conditions and conversions
	L"when", // <-- contextualise
	L"as", // <-- contextualise
	L"is", // <-- contextualise
	
	// Class keywords
	L"class",
	L"new",
	L"this",
	L"base",
	
	// Enum keyword
	L"enum",

	// Try-catch keywords
	L"try",
	L"catch",
	L"throw",
	
	// Use statements
	L"using",
	L"from", // <-- contextualise
	L"namespace",

	// Access modifiers
	L"public",
	L"private",
	L"protected",
	L"static",

	// Storage modifiers
	L"abstract",
	L"interface",
	L"tagged", // <-- Contextualise
	L"sealed",

	// Linkage modifiers
	L"external",
	L"internal",

	// Method modifiers
	L"virtual",
	L"override",
	L"operator", // <-- Contextualise (Must be followed by an operator)

	// Pre-processing
	/*L"macro",
	L"def",
	L"ifdef",
	L"ifndef",
	L"undef",*/

};

// Small note: Keywords marked with contextualise means thier context should be considered when evaluating them
// such that the identifier 'light' has more precedence over the keyword iff it's not followed be the class keyword

std::vector<AALexicalResult> AALexer::Analyse(std::wistream& input) {

	std::vector< AALexicalResult> results;

	int currentLine = 1;
	int currentColumn = 1;

	wchar_t character;
	AAToken current = AAToken::invalid;

	bool isReadingSingleLineComment = false;

	std::wstringstream ws;

	while (input.get(character)) {

		AAToken single;

		if (IsWhitespace(character)) {

			if (character == '\n') {
				currentLine++;
				currentColumn = 0;
				isReadingSingleLineComment = false;
			}

			// Push token
			this->DetermineToken(ws, current, results, AACodePosition(currentLine, currentColumn - 1 - ws.str().length()));

			// Push the whitespace (We need it incase of strings (The AA_PT will ignore it)
			results.push_back(AALexicalResult(std::wstring(1, character), AAToken::whitespace, AACodePosition(currentLine, currentColumn)));

			ws.str(L"");
			current = AAToken::invalid;

		} else if (!isReadingSingleLineComment) {

			if (IsSingleToken(character, single)) {

				if (current != AAToken::invalid) {
					this->DetermineToken(ws, current, results, AACodePosition(currentLine, currentColumn - 1 - ws.str().length()));
					ws.str(L"");
				}

				if (character == '/') {
					if (results.size() > 0 && results.at(results.size() - 1).content == L"/") {
						results.erase(results.end() - 1); // Remove the previous /
						isReadingSingleLineComment = true; // Tell the parser we're reading a comment => don't care about what it says
						continue;
					}
				}

				ws << character;
				results.push_back(AALexicalResult(ws.str(), single, AACodePosition(currentLine, currentColumn)));

				ws.str(L"");
				current = AAToken::invalid;

			} else if (IsIdentifier(character, current)) {
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
			if (i > 0 && results[i - 1].token == AAToken::accessor) {
				AALexicalResult lR = AALexicalResult(results[i - 1].content + results[i].content, AAToken::accessor, results[i - 1].position);
				if (IsValidJointOperator(lR.content)) {
					results.erase(results.begin() + i - 1, results.begin() + i + 1);
					results.insert(results.begin() + i - 1, lR);
				} else {
					wprintf(L"Invalid joint accessor detected '%s'", lR.content.c_str());
				}
			} else if (i > 0 && results[i - 1].token == AAToken::intlit && i + 1 < results.size() && results[i + 1].token == AAToken::intlit) {
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
			int j;
			if (FirstNonWhitespaceBefore(results, i, j) == AAToken::keyword) {
				AALexicalResult lR = AALexicalResult(results[j].content + results[i].content, AAToken::keyword, results[j].position);
				if (IsValidJointKeyword(lR.content)) { // If the formed keyword is valid, we join it
					results.erase(results.begin() + j, results.begin() + i + 1);
					results.insert(results.begin() + j, lR);
				} // otherwise it might just be certain keywords put in sequence (eg. "this match ..." or "public void")
			}
		} else if (results[i].token == AAToken::quote) {

			if (results[i].content == L"'") { // SINGLE character => char literal

				if (i + 2 < results.size() && results[i+2].token == AAToken::quote && results[i+2].content == L"'") {

					results[i].content = results[i + 1].content;
					results[i].token = AAToken::charlit;

					results.erase(results.begin() + i + 1, results.begin() + i + 3);

				} else if (i + 3 < results.size() && results[i + 3].content == L"'") {
					if (results[i + 1].content == L"\\") {

						wchar_t other = results[i + 2].content[0];

						if (other == 't') {
							results[i].content = L"\t";
						} else if (other == 'n') {
							results[i].content = L"\n";
						} // Add more cases here
						else {
							wprintf(L"Invalid escape sequence detected!");
						}

						results[i].token = AAToken::charlit;
						results.erase(results.begin() + i + 1, results.begin() + i + 4);

					} else {
						wprintf(L"Invalid char literal detected!");
					}
				} else {
					wprintf(L"Invalid char literal detected!");
				}

			} else if (results[i].content == L"\"") {

				std::wstring ws = L"";
				size_t j = i;

				i++;

				while (i < results.size()) {

					if (results[i].token == AAToken::quote && results[i].content == L"\"") {
						break;
					} else {
						ws += results[i].content;
					}

					i++;

				}

				results.erase(results.begin() + j + 1, results.begin() + i + 1);

				results[j].token = AAToken::stringlit;
				results[j].content = ws;

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
	} else if (ws == L"::") {
		return true;
	} else if (ws == L"<<") {
		return true;
	} else if (ws == L">>") {
		return true;
	} else if (ws == L"=>") {
		return true;
	} else if (ws == L"||") {
		return true;
	} else if (ws == L"&&") {
		return true;
	} else if (ws == L"+=") {
		return true;
	} else if (ws == L"-=") {
		return true;
	} else if (ws == L"/=") {
		return true;
	} else if (ws == L"*=") {
		return true;
	} else if (ws == L"%=") {
		return true;
	} else {
		return false;
	}
}

AAToken AALexer::FirstNonWhitespaceBefore(std::vector<AALexicalResult> tokens, int i, int& o) {
	o = i - 1;
	while (o >= 0) {
		if (tokens[o].token != AAToken::whitespace) {
			return tokens[o].token;
		}
		o--;
	}
	return AAToken::invalid;
}

AAToken AALexer::FirstNonWhitespaceAfter(std::vector<AALexicalResult> tokens, int i, int& o) {
	o = i + 1;
	while (o < (int)tokens.size()) {
		if (tokens[o].token != AAToken::whitespace) {
			return tokens[o].token;
		}
		o++;
	}
	return AAToken::invalid;

}

AALexicalResult AALexer::Merge(std::vector<AALexicalResult> ls) {

	std::wstring content = L"";
	AACodePosition pos = ls[0].position;

	for (size_t i = 0; i < ls.size(); i++)
		content += ls[i].content;

	return AALexicalResult(content, AAToken::invalid, pos);

}

bool AALexer::IsIdentifier(wchar_t character, AAToken currentToken) {

	if (IsCharacter(character) && (currentToken == AAToken::identifier || currentToken == AAToken::invalid)) {
		return true;
	} else {
		if (currentToken == AAToken::identifier && IsDigit(character)) { // Are we already considered to be an identifier?
			return true;
		} else {
			return false;
		}
	}

}
