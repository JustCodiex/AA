#include "AAVarEnv.h"

void AAVarEnv::DeclareVariable(unsigned int identifier) {

	AAVal v = AAVal();
	v.litVal.lit.i = 0;
	m_variables[identifier] = v;

}

void AAVarEnv::SetVariable(unsigned int identifier, AAVal val) {
	m_variables[identifier] = val;
}

AAVal AAVarEnv::GetVariable(unsigned int identifier) {
	return m_variables[identifier];
}

AAVarEnv* AAVarEnv::CloneSelf() {

	AAVarEnv* cpy = new AAVarEnv;
	for (auto e : this->m_variables) {
		cpy->m_variables[e.first] = e.second;
	}

	return cpy;

}
