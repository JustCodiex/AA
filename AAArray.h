#pragma once
#include "AAObject.h"

class AAArray : public AAObject {

public:

	AAArray(AAPrimitiveType type, uint32_t dimCount, uint32_t* dimLengths);

	void Release() override;

	const uint32_t& get_length() const;
	const uint32_t& get_length(const uint32_t dimension) const;

	const AAPrimitiveType get_type() const;

	void set_value(const uint32_t& index, const uint32_t& dimension, const AAStackValue& val);

	const AAStackValue get_value(const uint32_t& index, const uint32_t& dimension);

private:

	AAPrimitiveType m_primitiveType;
	uint32_t m_offsetSize;

	uint32_t m_dimensionCount;
	uint32_t* m_dimensionLengths;

	unsigned char** m_content;

};
