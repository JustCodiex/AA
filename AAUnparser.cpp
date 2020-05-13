#include "AAUnparser.h"
#include <stdarg.h>

// https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
bool hasEnding(std::wstring const& fullString, std::wstring const& ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}

void AAUnparser::Open(std::wstring targetFile) {

	if (targetFile.compare(L"") != 0) {

		m_stream = std::wofstream();
		m_stream.open(targetFile);

		m_isOpen = true;
		m_indent = 0;

	}

}

void AAUnparser::Close() {
	if (m_isOpen) {
		m_stream.flush();
		m_stream.close();
	}
}

void AAUnparser::Unparse(AA_AST* pTree) {
	if (m_isOpen) {
		m_stream << this->Unparse(pTree->GetRoot());
	}
}

std::wstring AAUnparser::Unparse(AA_AST_NODE* pNode) {

	std::wstring out = L"";

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::funcbody:
	case AA_AST_NODE_TYPE::classbody:
	case AA_AST_NODE_TYPE::enumbody:
	case AA_AST_NODE_TYPE::block: {
		out += this->WriteToString(L"{\n");
		this->IncreaseIndent();
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			std::wstring unparsed = this->WriteIndent() + this->WriteToString(this->Unparse(pNode->expressions[i]));
			if (hasEnding(unparsed, L"}\n")) {
				out += unparsed;
			} else {
				out += unparsed + L";\n";
			}
		}
		this->DecreaseIndent();
		out += this->WriteIndent() + this->WriteToString(L"}\n");
		break;
	}
	case AA_AST_NODE_TYPE::binop: {
		std::wstring left = this->Unparse(pNode->expressions[0]);
		std::wstring right = this->Unparse(pNode->expressions[1]);
		if (pNode->content.compare(L"+") == 0) {
			out = this->WriteToString(L"(%s + %s)", left, right);
		} else if (pNode->content.compare(L"*") == 0) {
			out = this->WriteToString(L"(%s * %s)", left, right);
		} else if (pNode->content.compare(L"-") == 0) {
			out = this->WriteToString(L"(%s - %s)", left, right);
		} else if (pNode->content.compare(L"/") == 0) {
			out = this->WriteToString(L"(%s / %s)", left, right);
		} else if (pNode->content.compare(L"%") == 0) {
			out = this->WriteToString(L"(%s % %s)", left, right);
		} else if (pNode->content.compare(L"=") == 0) {
			out = this->WriteToString(L"%s = %s", left, right);
		} else if (pNode->content.compare(L">=") == 0) {
			out = this->WriteToString(L"%s >= %s", left, right);
		} else if (pNode->content.compare(L">") == 0) {
			out = this->WriteToString(L"%s > %s", left, right);
		} else if (pNode->content.compare(L"<") == 0) {
			out = this->WriteToString(L"%s < %s", left, right);
		} else if (pNode->content.compare(L"<=") == 0) {
			out = this->WriteToString(L"%s <= %s", left, right);
		}
		break;
	}
	case AA_AST_NODE_TYPE::unop: {
		std::wstring right = this->Unparse(pNode->expressions[0]);
		if (pNode->content.compare(L"!") == 0) {
			out = this->WriteToString(L"(!%s)", right);
		} else if (pNode->content.compare(L"-") == 0) {
			out = this->WriteToString(L"(-%s)", right);
		}
		break;
	}
	case AA_AST_NODE_TYPE::vardecl: {
		out = this->WriteToString(L"var %s", pNode->content);
		break;
	}
	case AA_AST_NODE_TYPE::fundecl: {
		std::wstring rtype = pNode->expressions[AA_NODE_FUNNODE_RETURNTYPE]->content;
		std::wstring body = this->Unparse(pNode->expressions[AA_NODE_FUNNODE_BODY]);
		std::wstring args = this->Unparse(pNode->expressions[AA_NODE_FUNNODE_ARGLIST]);
		out = this->WriteToString(L"%s %s(%s) %s", rtype, pNode->content, args, body);
		break;
	}
	case AA_AST_NODE_TYPE::funarglist: 
		out = this->UnparseList(pNode);
		break;
	case AA_AST_NODE_TYPE::funarg:
		out = this->WriteToString(L"%s %s", this->Unparse(pNode->expressions[0]), pNode->content);
		break;
	case AA_AST_NODE_TYPE::typeidentifier:
		out = pNode->content;
		break;
	case AA_AST_NODE_TYPE::funcall:
		out = this->WriteToString(L"%s(%s)", pNode->content, this->UnparseList(pNode));
		break;
	case AA_AST_NODE_TYPE::memberaccess: {
		std::wstring lhs = this->Unparse(pNode->expressions[0]);
		std::wstring rhs = this->Unparse(pNode->expressions[1]);
		out = this->WriteToString(L"%s.%s", lhs, rhs);
		break;
	}
	case AA_AST_NODE_TYPE::callaccess:
	{
		std::wstring lhs = this->Unparse(pNode->expressions[0]);
		std::wstring rhs = this->Unparse(pNode->expressions[1]);
		out = this->WriteToString(L"%s.%s", lhs, rhs.substr(rhs.find_last_of(':') + 1));
		break;
	}
	case AA_AST_NODE_TYPE::fieldaccess: {
		std::wstring lhs = this->Unparse(pNode->expressions[0]);
		std::wstring rhs = this->Unparse(pNode->expressions[1]);
		out = this->WriteToString(L"%s.%s", lhs, rhs);
		break;
	}
	case AA_AST_NODE_TYPE::whilestatement: {
		out = this->WriteToString(L"while (%s) %s", this->Unparse(pNode->expressions[0]), this->Unparse(pNode->expressions[1]));
		break;
	}
	case AA_AST_NODE_TYPE::dowhilestatement: {
		out = this->WriteToString(L"do %s%swhile (%s)", this->Unparse(pNode->expressions[1]), this->WriteIndent(), this->Unparse(pNode->expressions[0]));
		break;
	}
	case AA_AST_NODE_TYPE::forstatement: {
		out = this->WriteToString(L"for (%s; %s; %s) %s", 
			this->Unparse(pNode->expressions[0]), 
			this->Unparse(pNode->expressions[1]), 
			this->Unparse(pNode->expressions[2]), 
			this->Unparse(pNode->expressions[3]));
		break;
	}
	case AA_AST_NODE_TYPE::forinit: {
		out = this->Unparse(pNode->expressions[0]);
		break;
	}
	case AA_AST_NODE_TYPE::forafterthought: {
		out = this->Unparse(pNode->expressions[0]);
		break;
	}
	case AA_AST_NODE_TYPE::condition: {
		std::wstring str = L"";
		for (auto& cond : pNode->expressions) {
			str += this->Unparse(cond);
		}
		out = str;
		break;
	}
	case AA_AST_NODE_TYPE::stringliteral:
		out = this->WriteToString(L"\"%s\"", pNode->content);
		break;
	case AA_AST_NODE_TYPE::charliteral:
		out = this->WriteToString(L"'%s'", pNode->content);
		break;
	case AA_AST_NODE_TYPE::variable:
	case AA_AST_NODE_TYPE::floatliteral:
	case AA_AST_NODE_TYPE::boolliteral:
	case AA_AST_NODE_TYPE::intliteral:
		out = pNode->content;
		break;
	default:
		wprintf(L"Failed to unparse nodetype: %i\n", (int)pNode->type);
		break;
	}

	return out;

}

