#pragma once
#include "AAStackValue.h"
#include <string>
#include <map>

class AAVarEnv {

public:

	void DeclareVariable(unsigned int identifier);

	void SetVariable(unsigned int identifier, AAStackValue val);
	AAStackValue GetVariable(unsigned int identifier);

	AAVarEnv* CloneSelf();

private:

	std::map<unsigned int, AAStackValue> m_variables;

};
