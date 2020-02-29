#pragma once

enum class AALiteralType : unsigned char {
	Int,
	String,
	Float,
	Boolean,
};

struct AA_IntLiteral {
	signed int val;
	bool operator==(AA_IntLiteral b) {
		return this->val == b.val;
	}
};

struct AA_StringLiteral {
	wchar_t* val;
	unsigned short len;
	bool operator==(AA_StringLiteral b) {
		if (this->len == b.len) {
			return wcscmp(this->val, b.val) == 0;
		} else {
			return false;
		}
	}
};

struct AA_FloatLiteral {
	float val;
	bool operator==(AA_FloatLiteral b) {
		return this->val == b.val;
	}
};

struct AA_BoolLiteral {
	bool val;
	bool operator==(AA_BoolLiteral b) {
		return this->val == b.val;
	}
};

union AA_AnyLiteral {
	AA_IntLiteral i;
	AA_BoolLiteral b;
	AA_StringLiteral s;
	AA_FloatLiteral f;
};

struct AA_Literal {
	AALiteralType tp;
	AA_AnyLiteral lit;
	bool operator==(AA_Literal b) {
		if (this->tp == b.tp) {
			switch (this->tp) {
			case AALiteralType::Int:
				return this->lit.i == b.lit.i;
			case AALiteralType::Boolean:
				return this->lit.b == b.lit.b;
			case AALiteralType::Float:
				return this->lit.f == b.lit.f;
			case AALiteralType::String:
				return this->lit.s == b.lit.s;
			default:
				return false;
			}
		} else {
			return false;
		}
	}
	AA_Literal operator+(AA_Literal other) {
		switch (this->tp) {
		case AALiteralType::Int:
			AA_Literal l = AA_Literal(*this);
			l.lit.i.val += other.lit.i.val;
			return l;
		default:
			break;
		}
		return *this;
	}
	AA_Literal operator-(AA_Literal other) {
		switch (this->tp) {
		case AALiteralType::Int:
			AA_Literal l = AA_Literal(*this);
			l.lit.i.val -= other.lit.i.val;
			return l;
		default:
			break;
		}
		return *this;
	}
	AA_Literal operator*(AA_Literal other) {
		switch (this->tp) {
		case AALiteralType::Int:
			AA_Literal l = AA_Literal(*this);
			l.lit.i.val *= other.lit.i.val;
			return l;
		default:
			break;
		}
		return *this;
	}
	AA_Literal operator/(AA_Literal other) {
		switch (this->tp) {
		case AALiteralType::Int:
			AA_Literal l = AA_Literal(*this);
			l.lit.i.val /= other.lit.i.val;
			return l;
		default:
			break;
		}
		return *this;
	}
	AA_Literal operator%(AA_Literal other) {
		switch (this->tp) {
		case AALiteralType::Int:
			AA_Literal l = AA_Literal(*this);
			l.lit.i.val %= other.lit.i.val;
			return l;
		default:
			break;
		}
		return *this;
	}
	AA_Literal operator-() {
		switch (this->tp) {
		case AALiteralType::Int:
			AA_Literal l = AA_Literal(*this);
			l.lit.i.val = -l.lit.i.val;
			return l;
		default:
			break;
		}
		return *this;
	}
};