std::wstring AAUnparser::UnparseList(AA_AST_NODE* pNode) {
	std::wstring args = L"";
	for (size_t i = 0; i < pNode->expressions.size(); i++) {
		std::wstring arg = this->Unparse(pNode->expressions[i]);
		if (i > 0) {
			args += L", " + arg;
		} else {
			args = arg;
		}
	}
	return args;
}

std::wstring AAUnparser::WriteIndent() {
	return std::wstring(m_indent, '\t');
}

std::wstring AAUnparser::WriteToString(std::wstring ln, ...) {

	std::wstringstream wis;

	va_list args;
	va_start(args, ln);

	for (size_t i = 0; i < ln.length(); i++) {

		if (ln[i] == '%' && i + 1 < ln.length()) {
			if (ln[i + 1] == 's') {
				wis << va_arg(args, std::wstring);
				i++;
			} else if (ln[i + 1] == 'i') {
				wis << va_arg(args, int);
				i++;
			} else if (ln[i + 1] == 'f') {
				wis << va_arg(args, float);
				i++;
			} else if (ln[i + 1] == 'd') {
				wis << va_arg(args, double);
				i++;
			}
			printf("");
		} else {
			wis << ln[i];
		}

	}

	va_end(args);

	return wis.str();

}

void AAUnparser::IncreaseIndent() {
	m_indent++;
}
void AAUnparser::DecreaseIndent() {
	m_indent--;
}
