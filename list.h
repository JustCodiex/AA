#pragma once
#include <vector>
#include <algorithm>

namespace aa {

	// A light-weight implementation of a list object
	// Implements the most used functionalities expected of a list structure
	// (add, remove, indexof, contains, size, and elementAt)
	template <typename T> class list {

	public:

		void Add(T val) {
			m_vector.push_back(val);
		}

		// Adds all items from the other list
		void Add(list<T> other) {
			for (size_t i = 0; i < other.Size(); i++) {
				m_vector.push_back(other.m_vector[i]);
			}
		}

		void Remove(T val) {
			m_vector.erase(std::find(m_vector.begin(), m_vector.end(), val));
		}

		void RemoveAt(size_t index) {
			m_vector.erase(m_vector.begin() + index);
		}

		T At(size_t index) {
			return m_vector.at(index);
		}

		bool Contains(T val) {
			return std::find(m_vector.begin(), m_vector.end(), val) != m_vector.end();
		}

		size_t IndexOf(T val) {
			for (size_t i = 0; i < m_vector.size(); i++) if (m_vector.at(i) == val) return i;
			return -1;
		}

		size_t Size() {
			return m_vector.size();
		}

		T& First() {
			return m_vector.front();
		}

		T& Last() {
			return m_vector.back();
		}

		static list<T> Merge(list<T> a, list<T> b) {

			list<T> merged = a;

			while (b.Size() > 0) {
				merged.Add(b.First());
				b.RemoveAt(0);
			}

			return merged;

		}

	private:

		std::vector<T> m_vector;

	};

}
