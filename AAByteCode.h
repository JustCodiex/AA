#pragma once

/// <summary>
/// Byte code operations for AA
/// </summary>
enum class AAByteCode : unsigned char {

	NOP, // No operation, expects nothing, does nothing.
	PUSHC, // (Push a constant onto stack from the constants table, 1 argument[const ID])
	PUSHN, // (Push a null onto stack, 0 arguments)
	PUSHV, // (Push a constant value onto stack, 2 arguments[size, value])
	PUSHWS, // (Push a wide-string from the constants table, 1 argument[const ID]);

	ADD, // (Addition, 1 argument [primitive type])
	SUB, // (Subtraction, 1 argument [primitive type])
	MUL, // (Multiplication, 1 argument [primitive type])
	DIV, // (Division, 1 argument [primitive type])
	MOD, // (Modulo, 1 argument [primitive type])
	NNEG, // (Numerical Negation, 1 argument [primitive type])

	CONCAT, // (Concatenation on two strings, 0 arguments)
	LEN, // (Length of a string or array, '#' unary operator, 2 arguments [object type, dimension])

	INC, // (Increments top element on stack, 2 argument, [primitive type, order {0 = increment and return, 1=return and increment}])
	DEC, // (Decrements top element on stack, 2 argument, [primitive type, order {0 = decrement and return, 1=return and decrement}])

	SETVAR, // (Set var, 2 arguments[identifier ID, primitive type])
	GETVAR, // (Get var - pushes value of variable on top of stack, 1 argument[identifier ID])

	SETFIELD, // (Set field, 2 arguments[field ID, primitive type])
	GETFIELD, // (Get field, 2 arguments[field ID, primitive type])

	GETELEM, // (Get element based on dynamic indexing, 1 argument[dimension])
	SETELEM, // (Set element based on dynamic indexing, 1 argument[dimension])

	JMP, // (Relative jump to operation, 1 argument[distance])
	JMPF, // (Relative jump to operation if top of stack is false, 1 argument[distance])
	JMPT, // (Relative jump to operation if top of stack is true, 1 argument[distance])
	LJMP, // (Long relative jump to operation, 1 argument[distance])
	CALL, // (Call subroutine, 2 arguments[procID, argCount])
	VCALL, // (Virtual call, 2 arguments[procID, argCount])>: Only call with objects!
	XCALL, // (External call - Invoke VM specified call, 2 arguments[procID, argCount])
	RET, // (Return, 0 arguments)

	CMPE, // (Compares the two top elements on stack and returns true if they're equal, 1 argument [primitive type])
	CMPNE, // (Compares the two top elements on stack and returns true if they're not equal, 1 argument [primitive type])
	LE, // (Compares numerics and returns true if x<y, 1 argument [primitive type])
	GE, // (Compares numerics and returns true if x>y, 1 argument [primitive type])
	GEQ, // (Compares numerics and returns true if x<=y, 1 argument [primitive type])
	LEQ, // (Compares numerics and returns true if x>=y, 1 argument [primitive type])

	LNEG, // (Logical Negation !, 0 arguments)
	LAND, // (Logical And &&, 0 arguments)
	LOR, // (Logcal Or ||, 0 arguments)

	BAND, // (Bitwise And &, 1 argument [primitive type])
	BOR, // (Bitwise Or |, 1 argument [primitive type])

	TUPLECMP, // (Compares two tuples, 0 arguments)
	TUPLECMPORSET,	// (Compares two tuples, where top-of-stack tuple may assign to variables, 1+n arguments[tuple count, { -1 | varid }^n : n = tuple count])
					// -1 = compare normally
					// 0 <= n < INT32_MAX = assign to var
					// ONT32_MAX = pop
	TUPLENEW, // (Construct a tuple, 1+n arguments[tuple count, { primitive type }^n : n = tuple count])
	TUPLEGET, // (Get the value at specified int in a tuple, 1 argument[tuple index])

	ALLOC, // (Heap Allocate memory for a new object, 1 argument[size in bytes]) |Not in use|
	ALLOCARRAY, // (Heap Allocate memory for a new array, 2 arguments[primitive type, dimension count])
	CTOR, // (Call constructor and bind type, 4 arguments[typeID, ctor, isvm, allocsize]) |ALLOC is built into this argument|
	
	TRY, // ---
	THROW, // ---
	BRK, // (Break, 0 args)

	POP, // (Pops top element of stack, 1 argument [bytes to pop])

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

	WRAP, // (Wrap top of stack into a stackvalue - save the type, 1 argument[primitive type])
	UNWRAP, // (Unwraps the top stack element from its associated stackvalue, 0 arguments)

	EXTTAG, // (Extract tags from object and put into tuple, 0 arguments)
	TAGTUPLECMP, // (Compares two tuples, first tuple value is preset to typeID and only one tuple is on the stack, 1 argument[Match type ID])
	TAGTUPLECMPORSET,	// (Compares two tuples, where top-of-stack tuple may assign to variables, first tuple value is ignored (typeId) and only one tuple can be on the top stack, 
						// 2+n arguments[tuple count Match type ID, { -1 | varid }^n : n = tuple count])
						// -1 = compare normally
						// 0 <= n < INT32_MAX = assign to var
						// ONT32_MAX = pop

	ACCEPT, // (Push a runtime-only-type onto the stack that will always yield true in accepting comparrisons [match statements], 0 arguments)

};
