#pragma once

// Byte code operations for AA
enum class AAByteCode : unsigned char {

	NOP,
	PUSHC, // (Push a constant onto stack from the constants table, 1 argument[const ID])
	PUSHV, // (Push a constant value onto stack, 2 arguments[literal type, value])
	ADD, // (Addition, 0 arguments)
	SUB, // (Subtraction, 0 arguments)
	MUL, // (Multiplication, 0 arguments)
	DIV, // (Division, 0 arguments)
	MOD, // (Modulo, 0 arguments)

};
