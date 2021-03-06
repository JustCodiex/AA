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

		void operator <<(int16_t si) {
			unsigned char bytes[2];
			memcpy(bytes, &si, 2);
			for (int i = 0; i < 2; i++)
				m_buffer.push_back(bytes[i]);
		}

		void operator <<(int32_t si) {
			unsigned char bytes[4];
			memcpy(bytes, &si, 4);
			for (int i = 0; i < 4; i++)
				m_buffer.push_back(bytes[i]);
		}

		void operator <<(int64_t si) {
			unsigned char bytes[8];
			memcpy(bytes, &si, 8);
			for (int i = 0; i < 8; i++)
				m_buffer.push_back(bytes[i]);
		}

		void operator <<(uint16_t si) {
			unsigned char bytes[2];
			memcpy(bytes, &si, 2);
			for (int i = 0; i < 2; i++)
				m_buffer.push_back(bytes[i]);
		}

		void operator <<(uint32_t si) {
			unsigned char bytes[4];
			memcpy(bytes, &si, 4);
			for (int i = 0; i < 4; i++)
				m_buffer.push_back(bytes[i]);
		}

		void operator <<(uint64_t si) {
			unsigned char bytes[8];
			memcpy(bytes, &si, 8);
			for (int i = 0; i < 8; i++)
				m_buffer.push_back(bytes[i]);
		}

		void operator <<(wchar_t wc) {
			unsigned char bytes[2];
			memcpy(bytes, &wc, 2);
			for (int i = 0; i < 2; i++)
				m_buffer.push_back(bytes[i]);
		}

		void operator <<(wchar_t* wstr) {
			size_t len = wcslen(wstr) + 1;
			for (size_t i = 0; i < len; i++) {
				(*this) << wstr[i];
			}
		}

		void operator <<(float_t f) {
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

		void write_bytes(const size_t& byteCount, unsigned char* bytes) {
			for (size_t i = 0; i < byteCount; i++) {
				m_buffer.push_back(bytes[i]);
			}
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
