#include "AATuple.h"

AATuple::AATuple(const aa::array<AAPrimitiveType>& types, const aa::array<AAVal>& values) {
	tupleValues = aa::array<TupleValue>(types.length());
	for (size_t i = 0; i < types.length(); i++) {
		tupleValues[i] = TupleValue(types[i], values[i]);
	}
}

const int& AATuple::Size() const {
	return tupleValues.length();
}

const AAPrimitiveType& AATuple::TypeAt(const size_t& index) const {
	return tupleValues[index].type;
}

const AAVal AATuple::ValueAt(const size_t& index) const {
	return tupleValues[index].val;
}

const std::wstring AATuple::ToString() const {
	return L"()";
}
