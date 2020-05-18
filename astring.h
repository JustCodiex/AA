#pragma once
#include <string>
#include <sstream>
#include <iomanip>

std::string string_cast(const wchar_t* pSource);
std::string string_cast(std::wstring sSource);

namespace aa {

	template<typename T>
	std::wstring wstring_hex(T n) {
		std::wstringstream wss;
		wss << L"0x";
		wss.fill('0');
		wss.width(sizeof(T) * 2);
		wss << std::hex << n;
		return wss.str();
	}

	template<typename T>
	std::string string_hex(T n) {
		std::stringstream s;
		s << L"0x";
		s.fill('0');
		s.width(sizeof(T) * 2);
		s << std::hex << n;
		return s.str();
	}

	unsigned int wstring_trail(std::wstring ws);

	unsigned int string_trail(std::string str);

}
