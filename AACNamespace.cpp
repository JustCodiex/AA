#include "AACNamespace.h"

std::wstring AAClassSignature::GetFullname() {
	return this->domain->GetNamePath() + L"::" + this->name;
}

std::wstring AAFuncSignature::GetFullname() {
	return this->domain->GetNamePath() + L"::" + this->name;
}
