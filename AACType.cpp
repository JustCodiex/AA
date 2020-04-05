#include "AACType.h"
#include "AACNamespace.h"
#include "AAClassSignature.h"

AACType* AACType::Void = new AACType(L"void");
AACType* AACType::Null = new AACType(L"null");
AACType* AACType::Any = new AACType(L"Any");
AACType* AACType::ErrorType = new AACType(L"ErrType");

AACType* AACTypeDef::Int32 = new AACType(L"int");
AACType* AACTypeDef::Bool = new AACType(L"bool");
AACType* AACTypeDef::Float32 = new AACType(L"float");
AACType* AACTypeDef::Char = new AACType(L"char");
AACType* AACTypeDef::String = 0;

AACType::AACType() {
	this->name = L"ErrType";
	this->isRefType = false;
	this->isArrayType = false;
	this->encapsulatedType = 0;
	this->classSignature = 0;
}

AACType::AACType(AAClassSignature* sig) {
	this->name = sig->name;
	this->isRefType = true;
	this->isArrayType = false;
	this->encapsulatedType = 0;
	this->classSignature = sig;
}

AACType::AACType(std::wstring name) {
	this->name = name;
	this->isRefType = false;
	this->isArrayType = false;
	this->encapsulatedType = 0;
	this->classSignature = 0;
}


std::wstring AACType::GetFullname() {
	if (this->classSignature && this->classSignature->domain) {
		return this->classSignature->domain->GetNamePath() + L"::" + this->name;
	} else {
		return this->name;
	}
}

AACType* AACType::AsArrayType() {
	AACType* t = new AACType(this->name + L"[]");
	t->isArrayType = true;
	t->encapsulatedType = this;
	t->isRefType = true;
	return t;
}

AACType* AACType::GetArrayType() {
	if (this->isArrayType && this->encapsulatedType) {
		return this->encapsulatedType;
	} else {
		return AACType::ErrorType;
	}
}