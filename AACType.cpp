#include "AACType.h"
#include "AACNamespace.h"
#include "AACEnum.h"
#include "AAClassSignature.h"

AACType* AACType::Void = new AACType(L"void");
AACType* AACType::Null = new AACType(L"null");
AACType* AACType::Any = new AACType(L"Any");
AACType* AACType::ErrorType = new AACType(L"ErrType");
AACType* AACType::ExportReferenceType = new AACType(L"ExportReferenceType");

AACType* AACTypeDef::Int16 = new AACType(L"short");
AACType* AACTypeDef::Int32 = new AACType(L"int");
AACType* AACTypeDef::Int64 = new AACType(L"long");
AACType* AACTypeDef::UInt16 = new AACType(L"ushort");
AACType* AACTypeDef::UInt32 = new AACType(L"uint");
AACType* AACTypeDef::UInt64 = new AACType(L"ulong");
AACType* AACTypeDef::Bool = new AACType(L"bool");
AACType* AACTypeDef::Byte = new AACType(L"byte");
AACType* AACTypeDef::SByte = new AACType(L"sbyte");
AACType* AACTypeDef::Float32 = new AACType(L"float");
AACType* AACTypeDef::Float64 = new AACType(L"double");
AACType* AACTypeDef::Char = new AACType(L"char");
AACType* AACTypeDef::IntPtr = new AACType(L"intptr");
AACType* AACTypeDef::String = 0;

AACType::AACType() {
	this->name = L"ErrType";
	this->isRefType = false;
	this->isArrayType = false;
	this->isEnum = false;
	this->isTupleType = false;
	this->isVMType = false;
	this->isLambdaType = false;
	this->classSignature = 0;
	this->enumSignature = 0;
	this->constantID = 0;
}

AACType::AACType(AAClassSignature* sig) {
	this->name = sig->name;
	this->isRefType = true;
	this->isArrayType = false;
	this->isEnum = false;
	this->isTupleType = false;
	this->isVMType = false;
	this->isLambdaType = false;
	this->classSignature = sig;
	this->enumSignature = 0;
	this->constantID = 0;
}

AACType::AACType(std::wstring name) {
	this->name = name;
	this->isRefType = false;
	this->isArrayType = false;
	this->isEnum = false;
	this->isTupleType = false;
	this->isVMType = false;
	this->isLambdaType = false;
	this->classSignature = 0;
	this->enumSignature = 0;
	this->constantID = 0;
}

AACType::AACType(AACEnumSignature* enumSignature) {
	this->name = enumSignature->name;
	this->isRefType = false;
	this->isArrayType = false;
	this->isEnum = true;
	this->isTupleType = false;
	this->isVMType = false;
	this->isLambdaType = false;
	this->classSignature = 0;
	this->enumSignature = enumSignature;
	this->constantID = 0;
}

AACType::AACType(std::wstring name, aa::list<AACType*> tupleSignature) {
	this->name = name;
	this->isRefType = false;
	this->isArrayType = false;
	this->isEnum = false;
	this->isTupleType = true;
	this->isVMType = false;
	this->isLambdaType = false;
	this->classSignature = 0;
	this->enumSignature = 0;
	this->constantID = 0;
	this->encapsulatedTypes = tupleSignature;
}

AACType* AACType::LambdaType(std::wstring name, AACType* returnType, aa::list<AACType*> params) {
	AACType* type = new AACType();
	type->name = name;
	type->isRefType = false;
	type->isArrayType = false;
	type->isEnum = false;
	type->isTupleType = false;
	type->isVMType = false;
	type->classSignature = 0;
	type->enumSignature = 0;
	type->isLambdaType = true;
	type->constantID = 0;
	type->encapsulatedTypes = aa::list<AACType*>();
	type->encapsulatedTypes.Add(params);
	type->encapsulatedTypes.Add(returnType);
	return type;
}

std::wstring AACType::GetFullname() {
	if (this->classSignature && this->classSignature->domain && !this->classSignature->domain->IsGlobalSpace()) {
		return this->classSignature->domain->GetNamePath() + L"::" + this->name;
	} else {
		return this->name;
	}
}

AACType* AACType::AsArrayType() {
	AACType* t = new AACType(this->name + L"[]");
	t->isArrayType = true;
	t->encapsulatedTypes.Add(this);
	t->isRefType = true;
	t->isEnum = false;
	return t;
}

AACType* AACType::GetArrayType() {
	AACType* encapped = this->GetEncapsulatedType();
	if (encapped && this->isArrayType) {
		return encapped;
	} else {
		return AACType::ErrorType;
	}
}

AACType* AACType::GetEncapsulatedType() {
	if (this->encapsulatedTypes.Size() > 0) {
		return this->encapsulatedTypes.First();
	} else {
		return AACType::ErrorType;
	}
}

AACType* AACType::GetEncapsulatedType(size_t index) {
	if (index >= 0 && index < this->encapsulatedTypes.Size()) {
		return this->encapsulatedTypes.At(index);
	} else {
		return AACType::ErrorType;
	}
}
