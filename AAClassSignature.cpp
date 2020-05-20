#include "AAClassSignature.h"

void AAClassSignature::CreateVTable() {
	this->classVTable = new AAClassVirtualTable(this);
}

bool AAClassSignature::DerivesFrom(AAClassSignature* other) {
	if (this->extends.Contains([other](AAClassSignature*& sig) { return sig == other || sig->DerivesFrom(other); })) {
		return true;
	} else {
		return false;
	}
}

AAFuncSignature* AAClassSignature::FindMethodFromFunctionalSignature(std::wstring functionalSignature) {

	int index;
	if (this->methods.FindFirstIndex([functionalSignature](AAFuncSignature*& sig) { return functionalSignature.compare(sig->GetFunctionalSignature(true)) == 0; }, index)) {
		return this->methods.Apply(index);
	}

	return NULL;

}

bool AAClassVirtualTable::AddVirtualFunction(AAFuncSignature* pSignature) {
	if (aa::modifiers::ContainsFlag(pSignature->storageModifier, AAStorageModifier::VIRTUAL)) {
		std::wstring func = pSignature->GetFunctionalSignature(true);
		virtualFunctions[func] = aa::list<std::pair<AAClassSignature*, AAFuncSignature*>>();
		virtualFunctions[func].Add(std::pair<AAClassSignature*, AAFuncSignature*>(virtualTableClass, pSignature));
		return true;
	} else {
		return false;
	}
}

bool AAClassVirtualTable::RegisterFunction(AAClassSignature* pMethodOwner, AAFuncSignature* pMethodSignature) {
	if (aa::modifiers::ContainsFlag(pMethodSignature->storageModifier, AAStorageModifier::OVERRIDE)) {
		std::wstring func = pMethodSignature->GetFunctionalSignature(true);
		auto ptr = virtualFunctions.find(func);
		if (ptr != virtualFunctions.end()) {
			(*ptr).second.Add(std::pair<AAClassSignature*, AAFuncSignature*>(pMethodOwner, pMethodSignature));
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}
