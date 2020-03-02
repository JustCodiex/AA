#pragma once
#include <string>

namespace aa{

	class bwalker {

	public:

		bwalker(unsigned char* bytes, unsigned long long len) {
			m_bytes = bytes;
			m_length = len;
			m_position = 0;
		}

		void operator>>(int& i) {
			memcpy(&i, m_bytes + m_position, 4);
			m_position += 4;
		}

		void operator>>(float& f) {
			memcpy(&f, m_bytes + m_position, 4);
			m_position += 4;
		}

		void operator>>(unsigned char& c) {
			memcpy(&c, m_bytes + m_position, 1);
			m_position ++;
		}

		void operator>>(bool& b) {
			memcpy(&b, m_bytes + m_position, 1);
			m_position++;
		}

	private:

		unsigned char* m_bytes;

		unsigned long long m_length; // currently not in use
		unsigned long long m_position;

	};

}
