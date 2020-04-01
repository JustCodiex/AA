#include "AAStaticAnalysis.h"

void AAStaticAnalysis::Reset(std::vector<AAFuncSignature> funcs, std::vector<AAClassSignature> classes) {

	// Copy functions
	m_preregisteredFunctions.FromVector(funcs);

	// Copy classes
	m_preregisteredClasses.FromVector(classes);

}

AAC_CompileErrorMessage AAStaticAnalysis::RunStaticAnalysis(std::vector<AA_AST*> trees) {

	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::RunTypecheckAnalysis() {

	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::FetchStaticDeclerations(std::vector<AA_AST*> trees) {

	return NO_COMPILE_ERROR_MESSAGE;

}
