#include "AAArray.h"

AAArray::AAArray(AAPrimitiveType type, int dimCount, int* dimLengths) : AAObject(0) {

	m_primitiveType = type;
	m_dimensionCount = dimCount;
	m_dimensionLengths = dimLengths;

}

void AAArray::Release() {

	delete[] m_dimensionLengths;

}

const size_t& AAArray::get_length() const {
	return m_dimensionLengths[0];
}

const size_t& AAArray::get_length(const size_t dimension) const {
	return m_dimensionLengths[dimension];
}

const AAPrimitiveType AAArray::get_type() const {
	return m_primitiveType;
}

void AAArray::set_value(const int32_t& index, const size_t& dimension, const AAStackValue& val) {



}

const AAStackValue& AAArray::get_value(const int32_t& index, const size_t& dimension) {

	AAStackValue v;

	return v;

}
