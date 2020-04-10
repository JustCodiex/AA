#pragma once
#include "AAVal.h"
#include "stack.h"
#include "list.h"
#include "AAFuncSignature.h"
#include <stdarg.h>
#include <vector>

class AAVM;
struct AARuntimeEnvironment;

typedef void(*AACFunctionPtr)(AAVM*, aa::stack<AAVal>, aa::stack<AAVal>&);

struct AACSingleFunction {

	std::wstring name;
	AACType* returnType;
	std::vector<AAFuncParam> params;
	AACFunctionPtr fPtr;
	AACSingleFunction() {
		this->name = L"";
		this->returnType = AACType::ErrorType;
		this->fPtr = 0;
	}
	AACSingleFunction(std::wstring name, AACFunctionPtr ptr, AACType* rt, int n_args, ...) {
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

	bool equals(AACSingleFunction other) {
		return this->fPtr == other.fPtr;
	}

	bool operator==(AACSingleFunction other) {
		return other.equals(*this);
	}

};

namespace aa {

	list<AAVal> ToArgumentList(stack<AAVal> args);

}
