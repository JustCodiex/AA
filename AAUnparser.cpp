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

AAUnparser::AAUnparser() {
	this->m_indent = 0;
	this->m_isOpen = false;
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
		if (pNode->content.compare(L"=") == 0) {
			out = this->WriteToString(L"%s = %s", left, right);
		} else {
			out = this->WriteToString(L"(%s %s %s)", left, pNode->content, right);
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
		if (pNode->expressions.size() > 0) {
			out = this->WriteToString(L"%s %s", this->Unparse(pNode->expressions[0]), pNode->content);
		} else {
			out = this->WriteToString(L"var %s", pNode->content);
		}
		break;
	}
	case AA_AST_NODE_TYPE::fundecl: {
		std::wstring rtype = pNode->expressions[AA_NODE_FUNNODE_RETURNTYPE]->content;
		std::wstring body = this->Unparse(pNode->expressions[AA_NODE_FUNNODE_BODY]);
		std::wstring args = this->Unparse(pNode->expressions[AA_NODE_FUNNODE_ARGLIST]);
		std::wstring name = pNode->content;
		size_t k = name.find_last_of(':');
		if (k != std::wstring::npos) {
			name = name.substr(k + 1);
		}
		out = this->WriteToString(L"%s %s(%s) %s", rtype, name, args, body);
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
	case AA_AST_NODE_TYPE::classdecl: {
		std::wstring modifiers = L"";
		std::wstring header = pNode->content;
		std::wstring inherits = this->UnparseList(pNode->expressions[AA_NODE_CLASSNODE_INHERITANCE]);
		if (inherits.size() > 0) {
			header += L" : " + inherits;
		}
		if (pNode->expressions.size() >= AA_NODE_CLASSNODE_BODY) {
			out = this->WriteToString(L"%sclass %s %s", modifiers, header, this->Unparse(pNode->expressions[AA_NODE_CLASSNODE_BODY]));
			out[out.length() - 1] = ';';
			out += L"\n";
		} else {
			out = this->WriteToString(L"%sclass %s;", modifiers, header, this->Unparse(pNode->expressions[AA_NODE_CLASSNODE_BODY]));
		}
		break;
	}
	case AA_AST_NODE_TYPE::name_space: {
		std::wstring space_body = L"{\n";
		this->IncreaseIndent();
		for (auto& element : pNode->expressions) {
			space_body += this->WriteIndent() + this->Unparse(element);
		}
		this->DecreaseIndent();
		space_body += this->WriteIndent() + L"}\n";
		out = this->WriteToString(L"namespace %s %s", pNode->content, space_body);
		break;
	}
	case AA_AST_NODE_TYPE::enumdecleration: {
		std::wstring body = this->WriteToString(L"{\n");
		this->IncreaseIndent();
		for (size_t i = 0; i < pNode->expressions[AA_NODE_ENUMNODE_VALUES]->expressions.size(); i++) {
			if (i < pNode->expressions[AA_NODE_ENUMNODE_VALUES]->expressions.size() - 1) {
				body += this->WriteIndent() + pNode->expressions[AA_NODE_ENUMNODE_VALUES]->expressions[i]->content + L",\n"; // TODO: Unparse num value (incase it's a constant evaluation)
			} else {
				body += this->WriteIndent() + pNode->expressions[AA_NODE_ENUMNODE_VALUES]->expressions[i]->content + L"\n"; // TODO: Unparse num value (incase it's a constant evaluation)
			}
		}
		for (size_t i = 0; i < pNode->expressions[AA_NODE_ENUMNODE_BODY]->expressions.size(); i++) {
			std::wstring unparsed = this->WriteIndent() + this->WriteToString(this->Unparse(pNode->expressions[AA_NODE_ENUMNODE_BODY]->expressions[i]));
			if (hasEnding(unparsed, L"}\n")) {
				body += unparsed;
			} else {
				body += unparsed + L";\n";
			}
		}
		this->DecreaseIndent();
		body += this->WriteIndent() + this->WriteToString(L"}\n");
		out = this->WriteToString(L"enum %s %s", pNode->content, body);
		break;
	}
	case AA_AST_NODE_TYPE::memberaccess: {
		std::wstring lhs = this->Unparse(pNode->expressions[0]);
		std::wstring rhs = this->Unparse(pNode->expressions[1]);
		out = this->WriteToString(L"%s::%s", lhs, rhs);
		break;
	}
	case AA_AST_NODE_TYPE::callaccess:
	{
		std::wstring lhs = this->Unparse(pNode->expressions[0]);
		std::wstring rhs = this->Unparse(pNode->expressions[1]);
		out = this->WriteToString(L"%s.%s", lhs, rhs.substr(rhs.find_last_of(':') + 1));
		break;
	}
	case AA_AST_NODE_TYPE::tupleaccess:
	case AA_AST_NODE_TYPE::fieldaccess: {
		std::wstring lhs = this->Unparse(pNode->expressions[0]);
		std::wstring rhs = this->Unparse(pNode->expressions[1]);
		out = this->WriteToString(L"%s.%s", lhs, rhs);
		break;
	}
	case AA_AST_NODE_TYPE::enumvalueaccess: {
		std::wstring lhs = this->Unparse(pNode->expressions[0]);
		std::wstring rhs = this->Unparse(pNode->expressions[1]);
		out = this->WriteToString(L"%s::%s", lhs, rhs);
		break;
	}
	case AA_AST_NODE_TYPE::enumidentifier: {
		out = pNode->content;
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
	case AA_AST_NODE_TYPE::ifstatement: {
		out = this->WriteToString(L"if (%s) {\n", this->Unparse(pNode->expressions[0]));
		this->IncreaseIndent();
		out += this->WriteIndent() + this->WriteToString(L"%s\n", this->Unparse(pNode->expressions[1]));
		this->DecreaseIndent();
		out += this->WriteIndent() + L"}";
		for (size_t i = 2; i < pNode->expressions.size(); i++) {
			if (pNode->expressions[i]->type == AA_AST_NODE_TYPE::ifstatement) {
				out += this->WriteToString(L" else if (%s) {\n", this->Unparse(pNode->expressions[i]->expressions[0]));
				this->IncreaseIndent();
				out += this->WriteIndent() + this->WriteToString(L"%s\n", this->Unparse(pNode->expressions[i]->expressions[1]));
				this->DecreaseIndent();
				out += this->WriteIndent() + L"}";
			} else {
				out += this->Unparse(pNode->expressions[i]);
			}
		}
		break;
	}
	case AA_AST_NODE_TYPE::elsestatement: {
		out = this->WriteToString(L" else {\n");
		this->IncreaseIndent();
		out += this->WriteIndent() + this->WriteToString(L"%s\n", this->Unparse(pNode->expressions[0]));
		this->DecreaseIndent();
		out += this->WriteIndent() + L"}";
		break;
	}
	case AA_AST_NODE_TYPE::newstatement: {
		out = this->WriteToString(L"new %s", this->Unparse(pNode->expressions[0]));
		break;
	}
	case AA_AST_NODE_TYPE::classctorcall: {
		out = this->WriteToString(L"%s(%s)", pNode->content, this->UnparseList(pNode));
		break;
	}
	case AA_AST_NODE_TYPE::usingstatement: {
		out = this->WriteIndent() + this->WriteToString(L"using %s;\n", this->Unparse(pNode->expressions[0]));
		break;
	}
	case AA_AST_NODE_TYPE::usingspecificstatement: {
		out = this->WriteIndent() + this->WriteToString(L"using %s from %s;\n", pNode->content, pNode->expressions[0]->content);
		break;
	}
	case AA_AST_NODE_TYPE::matchstatement: {
		std::wstring on = this->Unparse(pNode->expressions[0]);
		std::wstring cases = this->Unparse(pNode->expressions[1]);
		out = this->WriteToString(L"%s match %s", on, cases);
		break;
	}
	case AA_AST_NODE_TYPE::matchcaselist: {
		std::wstring block = L"{\n";
		this->IncreaseIndent();
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			std::wstring _case = this->Unparse(pNode->expressions[i]);
			block += this->WriteIndent() + _case;
			if (i < pNode->expressions.size() - 1) {
				block[block.length() - 1] = ',';
				block += L"\n";
			}
		}
		this->DecreaseIndent();
		block += this->WriteIndent() + L"}";
		out = block;
		break;
	}
	case AA_AST_NODE_TYPE::matchcasestatement: {
		out = this->WriteToString(L"case %s => %s", this->Unparse(pNode->expressions[0]), this->Unparse(pNode->expressions[1]));
		break;
	}
	case AA_AST_NODE_TYPE::stringliteral:
		out = this->WriteToString(L"\"%s\"", pNode->content);
		break;
	case AA_AST_NODE_TYPE::charliteral: {
		std::wstring w = pNode->content;
		if (pNode->content == L"\n") {
			w = L"\\n";
		} else if (pNode->content == L"\t") {
			w = L"\\t";
		} else if (pNode->content == L"\r") {
			w = L"\\r";
		} else if (pNode->content == L"\\") {
			w = L"\\";
		}
		out = this->WriteToString(L"'%s'", w);
		break;
	}
	case AA_AST_NODE_TYPE::index: {
		out = this->WriteToString(L"%s[%s]", this->Unparse(pNode->expressions[0]), this->Unparse(pNode->expressions[1]));
		break;
	}
	case AA_AST_NODE_TYPE::tupleval: {
		out = this->WriteToString(L"%s", this->UnparseTuple(pNode));
		break;
	}
	case AA_AST_NODE_TYPE::tuplevardecl: {
		std::wstring ws = this->UnparseTuple(pNode);
		out = this->WriteToString(L"%s %s", ws, pNode->content);
		break;
	}
	case AA_AST_NODE_TYPE::field:
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

std::wstring AAUnparser::UnparseTuple(AA_AST_NODE* pNode) {

	std::wstringstream wss;
	wss << L"(";

	// Loop through all types
	for (size_t i = 0; i < pNode->expressions.size(); i++) {
		wss << this->Unparse(pNode->expressions[i]);
		if (i < pNode->expressions.size() - 1) {
			wss << ", ";
		}
	}

	wss << L")";

	return wss.str();

}

std::wstring AAUnparser::WriteIndent() {
	return this->WriteIndent(0);
}

std::wstring AAUnparser::WriteIndent(int offset) {
	if (offset < 0)
		offset = 0;
	return std::wstring(m_indent + offset, '\t');
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

void AAUnparser::AddSemicolon(std::wstring& ws) {
	if (ws[ws.length() - 1] == '\n') {
		ws[ws.length() - 1] = ';';
		ws += L";";
	} else {
		ws += L";";
	}
}
