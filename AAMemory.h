#pragma once

struct AAMemoryPtr {
	size_t val;
	AAMemoryPtr() {
		this->val = 0;
	}
	AAMemoryPtr(size_t ptr) {
		this->val = ptr;
	}
	std::size_t operator()(const AAMemoryPtr& ptr) const {
		return ptr.val;
	}
	bool operator==(const AAMemoryPtr& other) const {
		return this->val == other.val;
	}
	bool operator==(const int& i) const {
		return this->val == i;
	}
	bool operator!=(const int& i) const {
		return !(this->val == i);
	}
};

template <>
struct std::hash<AAMemoryPtr> {
	std::size_t operator()(const AAMemoryPtr& k) const {
		return k.val;
	}
};
