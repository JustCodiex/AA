#pragma once
#include <vector>

namespace aa {

	class bstream {

	public:

		void operator <<(unsigned char byte) {
			m_buffer.push_back(byte);
		}

		void operator <<(int i) {
			unsigned char bytes[4];
			memcpy(bytes, &i, 4);
			for (int i = 0; i < 4; i++)
				m_buffer.push_back(bytes[i]);
		}

		size_t length() {
			return m_buffer.size();
		}

		unsigned char* array() {
			return &m_buffer[0];
		}

	private:

		std::vector<unsigned char> m_buffer;

	};

}
