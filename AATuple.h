#pragma once
#include "AAVal.h"
#include "AAPrimitiveType.h"
#include "array.h"

/// <summary>
/// Represents a tuple of variable length and of variable types
/// </summary>
struct AATuple {
private:

	struct TupleValue {
		AAVal val;
		AAPrimitiveType type;
		TupleValue() { val = AAVal(); type = AAPrimitiveType::Undefined; }
		TupleValue(AAPrimitiveType type, AAVal v) {
			this->type = type;
			this->val = v;
		}
	};

public:

	AATuple(const aa::array<AAPrimitiveType>& types, const aa::array<AAVal>& values);

	/// <summary>
	/// Returns the size of the tuple
	/// </summary>
	/// <returns></returns>
	const int& Size() const;

	/// <summary>
	/// Retrieve the type at the AAVal location
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	const AAPrimitiveType& TypeAt(const size_t& index) const;

	/// <summary>
	/// Retrieve the AAVal at location
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	const AAVal ValueAt(const size_t& index) const;

	/// <summary>
	/// Get a formatted string representation of the tuple
	/// </summary>
	/// <returns></returns>
	const std::wstring ToString() const;

private:

	aa::array<TupleValue> tupleValues;

};
