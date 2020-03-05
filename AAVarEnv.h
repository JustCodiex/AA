#pragma once
#include "AAVal.h"
#include <string>
#include <map>

class AAVarEnv {

public:

	void DeclareVariable(unsigned int identifier);

	void SetVariable(unsigned int identifier, AAVal);
	AAVal GetVariable(unsigned int identifier);

	AAVarEnv* CloneSelf();

private:

	std::map<unsigned int, AAVal> m_variables;

};
