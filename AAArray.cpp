#include "AAArray.h"

AAArray::AAArray(AAPrimitiveType type, uint32_t dimCount, uint32_t* dimLengths) : AAObject(0) {

	// Save array type
	m_primitiveType = type;
	m_offsetSize = aa::runtime::size_of_type(type);

	// Get dimension data
	m_dimensionCount = dimCount;
	m_dimensionLengths = dimLengths;
	
	// Alloc dimensions
	m_content = new unsigned char*[dimCount];
	for (size_t i = 0; i < dimCount; i++) {

		// Alloc bytes
		m_content[i] = new unsigned char[dimLengths[i] * m_offsetSize];

		// Set to all zeros
		memset(m_content[i], 0, dimLengths[i]);

	}

}

void AAArray::Release() {

	// For all array dimensions
	for (size_t i = 0; i < m_dimensionCount; i++) {
		delete[] m_content[i];
	}

	// Delete dimensions
	delete[] m_content;

	// Delete dimension lengths array
	delete[] m_dimensionLengths;

	// Delete self
	delete this;

}

const uint32_t& AAArray::get_length() const {
	return m_dimensionLengths[0];
}

const uint32_t& AAArray::get_length(const uint32_t dimension) const {
	return m_dimensionLengths[dimension];
}

const AAPrimitiveType AAArray::get_type() const {
	return m_primitiveType;
}

void AAArray::set_value(const uint32_t& index, const uint32_t& dimension, const AAStackValue& val) {

	uint32_t i = index * m_offsetSize;
	AAVal v = val.as_val();

	memcpy(m_content[dimension] + i, v.get_bytes(), m_offsetSize);

}

const AAStackValue AAArray::get_value(const uint32_t& index, const uint32_t& dimension) {

	uint32_t i = index * m_offsetSize;
	unsigned char* bytes = new unsigned char[m_offsetSize];
	memcpy(bytes, m_content[dimension] + i, m_offsetSize);

	return AAStackValue(m_primitiveType, AAVal(bytes, m_offsetSize));

}
