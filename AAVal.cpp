#include "AAVal.h"

void AAVal::Free() {
	if (m_data) {
		delete[] m_data;
	}
	m_data = 0;
}

const bool AAVal::is_valid() const {
	return m_data != 0;
}
