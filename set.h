#pragma once
#include "list.h"

namespace aa {

	/// <summary>
	/// A set of objects
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <class T> class set {

	public:

		/// <summary>
		/// Adds a new element to the set
		/// </summary>
		/// <param name="element">The element to add to set</param>
		/// <returns>True if element was added. False if element was already in the set</returns>
		bool Add(T element) {
			if (!this->Contains(element)) {
				this->m_list.Add(element);
				return true;
			} else {
				return false;
			}
		}

		/// <summary>
		/// Checks if an element is contained within the set
		/// </summary>
		/// <param name="element">The element to check for</param>
		/// <returns>True if element is contained within the set</returns>
		bool Contains(T element) {
			return m_list.Contains(element);
		}

		/// <summary>
		/// Checks if the set contains an element that fullfills the predicate
		/// </summary>
		/// <param name="predicate">Predicate to test</param>
		/// <returns>True if set contains the element</returns>
		bool Contains(std::function<bool(T&)> predicate) {
			return m_list.Any(predicate);
		}

		/// <summary>
		/// Find the first element in list that fulfills the predicate
		/// </summary>
		/// <param name="predicate">Predicate to test on elements</param>
		/// <returns>First element that fulfills predicate conditions</returns>
		T& FindFirst(std::function<bool(T&)> predicate) {
			return this->m_list.FindFirst(predicate);
		}

		/// <summary>
		/// Finds the index of the first element that fulfills the predicate
		/// </summary>
		/// <param name="predicate">The predicate to test on elements</param>
		/// <param name="index">The index of the first element that yielded a true result for the predicate</param>
		/// <returns>True if any element was found matching the predicate. Index will contain the index of that element</returns>
		bool FindFirstIndex(std::function<bool(T&)> predicate, int& index) {
			index = -1;
			try {
				T t = this->FindFirst(predicate);
				index = this->m_list.IndexOf(t);
				return true;
			} catch (std::exception ex) {
				return false;
			}
		}

		/// <summary>
		/// Apply function for each element in the set.
		/// </summary>
		/// <param name="f">Function to call on all set elements</param>
		void ForEach(std::function<void(T&)> f) {
			m_list.ForEach(f);
		}

		/// <summary>
		/// Union with the elements from another set
		/// </summary>
		/// <param name="other">The other set to union with</param>
		void UnionWith(set<T> other) {
			other.ForEach([this](T e) { this->Add(e); });
		}

		/// <summary>
		/// Create a new set representing the union between two unions
		/// </summary>
		/// <param name="other">The other union to create new set with</param>
		/// <returns>New set with all elements from the two lists</returns>
		set<T> Union(set<T> other) {
			set<T> s = set<T>(this);
			s.UnionWith(other);
			return s;
		}

		/// <summary>
		/// Convert the set into a list
		/// </summary>
		/// <returns>list representation of the set</returns>
		aa::list<T> ToList() {
			return m_list;
		}

		/// <summary>
		/// Get the size of the set
		/// </summary>
		/// <returns>The amount of items in the set</returns>
		size_t Size() {
			return m_list.Size();
		}

		/// <summary>
		/// Applies some action to element that's found at the specified index
		/// </summary>
		/// <param name="index">The index of the element to apply action to</param>
		/// <returns>Element found at index</returns>
		T& Apply(size_t index) {
			if (index >= 0 && index < m_list.Size()) {
				return m_list.At(index);
			} else {
				throw std::exception("Index out of range");
			}
		}

		/// <summary>
		/// Clears the set and copies all distinct elements from the vector
		/// </summary>
		/// <param name="vec">The vector to copy set elements from</param>
		void FromVector(std::vector<T> vec) {
			m_list = aa::list<T>();
			for (T& t : vec) {
				this->Add(t);
			}
		}

		/// <summary>
		/// Clears the set and copies all distinct elements from the list into the set
		/// </summary>
		/// <param name="ls">The list to copy set elements from</param>
		void FromList(aa::list<T> ls) {
			m_list = aa::list<T>();
			ls.ForEach([this](T& t) { this->Add(t); });
		}

	private:

		aa::list<T> m_list;

	};

}
