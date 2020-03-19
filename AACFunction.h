#pragma once
#include <stdarg.h>
#include <vector>

class AAVM;
struct AARuntimeEnvironment;

typedef void(*AACFunctionPtr)(AAVM*, AARuntimeEnvironment*);

struct AACSingleFunction {

	std::wstring name;
	std::wstring returnType;
	std::vector<AAFuncParam> params;
	AACFunctionPtr fPtr;

	AACSingleFunction(std::wstring name, AACFunctionPtr ptr, std::wstring rt, int n_args, ...) {
		this->name = name;
		this->fPtr = ptr;
		this->returnType = rt;
		va_list ap;
		va_start(ap, n_args);
		for (size_t n = 0; n < (size_t)n_args; n++) {
			params.push_back(va_arg(ap, AAFuncParam));
		}
		va_end(ap);
	}

};
