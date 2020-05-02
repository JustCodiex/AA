#include "AAVarEnv.h"

void AAVarEnv::DeclareVariable(unsigned int identifier) {
	m_variables[identifier] = AAStackValue();
}

void AAVarEnv::SetVariable(unsigned int identifier, AAStackValue val) {
	m_variables[identifier] = val;
}

AAStackValue AAVarEnv::GetVariable(unsigned int identifier) {
	return m_variables[identifier];
}

AAVarEnv* AAVarEnv::CloneSelf() {

	AAVarEnv* cpy = new AAVarEnv;
	for (auto e : this->m_variables) {
		cpy->m_variables[e.first] = e.second;
	}

	return cpy;

}
