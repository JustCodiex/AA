#pragma once
#include "AAObject.h"

class AAArray : public AAObject {

public:

	AAArray(AAPrimitiveType type, int dimCount, int* dimLengths);

	void Release() override;

	const size_t& get_length() const;
	const size_t& get_length(const size_t dimension) const;

	const AAPrimitiveType get_type() const;

	void set_value(const int32_t& index, const size_t& dimension, const AAStackValue& val);

	const AAStackValue& get_value(const int32_t& index, const size_t& dimension);

private:

	AAPrimitiveType m_primitiveType;
	int m_dimensionCount;
	int* m_dimensionLengths;

};
