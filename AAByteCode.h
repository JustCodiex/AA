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

	SETFIELD, // (Set field, 1 argument[field ID])
	GETFIELD, // (Get field, 1 argument[field ID])

	GETELEM, // (Get element based on dynamic indexing, 0 arguments)
	SETELEM, // (Set element based on dynamic indexing, 0 arguments)

	JMP, // (Relative jump to operation, 1 argument[distance])
	JMPF, // (Relative jump to operation if top of stack is false, 1 argument[distance])
	JMPT, // (Relative jump to operation if top of stack is true, 1 argument[distance])
	LJMP, // (Long relative jump to operation, 1 argument[distance])
	CALL, // (Call subroutine, 2 arguments[procID, argCount])
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

	HALLOC, // (Heap Allocate memory for new object, 1 argument[size in bytes])
	SALLOC, // (Stack allocate memory for the new object, 1 argument[size in bytes])

	VMCALL, // (Virtual Machine call, TBD)

	TRY, // ---
	THROW, // ---
	BRK, // (Break, 0 args)

	CASTI2F, // i32 => f32
	CASTF2I, // f32 => i32
	CASTS2I, // i16 => f32
	CASTS2F, // i16 => f32
	CASTF2S, // f32 => i16
	CASTL2F, // i64 => f32
	CASTL2I, // i64 => i32
	CASTL2S, // i64 => i16
	CASTF2L, // f32 => i64
	CASTI2D, // i32 => f64
	CASTS2D, // i16 => f64
	CASTF2D, // f32 => f64
	CASTL2D, // i64 => f64
	CASTD2I, // f64 => i32
 	CASTD2S, // f64 => i16
	CASTD2F, // f64 => f32
 	CASTD2L, // f64 => i64

};
