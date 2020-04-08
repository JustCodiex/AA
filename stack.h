#pragma once
#include <vector>

namespace aa {

	template<typename T>
	class stack {

	public:

		void Push(T val) {
			m_stack.push_back(val);
			m_len++;
		}

		T Pop() {
			T v = m_stack.at(m_len - 1);
			m_stack.pop_back();
			m_len--;
			return v;
		}

		T At(size_t index) {
			return m_stack[index];
		}

		size_t Size() {
			return m_len;
		}

		void Clear() {
			m_stack.clear();
		}

	private:

		std::vector<T> m_stack;
		size_t m_len;

	};

}
