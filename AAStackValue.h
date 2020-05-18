#pragma once
#include "AATuple.h"
#include "AA_literals.h"
#include "AAIntrPtr.h"
#include "AAMemory.h"

class AAStackValue {

public:

	AAStackValue();

	AAStackValue(AAMemoryPtr ptr);
	AAStackValue(AATuple tuple);
	AAStackValue(AAIntPtr iPtr);

	AAStackValue(signed char b);
	AAStackValue(unsigned char b);
	AAStackValue(bool b);

	AAStackValue(uint16_t us);
	AAStackValue(int16_t s);

	AAStackValue(uint32_t us);
	AAStackValue(int32_t s);

	AAStackValue(uint64_t us);
	AAStackValue(int64_t s);

	AAStackValue(std::wstring ws);
	AAStackValue(wchar_t w);

	AAStackValue(float r32);
	AAStackValue(double r64);

	AAStackValue(AA_Literal lit);
	AAStackValue(AAPrimitiveType type, AAVal val);

	AAPrimitiveType get_type() const;

	std::wstring ToString();

	template<typename T>
	T to_cpp() {
		return m_val.Raw<T>();
	}

	bool is_string() const;

	bool is_nil() const;

	bool is_same_value(const AAStackValue& value) const;

	AAStackValue change_type(AAPrimitiveType type);

	AAVal as_val() const;

	static AAStackValue None;
	static AAStackValue Null;

private:

	AAVal m_val;
	AAPrimitiveType m_type;

};
