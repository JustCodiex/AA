#pragma once
#include <vector>
#include <string>

namespace aa {

	// A very light-weight implementation of a byte stream, were input may only be appended.
	class bstream {

	public:

		void operator <<(unsigned char byte) {
			m_buffer.push_back(byte);
		}

		void operator <<(int si) {
			unsigned char bytes[4];
			memcpy(bytes, &si, 4);
			for (int i = 0; i < 4; i++)
				m_buffer.push_back(bytes[i]);
		}

		void operator <<(float f) {
			unsigned char bytes[4];
			memcpy(bytes, &f, 4);
			for (int i = 0; i < 4; i++)
				m_buffer.push_back(bytes[i]);
		}

		void operator <<(std::wstring str) {
			size_t len = str.length() * 2;
			unsigned char* bytes = new unsigned char[len];
			memcpy(bytes, str.c_str(), len);
			for (size_t i = 0; i < len; i++)
				m_buffer.push_back(bytes[i]);
		}

		void operator <<(bool b) {
			unsigned char byte;
			memcpy(&byte, &b, 1);
			m_buffer.push_back(byte);
		}

		// Get length of the underlying buffer at the time of the call
		size_t length() {
			return m_buffer.size();
		}

		// Get the pointer to the buffer's internal storage
		unsigned char* array() {
			return &m_buffer[0];
		}

	private:

		std::vector<unsigned char> m_buffer;

	};

}
