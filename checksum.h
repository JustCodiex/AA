#pragma once
#include <string>

namespace aa {
	namespace algorithm {

		template<typename T>
		constexpr const T simple_checksum(const char* ws, const size_t fixedSize) {
			T sum = fixedSize;
			for (size_t i = 0; i < fixedSize; i++) {
				sum *= (T)(ws[i] ^ i) / ((i+1) * 2);
			}
			return sum | sizeof(T);
		}

	}
}
