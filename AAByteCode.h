#pragma once

// Byte code operations for AA
enum class AAByteCode : unsigned char {

	NOP, // No operation, expects nothing, does nothing.
	PUSHC, // (Push a constant onto stack from the constants table, 1 argument[const ID])
	PUSHV, // (Push a constant value onto stack, 2 arguments[literal type, value])
	ADD, // (Addition, 0 arguments)
	SUB, // (Subtraction, 0 arguments)
	MUL, // (Multiplication, 0 arguments)
	DIV, // (Division, 0 arguments)
	MOD, // (Modulo, 0 arguments)
	NNEG, // (Numerical Negation, 0 arguments)

	INC, // (Increments top element on stack, 0 arguments)
	DEC, // (Decrements top element on stack, 0 arguments)

	SETVAR, // (Set var, 1 argument[identifier ID])
	GETVAR, // (Get var - pushes value of variable on top of stack, 1 argument[identifier ID])

	JMP, // (Relative jump to operation, 1 argument[distance])
	JMPF, // (Relative jump to operation if top of stack is false, 1 argument[distance])
	JMPT, // (Relative jump to operation if top of stack is true, 1 argument[distance])
	LJMP, // (Long relative jump to operation, 1 argument[distance])
	CALL, // (Call subroutine, 1 argument[procID])
	RET, // (Return, 1 argument[returtCount])

	CMPE, // (Compares the two top elements on stack and returns true if they're equal, 0 arguments)
	CMPNE, // (Compares the two top elements on stack and returns true if they're not equal, 0 arguments)
	LE, // (Compares numerics and returns true if x<y, 0 arguments)
	GE, // (Compares numerics and returns true if x>y, 0 arguments)
	GEQ, // (Compares numerics and returns true if x<=y, 0 arguments)
	LEQ, // (Compares numerics and returns true if x>=y, 0 arguments)

	LNEG, // (Logical Negation, 0 arguments)
	LAND, // (Logical And, 0 arguments)
	LOR, // (Logcal Or, 0 arguments)

};
