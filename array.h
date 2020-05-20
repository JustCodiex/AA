#pragma once
#include <functional>

namespace aa {

	/// <summary>
	/// A simple implementation of an array
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T>
	class array {

	public:

		array() {
			m_length = 0;
			m_content = 0;
		}

		array(const size_t& size) {
			m_length = size;
			m_content = new T[m_length];
		}

		/// <summary>
		/// Get the length of the array
		/// </summary>
		/// <returns>Length of array</returns>
		const size_t length() const {
			return m_length;
		}

		T& operator[](const size_t& index) {
			return m_content[index];
		}

		const T operator[](const size_t& index) const {
			return m_content[index];
		}

		/// <summary>
		/// Map all elementss of array into a new array.
		/// </summary>
		/// <typeparam name="U">Other type</typeparam>
		/// <param name="mapFunction">Mapping function</param>
		/// <returns></returns>
		template<typename U>
		array<U> map(std::function<U(T)> mapFunction) const {
			array<U> u = array<U>(m_length);
			for (size_t i = 0; i < m_length; i++) {
				u[i] = mapFunction(m_content[i]);
			}
			return u;
		}

		/// <summary>
		/// Perform action with all elements of array as input (Use for loop - no mutation)
		/// </summary>
		/// <typeparam name="T"></typeparam>
		void for_each(std::function<void(T)> action) const {
			for (size_t i = 0; i < m_length; i++) {
				action(m_content[i]);
			}
		}

		/// <summary>
		/// Perform action on all elements of array (Use for mutation)
		/// </summary>
		/// <typeparam name="T"></typeparam>
		void for_each(std::function<void(T&)> action)  {
			for (size_t i = 0; i < m_length; i++) {
				action(m_content[i]);
			}
		}

	private:

		T* m_content;
		size_t m_length;

	};

}
