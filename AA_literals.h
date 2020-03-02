#pragma once
#include <string>

enum class AALiteralType : unsigned char {
	Int,
	String,
	Float,
	Boolean,
};

struct AA_IntLiteral {
	signed int val;
	AA_IntLiteral() {
		this->val = 0;
	}
	AA_IntLiteral(signed int v) {
		this->val = v;
	}
	bool operator==(AA_IntLiteral b) {
		return this->val == b.val;
	}
};

struct AA_StringLiteral {
	wchar_t* val;
	size_t len;
	AA_StringLiteral() {
		this->val = 0;
		this->len = -1;
	}
	AA_StringLiteral(wchar_t* val, size_t len) {
		this->val = val;
		this->len = len;
	}
	AA_StringLiteral(const wchar_t* val) {
		this->len = wcslen(val);
		this->val = new wchar_t[this->len];
		wcscpy(this->val, val);
	}
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
	AA_FloatLiteral(float v) {
		this->val = v;
	}
	AA_FloatLiteral() {
		this->val = 0.0f;
	}
	bool operator==(AA_FloatLiteral b) {
		return this->val == b.val;
	}
};

struct AA_BoolLiteral {
	bool val;
	AA_BoolLiteral(bool v) {
		this->val = v;
	}
	AA_BoolLiteral() {
		this->val = false;
	}
	bool operator==(AA_BoolLiteral b) {
		return this->val == b.val;
	}
};

union AA_AnyLiteral {
	AA_IntLiteral i;
	AA_BoolLiteral b;
	AA_StringLiteral s;
	AA_FloatLiteral f;
	AA_AnyLiteral() {
		memset(this, 0, sizeof(AA_AnyLiteral));
	}
};

struct AA_Literal {
	AALiteralType tp;
	AA_AnyLiteral lit;
	AA_Literal() {
		lit.i = 0;
		tp = AALiteralType::Int;
	}
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
		case AALiteralType::Int: {
			AA_Literal l = AA_Literal(*this);
			l.lit.i.val += other.lit.i.val;
			return l;
		}
		case AALiteralType::Float: {
			AA_Literal l = AA_Literal(*this);
			l.lit.f.val += other.lit.f.val;
			return l;
		}
		default:
			break;
		}
		return *this;
	}
	AA_Literal operator-(AA_Literal other) {
		switch (this->tp) {
		case AALiteralType::Int: {
			AA_Literal l = AA_Literal(*this);
			l.lit.i.val -= other.lit.i.val;
			return l;
		}
		case AALiteralType::Float: {
			AA_Literal l = AA_Literal(*this);
			l.lit.f.val -= other.lit.f.val;
			return l;
		}
		default:
			break;
		}
		return *this;
	}
	AA_Literal operator*(AA_Literal other) {
		switch (this->tp) {
		case AALiteralType::Int: {
			AA_Literal l = AA_Literal(*this);
			l.lit.i.val *= other.lit.i.val;
			return l;
		}
		case AALiteralType::Float: {
			AA_Literal l = AA_Literal(*this);
			l.lit.f.val *= other.lit.f.val;
			return l;
		}
		default:
			break;
		}
		return *this;
	}
	AA_Literal operator/(AA_Literal other) {
		switch (this->tp) {
		case AALiteralType::Int: {
			AA_Literal l = AA_Literal(*this);
			l.lit.i.val /= other.lit.i.val;
			return l;
		}
		case AALiteralType::Float: {
			AA_Literal l = AA_Literal(*this);
			l.lit.f.val /= other.lit.f.val;
			return l;
		}
		default:
			break;
		}
		return *this;
	}
	AA_Literal operator%(AA_Literal other) {
		switch (this->tp) {
		case AALiteralType::Int: {
			AA_Literal l = AA_Literal(*this);
			l.lit.i.val %= other.lit.i.val;
			return l;
		}
		/*case AALiteralType::Float: { // TODO: Implement
			AA_Literal l = AA_Literal(*this);
			l.lit.f.val %= other.lit.f.val;
			return l;
		}*/
		default:
			break;
		}
		return *this;
	}
	AA_Literal operator-() {
		switch (this->tp) {
		case AALiteralType::Int: {
			AA_Literal l = AA_Literal(*this);
			l.lit.i.val = -l.lit.i.val;
			return l;
		}
		default:
			break;
		}
		return *this;
	}
	AA_Literal operator!() {
		switch (this->tp) {
		case AALiteralType::Boolean: {
			AA_Literal l = AA_Literal(*this);
			l.lit.b.val = !l.lit.b.val;
			return l;
		}
		default:
			break;
		}
		return *this;
	}
};
