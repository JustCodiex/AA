#pragma once
#include "AAVal.h"
#include "AAPrimitiveType.h"
#include "pairedarray.h"

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

	AATuple(const aa::paired_array<AAPrimitiveType, AAVal>& tuple);

	/// <summary>
	/// Returns the size of the tuple
	/// </summary>
	/// <returns></returns>
	const int Size() const;

	/// <summary>
	/// Retrieve the type at the AAVal location
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	const AAPrimitiveType TypeAt(const size_t& index) const;

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

	/// <summary>
	/// Checks if the first parameter is matching (not neccessarily equal) to the second parameter
	/// </summary>
	/// <param name="matchon">Tuple to see if matching</param>
	/// <param name="matchwith">Tuple to try and match with</param>
	/// <returns>True if the first parameter can match with the other</returns>
	static bool MatchTuple(AATuple matchon, AATuple matchwith);

private:

	aa::array<TupleValue> tupleValues;

};
