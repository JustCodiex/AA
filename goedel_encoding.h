#pragma once
#include "array.h"
#include "list.h"
#include "set.h"

namespace aa {
	namespace algorithm {
		namespace encoding {

			/// <summary>
			/// Find the next prime AFTER the given number
			/// </summary>
			/// <param name="n">The next prime number following n. Can be ANY number</param>
			/// <returns>The next prime or -1 if out of pre-calculated range</returns>
			constexpr int next_prime(const int n);

			namespace goedel {

				/// <summary>
				/// The number used by AA for encoding primitive types
				/// </summary>
				constexpr int GOEDEL_AA_PRIMITIVE_ENCODING = 19;

				/// <summary>
				/// Gödel number representation of an encoded array
				/// </summary>
				struct goedel_number {
					
					unsigned char len; // encoded array length
					long long n; // actual number (64-bit number)
					
					/// <summary>
					/// Array of length 0
					/// </summary>
					goedel_number();

					/// <summary>
					/// Encoded array of specified length and with precalculated number
					/// </summary>
					/// <param name="ln">Length of the encoded array</param>
					/// <param name="n">The encoded number</param>
					goedel_number(const unsigned char ln, long long n);
					
					/// <summary>
					/// Decode the array value at index
					/// </summary>
					/// <param name="index">The index to fetch number from</param>
					/// <param name="power">The power used to calculate the number</param>
					/// <returns></returns>
					int decode(const int index, const int power);

				};

				/// <summary>
				/// Encode a C integer array into a gödel number with p=GOEDEL_AA_PRIMITIVE_ENCODING
				/// </summary>
				/// <param name="length">The length of the array</param>
				/// <param name="array">The array of integers to encode</param>
				/// <returns></returns>
				goedel_number encode(const int& length, const int* array);

				/// <summary>
				/// Encode a C integer array into a gödel number
				/// </summary>
				/// <param name="p">The power to use when encoding</param>
				/// <param name="length">The length of the array</param>
				/// <param name="array">The array of integers to encode</param>
				/// <returns></returns>
				goedel_number encode(const int p, const int& length, const int* array);

				/// <summary>
				/// Encode an integer array into a gödel number with p=GOEDEL_AA_PRIMITIVE_ENCODING
				/// </summary>
				/// <param name="array">The array of integers to encode</param>
				/// <returns></returns>
				goedel_number encode(aa::array<int> array);

				/// <summary>
				/// Encode an integer array into a gödel number
				/// </summary>
				/// <param name="p">The power to use when encoding</param>
				/// <param name="array">The array of integers to encode</param>
				/// <returns></returns>
				goedel_number encode(const int p, aa::array<int> array);

				/// <summary>
				/// Encode an integer list into a gödel number with p=GOEDEL_AA_PRIMITIVE_ENCODING
				/// </summary>
				/// <param name="ls">The list of integers to encode</param>
				/// <returns></returns>
				goedel_number encode(aa::list<int> ls);

				/// <summary>
				/// Encode an integer list into a gödel number
				/// </summary>
				/// <param name="p">The power to use when encoding</param>
				/// <param name="ls">The list of integers to encode</param>
				/// <returns></returns>
				goedel_number encode(const int p, aa::list<int> ls);

				/// <summary>
				/// Encode an integer set into a gödel number with p=GOEDEL_AA_PRIMITIVE_ENCODING
				/// </summary>
				/// <param name="set">The set of integers to encode</param>
				/// <returns></returns>
				goedel_number encode(aa::set<int> set);

				/// <summary>
				/// Encode an integer set into a gödel number
				/// </summary>
				/// <param name="p">The power to use when encoding</param>
				/// <param name="set">The set of integers to encode</param>
				/// <returns></returns>
				goedel_number encode(const int p, aa::set<int> set);

				/// <summary>
				/// Decode a Gödel number into the number found at the encoded array position with p=GOEDEL_AA_PRIMITIVE_ENCODING
				/// </summary>
				/// <param name="gn">Encoded Gödel number</param>
				/// <param name="index">The index to fetch number from</param>
				/// <returns></returns>
				int decode(const goedel_number gn, const int index);

				/// <summary>
				/// Decode a Gödel number into the number found at the encoded array position
				/// </summary>
				/// <param name="gn">Encoded Gödel number</param>
				/// <param name="index">The index to fetch number from</param>
				/// <param name="power">The power used to encode</param>
				/// <returns></returns>
				int decode(const goedel_number gn, const int index, const int power);

			}
		}
	}
}
