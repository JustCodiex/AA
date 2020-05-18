#include "AADynamicTypeEnviornment.h"
#include "AATypeChecker.h"

AADynamicTypeEnvironment::AADynamicTypeEnvironment() {

}

AACType* AADynamicTypeEnvironment::FindType(std::wstring format, TypeMapFunction typeMapper) {

	if (this->m_registeredTypes.find(format) != this->m_registeredTypes.end()) {
		return this->m_registeredTypes[format];
	}

	return AACType::ErrorType;

}

AACType* AADynamicTypeEnvironment::AddType(std::wstring format, TypeMapFunction typeMapper) {

	if (format[0] == '(') { // tuple
		this->m_registeredTypes[format] = new AACType(format, this->UnpackTuple(format, typeMapper));
		return this->m_registeredTypes[format];
	} else { // Arrays
		_ASSERT(false); // Not implemented
	}

	return AACType::ErrorType;

}

aa::list<AACType*> AADynamicTypeEnvironment::UnpackTuple(std::wstring format, TypeMapFunction typeMapper) {

	aa::list<AACType*> types;

	format = format.substr(1);
	while (format.length() > 0) {
		size_t i = 0;
		if ((i = format.find_first_of(',')) == std::wstring::npos) {
			if ((i = format.find_first_of(')')) == std::wstring::npos) {
				break;
			}
		}
		AACType* pType = typeMapper(format.substr(0, i));
		if (pType != AACType::ErrorType) {
			types.Add(pType);
		} else {
			wprintf(L"Unknown type found at tuple type argument %i, tuple = ('%s')", ((int)types.Size() + 1), format.c_str());
			return aa::list<AACType*>();
		}
		format = format.substr(i + 1);
	}

	return types;

}

AACType* AADynamicTypeEnvironment::FindOrAddTypeIfNotFound(std::wstring format, TypeMapFunction typeMapper) {

	if (this->m_registeredTypes.find(format) == this->m_registeredTypes.end()) {
		return this->AddType(format, typeMapper);
	} else {
		return this->m_registeredTypes[format];
	}

}

void AADynamicTypeEnvironment::AddTypeIfNotFound(std::wstring format, TypeMapFunction typeMapper) {

	if (this->m_registeredTypes.find(format) == this->m_registeredTypes.end()) {
		if (format[0] == '(') { // tuple
			this->m_registeredTypes[format] = new AACType(format, this->UnpackTuple(format, typeMapper));
		} else { // Arrays
			_ASSERT(false); // Not implemented
		}
	}

}

bool AADynamicTypeEnvironment::IsValidDynamicType(AACType* pType) {

	// Check if tuple
	if (pType->isTupleType) {

		// Run through all types registered - if any matches, it's valid
		for (auto& typePairs : this->m_registeredTypes) {
			if (typePairs.second == pType) {
				return true;
			}
		}

	} else if (pType->isArrayType) {
		// TODO: Implement
	}

	return false;

}

void AADynamicTypeEnvironment::WipeClean() {

	if (m_registeredTypes.size() > 0) {

		printf("");

	}

}

void AADynamicTypeEnvironment::Release() {

	// Clean self
	this->WipeClean();

	// Then delete ourselves
	delete this;

}

namespace aa {
	namespace type {

		std::wstring FormalizeTuple(AA_AST_NODE* pTupleNode, TypeMapFunction typenameFunction) {

			// String to build
			std::wstringstream wss;

			// Add opener
			wss << L"(";

			// Loop through all types
			for (size_t i = 0; i < pTupleNode->expressions.size(); i++) {
				wss << typenameFunction(pTupleNode->expressions[i]->content)->GetFullname();
				if (i < pTupleNode->expressions.size() - 1) {
					wss << ",";
				}
			}

			// Add ender
			wss << L")";

			// Return the string
			return wss.str();

		}

		std::wstring FormalizeTuple(aa::list<AACType*> types) {

			// String to build
			std::wstringstream wss;

			// Add opener
			wss << L"(";

			// Loop through all types
			for (size_t i = 0; i < types.Size(); i++) {
				wss << types.At(i)->GetFullname();
				if (i < types.Size() - 1) {
					wss << ",";
				}
			}

			// Add ender
			wss << L")";

			// Return the string
			return wss.str();

		}


	}
}
