#include "AAUnparser.h"
#include <stdarg.h>

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
		this->Unparse(pTree->GetRoot());
	}
}

void AAUnparser::Unparse(AA_AST_NODE* pNode) {

	switch (pNode->type) {

	default:
		wprintf(L"Failed to unparse nodetype: %i\n", (int)pNode->type);
		break;
	}

}

void AAUnparser::WriteLine(std::wstring ln, ...) {

	m_stream << std::wstring('\t', m_indent);

	va_list args;
	va_start(args, ln);

	for (size_t i = 0; i < ln.length(); i++) {

		if (i + 1 < ln.length() && ln[i] == '€') {
			if (ln[i + 1] == 's') {
				m_stream << va_arg(args, const wchar_t*);
				i++;
			} else if (ln[i + 1] == 'i') {
				m_stream << va_arg(args, int);
				i++;
			} else if (ln[i + 1] == 'f') {
				m_stream << va_arg(args, float);
				i++;
			} else if (ln[i + 1] == 'd') {
				m_stream << va_arg(args, double);
				i++;
			}
		} else {
			m_stream << ln[i];
		}

	}

	va_end(args);

	m_stream << '\n';

}

void AAUnparser::IncreaseIndent() {
	m_indent++;
}
void AAUnparser::DecreaseIndent() {
	m_indent--;
}
