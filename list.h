#pragma once
#include <vector>
#include <algorithm>
#include <functional>

namespace aa {

	/// <summary>
	/// A list of elements of the same type
	/// </summary>
	/// <typeparam name="T">Arbitrary type</typeparam>
	template <typename T> class list {

	public:

		list() {}

		list(const std::vector<T>& v) {
			m_vector = v;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="val"></param>
		void Add(T val) {
			m_vector.push_back(val);
		}

		/// <summary>
		/// Adds all items from the other list
		/// </summary>
		/// <typeparam name="T"></typeparam>		
		void Add(list<T> other) {
			for (size_t i = 0; i < other.Size(); i++) {
				m_vector.push_back(other.m_vector[i]);
			}
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="val"></param>
		void Remove(T val) {
			m_vector.erase(std::find(m_vector.begin(), m_vector.end(), val));
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="index"></param>
		void RemoveAt(size_t index) {
			m_vector.erase(m_vector.begin() + index);
		}

		/// <summary>
		/// Remove all items if they match the predicate
		/// </summary>
		/// <param name="predicate">The predicate to check on all elements</param>
		void RemoveIf(std::function<bool(T&)> predicate) {
			auto itt = m_vector.begin();
			while (itt != m_vector.end()) {
				if (predicate(*itt)) {
					itt = m_vector.erase(itt);
				} else {
					itt++;
				}
			}
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		T& At(size_t index) {
			return m_vector.at(index);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="val"></param>
		/// <returns></returns>
		bool Contains(T val) {
			return std::find(m_vector.begin(), m_vector.end(), val) != m_vector.end();
		}

		/// <summary>
		/// Find the index of specified element
		/// </summary>
		/// <param name="val">The value to find index of</param>
		/// <returns>Index of element in list or SIZE_MAX if not found</returns>
		size_t IndexOf(T val) {
			for (size_t i = 0; i < m_vector.size(); i++) if (m_vector.at(i) == val) return i;
			return SIZE_MAX;
		}

		/// <summary>
		/// Find the index of specified element
		/// </summary>
		/// <param name="val">The value to find index of</param>
		/// <returns>Index of element in list or SIZE_MAX if not found</returns>
		const size_t IndexOf(T val) const {
			for (size_t i = 0; i < m_vector.size(); i++) if (m_vector.at(i) == val) return i;
			return SIZE_MAX;
		}

		/// <summary>
		/// Return the (amount of elements) size of the list
		/// </summary>
		/// <returns>Size of internal vector</returns>
		size_t Size() const {
			return m_vector.size();
		}

		/// <summary>
		/// First element of the list
		/// </summary>
		T& First() {
			return m_vector.front();
		}

		/// <summary>
		/// Last element of the list
		/// </summary>
		T& Last() {
			return m_vector.back();
		}

		/// <summary>
		/// For each element in list
		/// </summary>
		/// <param name="f">Function to apply to all elements in list</param>
		void ForEach(std::function<void(T&)> f) {
			for (auto& v : m_vector) {
				f(v);
			}
		}

		/// <summary>
		/// Tests if condition holds true for all elements in list
		/// </summary>
		/// <param name="f">Function predicate to test on all elements</param>
		/// <returns>True if all elements fullfill the predicate. False if one does not.</returns>
		bool ForAll(std::function<bool(T&)> f) {
			for (auto& v : m_vector) {
				if (!f(v)) {
					return false;
				}
			}
			return true;
		}

		/// <summary>
		/// Tests if any element fullfills the condition
		/// </summary>
		/// <param name="f">Predicate to test</param>
		/// <returns>True if at least one element fullfills the predicate</returns>
		bool Any(std::function<bool(T&)> f) {
			for (auto& v : m_vector) {
				if (f(v)) {
					return true;
				}
			}
			return false;
		}

		/// <summary>
		/// Maps all elements of the list into another element
		/// </summary>
		/// <typeparam name="U">Other type</typeparam>
		/// <param name="f">Conversion function</param>
		/// <returns>The new list generated from this operation</returns>
		template<class U> list<U> Map(std::function<U(T&)> f) {
			list<U> ls;
			for (auto& v : m_vector) {
				ls.Add(f(v));
			}
			return ls;
		}

		/// <summary>
		/// Filters all elements in the list to a new list such that all elements fulfill the predicate
		/// </summary>
		/// <param name="f">Filter predicate</param>
		/// <returns>New list of all elements that fulfill predicate</returns>
		list<T> Filter(std::function<bool(T&)> f) {
			list<T> ls;
			for (auto& v : m_vector) {
				if (f(v)) {
					ls.Add(v);
				}
			}
			return ls;
		}

		/// <summary>
		/// Returns the first element in the list fulfilling the predicate
		/// </summary>
		/// <param name="predicate">The testing predicate to test on all elements</param>
		/// <returns>First or default element matching the predicate</returns>
		T& FindFirst(std::function<bool(T&)> predicate) {
			for (auto& v : m_vector) {
				if (predicate(v)) {
					return v;
				}
			}
			throw std::exception("Element not found");
		}

		/// <summary>
		/// Returns a list of references to all elements in list matching the predicate
		/// </summary>
		/// <param name="predicate">Predicate function to test on all elements</param>
		/// <returns>List of all elements matching predicate conditions</returns>
		list<T> FindAll(std::function<bool(T&)> predicate) {
			list<T> ls;
			for (auto& v : m_vector) {
				if (predicate(v)) {
					ls.Add(v);
				}
			}
			return ls;
		}

		/// <summary>
		/// Clear internal vector
		/// </summary>
		void Clear() {
			m_vector.clear();
		}

		/// <summary>
		/// Merge two lists into one list
		/// </summary>
		/// <param name="a">The first list to merge</param>
		/// <param name="b">The second list to merge</param>
		/// <returns>New list containing all elements from list A and list B</returns>
		static list<T> Merge(list<T> a, list<T> b) {

			list<T> merged = a;

			while (b.Size() > 0) {
				merged.Add(b.First());
				b.RemoveAt(0);
			}

			return merged;

		}

		/// <summary>
		/// Get the underlying vector object (The actual list)
		/// </summary>
		/// <returns><see cref="std::vector"/> containing all elements from list</returns>
		std::vector<T> Vector() {
			return m_vector;
		}

	private:

		std::vector<T> m_vector;

	};

}
