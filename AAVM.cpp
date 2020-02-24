#include "AAVM.h"


AAVM::AAVM() {

	m_compiler = new AAC;
	m_parser = new AAP;

}

void AAVM::Release() {

	// Delete compiler
	delete m_compiler;
	m_compiler = 0;

	// Delete parser
	m_parser->Release();
	delete m_parser;
	m_parser = 0;

}

AAVM* AANewVM() {

	AAVM* vm = new AAVM;

	return vm;

}
