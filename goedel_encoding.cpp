#include "goedel_encoding.h"
#include <cmath>

namespace aa {
	namespace algorithm {
		namespace encoding {

			constexpr int primes[] = {
				7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53,
				59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107,
				109, 113, 127, 131, 137, 139, 149, 151, 157, 163,
				167, 173, 179, 181, 191, 193, 197, 199, 211, 223
			};

			constexpr int PC = sizeof(primes) / sizeof(primes[0]);

			constexpr int next_prime(const int n) {
				if (n < primes[0]) {
					return primes[0];
				} else if (n > primes[PC - 1]) {
					return -1;
				} else {
					int i = PC / 2;
					while (i >= 0 && i < PC) {
						if (n < primes[i]) {
							i--;
							if (i - 1 >= 0 && n > primes[i + 1]) {
								break;
							}
						} else if (n > primes[i]) {
							i++;
							if (i + 1 < PC && n < primes[i + 1]) {
								break;
							}
						} else {
							break;
						}
					}
					return primes[i];
				}
			}

			/*
			
			* TODO: Add exceptions (so attempts to encode 'numbers > power' will result in errors) 
			* TODO: Allow negative numbers (when index > 0, subtract one for negative numbers)
			**			Will need more work (mixing negative and positive numbers wont work)

			*/

			namespace goedel {

				goedel_number::goedel_number() {
					this->len = 0;
					this->n = 0;
				}

				goedel_number::goedel_number(const unsigned char ln, long long n) {
					this->len = ln;
					this->n = n;
				}

				int goedel_number::decode(const int index, const int power) {
					return goedel::decode(*this, index, power);
				}

				goedel_number encode(const int& length, const int* array) {
					return encode(GOEDEL_AA_PRIMITIVE_ENCODING, length, array);
				}

				goedel_number encode(const int p, const int& length, const int* array) {
					goedel_number gn;
					gn.len = (unsigned char)length;
					long long n = 0;
					for (int i = 0; i < length; i++) {
						n += (long long)(array[i] * (long long)std::pow(p, i));
					}
					gn.n = n;
					return gn;
				}

				goedel_number encode(aa::array<int> array) {
					return encode(GOEDEL_AA_PRIMITIVE_ENCODING, array);
				}

				goedel_number encode(const int p, aa::array<int> array) {
					goedel_number gn;
					gn.len = (unsigned char)array.length();
					long long n = 0;
					for (int i = 0; i < gn.len; i++) {
						n += (long long)(array[i] * (long long)std::pow(p, i));
					}
					gn.n = n;
					return gn;
				}

				goedel_number encode(aa::list<int> ls) {
					return encode(GOEDEL_AA_PRIMITIVE_ENCODING, ls.Size());
				}

				goedel_number encode(const int p, aa::list<int> ls) {
					goedel_number gn;
					gn.len = (unsigned char)ls.Size();
					long long n = 0;
					for (int i = 0; i < gn.len; i++) {
						n += (long long)(ls.At(i) * (long long)std::pow(p, i));
					}
					gn.n = n;
					return gn;
				}

				goedel_number encode(aa::set<int> set) {
					return encode(GOEDEL_AA_PRIMITIVE_ENCODING, set.Size());
				}

				goedel_number encode(const int p, aa::set<int> set) {
					goedel_number gn;
					gn.len = (unsigned char)set.Size();
					long long n = 0;
					for (int i = 0; i < gn.len; i++) {
						n += (long long)(set.Apply(i) * (long long)std::pow(p, i));
					}
					gn.n = n;
					return gn;
				}

				int decode(const goedel_number gn, const int index) {
					return (gn.n / (int)std::pow(GOEDEL_AA_PRIMITIVE_ENCODING, index)) % GOEDEL_AA_PRIMITIVE_ENCODING;
				}

				int decode(const goedel_number gn, const int index, const int power) {
					return (gn.n / (int)std::pow(power, index)) % power;
				}

			}

		}

	}

}
