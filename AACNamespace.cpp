#include "AACNamespace.h"

std::wstring AAClassSignature::GetFullname() {
	if (this->domain) {
		return this->domain->GetNamePath() + L"::" + this->name;
	} else {
		return this->name;
	}
}

std::wstring AAFuncSignature::GetFullname() {
	if (this->domain) {
		return this->domain->GetNamePath() + L"::" + this->name;
	} else {
		return this->name;
	}
}

std::wstring AACEnumSignature::GetFullname() {
	if (this->domain) {
		return this->domain->GetNamePath() + L"::" + this->name;
	} else {
		return this->name;
	}
}
