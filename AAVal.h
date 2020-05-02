#pragma once
#include <cstring>
#include <string>

struct AAVal {
	
public:

	AAVal() {
		m_data = 0;
	}

	template<typename T>
	AAVal(T val) {
		m_data = new unsigned char[sizeof(T)];
		memcpy(m_data, &val, sizeof(T));
	}

	template<>
	AAVal(std::wstring ws) {
		m_data = new unsigned char[sizeof(wchar_t) * (ws.length() + 1)];
		memset(m_data, '\0', sizeof(wchar_t) * (ws.length()+1));
		memcpy(m_data, &ws[0], sizeof(wchar_t) * ws.length());
	}

	AAVal(const unsigned char* dat, size_t sz) {
		m_data = new unsigned char[sz];
		memcpy(m_data, dat, sz);
	}

	void Free();

	template<typename T>
	T Raw() {
		return *reinterpret_cast<T*>(m_data);
	}

	template<>
	std::wstring Raw() {
		return std::wstring(reinterpret_cast<const wchar_t*>(m_data));
	}

	const bool Equals(const AAVal& other, const size_t& cmpsz) const {
		return memcmp(this->m_data, other.m_data, cmpsz) == 0;
	}

	const bool is_valid() const;

	template<typename T>
	const T ToValue() const {
		return *reinterpret_cast<T*>(m_data);
	}

	unsigned char* get_bytes() { return m_data; }

private:
	unsigned char* m_data;

};
