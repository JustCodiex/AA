#pragma once
#include <string>
#include <sstream>

namespace aa{

	class bwalker {

	public:

		bwalker(unsigned char* bytes, unsigned long long len) {
			m_bytes = bytes;
			m_length = len;
			m_position = 0;
		}

		void operator>>(int32_t& i) {
			memcpy(&i, m_bytes + m_position, 4);
			m_position += 4;
		}

		void operator>>(uint32_t& i) {
			memcpy(&i, m_bytes + m_position, 4);
			m_position += 4;
		}

		void operator>>(int16_t& i) {
			memcpy(&i, m_bytes + m_position, 2);
			m_position += 2;
		}

		void operator>>(uint16_t& i) {
			memcpy(&i, m_bytes + m_position, 2);
			m_position += 2;
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

		void operator>>(wchar_t& c) {
			memcpy(&c, m_bytes + m_position, 2);
			m_position += 2;
		}

		void operator>>(std::wstring& ws) {
			int len;
			this->operator>>(len);
			unsigned char* bytes = new unsigned char[len * 2];
			memset(bytes, '\0', len * 2);
			memcpy(bytes, m_bytes + m_position, len * 2);
			m_position += len * 2ULL;
			wchar_t* ubytes = new wchar_t[len + 1];
			wmemset(ubytes, '\0', len + 1);
			memcpy(ubytes, bytes, len * 2);
			ws = std::wstring(ubytes);
		}

		void read_bytes(unsigned char* bytes, const size_t& amount) {
			for (size_t i = 0; i < amount; i++) {
				memcpy(bytes + i, m_bytes + m_position, 1);
				m_position++;
			}
		}

		std::wstring read_cstring() {
			std::wstringstream wif;
			while (m_position < m_length) {
				wchar_t _wchar;
				memcpy(&_wchar, m_bytes + m_position, sizeof(wchar_t));
				m_position += 2;
				wif << _wchar;
				if (_wchar == L'\0') {
					break;
				}
			}
			return wif.str();
		}

	private:

		unsigned char* m_bytes;

		unsigned long long m_length; // currently not in use
		unsigned long long m_position;

	};

}
