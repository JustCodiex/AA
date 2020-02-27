#pragma once

enum class AAByteCode : unsigned char {

	NOP,
	PUSH_CONST, // (Push constant to top of stack, 1 argument[const ID])
	ADD, // (Addition, 2 arguments[left, right])

};

