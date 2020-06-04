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

	namespace aastring {

		/// <summary>
		/// Check if a string s begins with the start string
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="s">String to check if begins with 'start'</param>
		/// <param name="start">String to see if 's' begins with</param>
		/// <returns>True if 's' begins with 'start'</returns>
		template<typename T>
		bool begins_with(T s, T start) {
			if (s.length() >= start.length()) {
				return start.compare(s.substr(0, start.length())) == 0;
			} else {
				return false;
			}
		}

		// NOT IMPLEMENTED
		template<typename T>
		bool ends_with(T s, T end) {
			return false;
		}

		/// <summary>
		/// Check if a string contains another string
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="s">String to check</param>
		/// <param name="contains">String to check if contained in 's'</param>
		/// <returns></returns>
		template<typename T>
		bool contains(T s, T contains) {
			return s.find(contains, 0) != T::npos;
		}

	}

}
