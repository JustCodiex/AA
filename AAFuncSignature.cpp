#include "AAFuncSignature.h"
#include <sstream>

std::wstring AAFuncSignature::GetName() const {
	if (isClassMethod) {
		return this->name.substr(this->name.find_last_of(':') + 1);
	} else {
		return this->name;
	}
}

const std::wstring AAFuncSignature::GetFunctionalSignature(bool ignoreClass, bool ignoreName) const {

	// String stream
	std::wstringstream wss;

	// Write function name if not supposed to ignore
	if (!ignoreName) {
		wss << L"[" << ((ignoreClass) ? this->GetName() : this->name) << L"]-";
	}

	// Opener
	wss << L"(";

	// Start and ending offsets
	int offsetStart = 0;
	int offsetEnd = 0;

	// Determine offsets
	if (isClassCtor) {
		offsetEnd = 1;
	} else if (isClassMethod) {
		offsetStart = 1;
	}

	// Write out all parameters
	for (size_t i = offsetStart; i < this->parameters.size() - offsetEnd; i++) {
		wss << this->parameters[i].type->GetFullname();
		if (i < this->parameters.size() - 1) {
			wss << ",";
		}
	}

	// Ender
	wss << L")";

	return wss.str();

}

bool AAFuncSignature::EqualsParams(std::vector<AAFuncParam> params, bool ignoreFirst) {
	if (this->parameters.size() == params.size()) {
		for (size_t i = (ignoreFirst) ? 1 : 0; i < this->parameters.size(); i++) {
			if (this->parameters[i].type != params[i].type) {
				return false;
			}
		}
		return true;
	} else {
		return false;
	}
}

bool AAFuncSignature::IsValidOverride(AAFuncSignature* other) {
	if (this->isClassCtor || other->isClassCtor) { // cannot override constructors!
		return false;
	}
	if (this->storageModifier == AAStorageModifier::OVERRIDE) {
		if (this->GetName().compare(other->GetName()) == 0) {
			if (this->returnType == other->returnType && this->EqualsParams(other->parameters, other->isClassMethod && this->isClassMethod)) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool AAFuncSignature::operator==(AAFuncSignature other) const {
	if (this->name.compare(other.name) == 0 && this->domain == other.domain) {
		if (this->parameters.size() == other.parameters.size()) {
			for (size_t p = 0; p < this->parameters.size(); p++) {
				if (this->parameters[p] != other.parameters[p]) {
					return false;
				}
			}
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}
