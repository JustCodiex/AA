#pragma once
#include "list.h"
#include <type_traits>

namespace aa {

	/// <summary>
	/// Represents a pair in a set
	/// </summary>
	/// <typeparam name="U">Type of first element</typeparam>
	/// <typeparam name="V">Type of second element</typeparam>
	template <typename U, typename V>
	struct set_pair {
		U first; // first element
		V second; // second element
		set_pair(U f, V s) {
			this->first = f;
			this->second = s;
		}
		bool operator==(set_pair p) {
			return 
				(this->first == p.first && this->second == p.second) ||
				(this->first == p.second && this->second == p.first);
		}
	};


	/// <summary>
	/// A set of objects
	/// </summary>
	/// <typeparam name="T">Type contained within the set</typeparam>
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
		/// Find all elements matching predicate
		/// </summary>
		/// <param name="predicate">Predicate to test on all elements in list</param>
		/// <returns>Set containing all elements matching predicate conditions</returns>
		set<T> FindAll(std::function<bool(T&)> predicate) {
			set<T> s;
			s.FromList(m_list.FindAll(predicate));
			return s;
		}

		/// <summary>
		/// Check if a predicate holds true for all elements in set
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns>True if the predicate returned true for all elements</returns>
		bool ForAll(std::function<bool(T&)> predicate) {
			return this->m_list.ForAll(predicate);
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
		/// Create a new set representing the union between two sets
		/// </summary>
		/// <param name="other">The other union to create new set with</param>
		/// <returns>New set with all elements from the two lists</returns>
		set<T> Union(set<T> other) {
			set<T> s = set<T>(*this);
			s.UnionWith(other);
			return s;
		}

		/// <summary>
		/// Merges with another set where a merge predicate is executed on all elements in the other set. If it holds true a merge function is executed on element before it's added
		/// </summary>
		/// <param name="other">Other set to merge with</param>
		/// <param name="mergePredicate">The predicate that an element must pass for it to be included</param>
		/// <param name="mergeAction">The action to perform on the element</param>
		/// <returns>The amount of elements that was merged</returns>
		int Merge(set<T> other, std::function<bool(T)> mergePredicate, std::function<T(T)> mergeFunc) {
			int merged = 0;
			other.ForEach([&merged, this, mergePredicate, mergeFunc] (T& elem)
				{
					if (mergePredicate(elem)) {
						T sub = mergeFunc(elem);
						if (this->Add(sub)) {
							merged++;
						}
					}
				}
			);
			return merged;
		}

		/// <summary>
		/// Create a new set representing the difference between two sets
		/// </summary>
		/// <param name="other">The other set to create difference set from</param>
		/// <returns>New set representing the set difference between the two sets</returns>
		set<T> Difference(set<T> other) {
			set<T> s = this->Union(other);
			s.m_list.RemoveIf([this](T& t) { return this->Contains(t); });
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

	template<typename U, typename V>
	set<set_pair<U, V>> operator*(set<U> set_a, set<V> set_b) {
		set<set_pair<U, V>> product; // Create set of cartesian products
		list<U> _ls = set_a.ToList(); // Convert the first set to a list
		for (size_t i = 0; i < _ls.Size(); i++) { // Loop through the list
			U _a = _ls.At(i); // Get eleemnt
			set_b.ForEach([&product, _a](U& t) { product.Add(set_pair<U, V>(_a, t)); }); // Go through all elements in second set and create a pair with first element
		}
		return product; // Return product
	} // What a nightmare to write (I already hate C++ templates)

}
