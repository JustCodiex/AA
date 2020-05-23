#pragma once
#include "array.h"

namespace aa {

	/// <summary>
	/// Array consisting of finite length containing a pair of two values
	/// </summary>
	/// <typeparam name="U">First type</typeparam>
	/// <typeparam name="V">Second type</typeparam>
	template<typename U, typename V>
	class paired_array {

	public:

		struct pair {
			U first;
			V second;
			pair() {}
			pair(U f, V s) {
				first = f;
				second = s;
			}
		};

	public:

		paired_array(const size_t& size) {
			m_pairs = aa::array<pair>(size);
		}

		paired_array(const size_t& size, U first, V second) {
			m_pairs = aa::array<pair>(size);
			if (size > 0) {
				m_pairs[0] = pair(first, second);
			}
		}

		pair& operator[](const size_t& index) {
			return m_pairs[index];
		}

		/// <summary>
		/// Retrieve the length of the array (not the size) - the pair amount
		/// </summary>
		/// <returns></returns>
		const size_t length() const { return m_pairs.length(); }
		
		/// <summary>
		/// Retrieve the size of the array (not the length) - size in bytes
		/// </summary>
		/// <returns></returns>
		const size_t size() const { return m_pairs.length() * sizeof(U) + m_pairs.length() * sizeof(V); }

		/// <summary>
		/// Set some paired value
		/// </summary>
		/// <param name="index">Index of pair to set value</param>
		/// <param name="f">First value</param>
		/// <param name="s">Second value</param>
		void set_paired(const size_t& index, const U& f, const V& s) {
			m_pairs[index] = pair(f, s);
		}

		/// <summary>
		/// Set the first element of the paired value
		/// </summary>
		/// <param name="index">Index of pair to set value</param>
		/// <param name="f">First value</param>
		void set_first(const size_t& index, const U& f) {
			m_pairs[index].first = f;
		}

		/// <summary>
		/// Set the second element of the paired value
		/// </summary>
		/// <param name="index">Index of pair to set value</param>
		/// <param name="s">Second value</param>
		void set_second(const size_t& index, const V& s) {
			m_pairs[index].second = s;
		}

		/// <summary>
		/// Get the value of the pair at index
		/// </summary>
		/// <param name="index">Index of pair to set value</param>
		/// <param name="f">First value</param>
		/// <param name="s">Second value</param>
		void get_paired(const size_t& index, U& f, V& s) {
			f = m_pairs[index].first;
			s = m_pairs[index].second;
		}
		
		/// <summary>
		/// Get the first element of the paired value
		/// </summary>
		/// <param name="index">Index of pair to set value</param>
		const U get_first(const size_t& index) {
			return m_pairs[index].first;
		}

		/// <summary>
		/// Get the second element of the paired value
		/// </summary>
		/// <param name="index">Index of pair to set value</param>
		const V get_second(const size_t& index) {
			return m_pairs[index].second;
		}

		/// <summary>
		/// Fill two separate arrays with the paired values (Arrays are initialized inside function)
		/// </summary>
		/// <param name="f">Array for first pair element</param>
		/// <param name="s">Array for second pair element</param>
		void fill_two(aa::array<U>& f, aa::array<V>& s) const {
			f = aa::array<U>(m_pairs.length());
			s = aa::array<U>(m_pairs.length());
			for (size_t i = 0; i < m_pairs.length(); i++) {
				f[i] = m_pairs[i].first;
				s[i] = m_pairs[i].second;
			}
		}
		
		/// <summary>
		/// Run a non-mutating action on all elements in array
		/// </summary>
		/// <param name="action">Action to execute on each element</param>
		void for_each(std::function<void(pair)> action) const {
			m_pairs.for_each(action);
		}

		/// <summary>
		/// Run a mutating action on all elements in array
		/// </summary>
		/// <param name="action">Action to execute on each element</param>
		void for_each(std::function<void(pair&)> action) {
			m_pairs.for_each(action);
		}

	private:

		aa::array<pair> m_pairs;

	};

}
