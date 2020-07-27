#include "AADynamicTypeEnviornment.h"
#include "AATypeChecker.h"
#include "astring.h"

AADynamicTypeEnvironment::AADynamicTypeEnvironment() {

}

AACType* AADynamicTypeEnvironment::FindType(std::wstring format, TypeMapFunction typeMapper) {

	if (this->m_registeredTypes.find(format) != this->m_registeredTypes.end()) {
		return this->m_registeredTypes[format];
	}

	return AACType::ErrorType;

}

AACType* AADynamicTypeEnvironment::AddType(std::wstring format, TypeMapFunction typeMapper) {

	if (format[0] == '(') { // tuple or lambda
		if (aa::aastring::contains<std::wstring>(format, L"=>")) { // lambda
			AACType* returnType;
			aa::list<AACType*> paramTypes;
			this->UnpackLambda(format, returnType, paramTypes, typeMapper);
			this->m_registeredTypes[format] = AACType::LambdaType(format, returnType, paramTypes);
		} else { // tuple
			this->m_registeredTypes[format] = new AACType(format, this->UnpackTuple(format, typeMapper));
		}
		return this->m_registeredTypes[format];
	} else { // Arrays
		_ASSERT(false); // Not implemented
	}

	return AACType::ErrorType;

}

aa::list<AACType*> AADynamicTypeEnvironment::UnpackTuple(std::wstring format, TypeMapFunction typeMapper) {

	// TODO: Get regex on this

	aa::list<AACType*> types;

	format = format.substr(1);
	while (format.length() > 0) {
		size_t i = 0;
		if ((i = format.find_first_of(',')) == std::wstring::npos) { // nested tuples not allowed here...
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

void AADynamicTypeEnvironment::UnpackLambda(std::wstring format, AACType*& returntype, aa::list<AACType*>& params, TypeMapFunction typeMapper) {

	// TODO: Get regex on this

	format = format.substr(1);
	bool isRType = false;
	while (format.length() > 1) {

		size_t i = 0;
		if ((i = format.find_first_of(',')) == std::wstring::npos) {
			if ((i = format.find_first_of(L"=>")) == std::wstring::npos) {
				if ((i = format.find_first_of(')')) == std::wstring::npos) {
					break;
				}
			} else {
				if (i == 0) {
					format = format.substr(1);
					isRType = true;
					continue;
				}
			}
		}

		AACType* pType = typeMapper(format.substr(0, i));
		if (pType != AACType::ErrorType) {
			if (isRType) {
				returntype = pType;
			} else {
				params.Add(pType);
			}
		} else {
			wprintf(L"Unknown type found at lambda type argument, lambda = ('%s')", format.c_str());
			return;
		}
		format = format.substr(i + 1);

	}

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

	// Make sure we have content to clean (This is called even when empty)
	if (m_registeredTypes.size() > 0) {

		// For all registered types
		for (auto& typePair : m_registeredTypes) {
			
			// We delete this dynamic type ==> So it doesn't hang around
			delete typePair.second;

		}

		// Clear the type map
		m_registeredTypes.clear();

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

		std::wstring FormalizeLambda(aa::list<AACType*> params, AACType* out) {

			std::wstringstream wss;

			wss << L"(";

			for (size_t i = 0; i < params.Size(); i++) {
				wss << params.At(i)->GetFullname();
				if (i < params.Size() - 1) {
					wss << L",";
				}
			}

			wss << L"=>" << out->GetFullname() << L")";

			return wss.str();

		}

	}
}
