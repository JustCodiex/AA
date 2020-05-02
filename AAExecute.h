#pragma once
#include "AAStackValue.h"
#include "AAByteCode.h"
#include "any_stack.h"

namespace aa {
	namespace vm {

		template <typename N>
		inline N Arithmetic_BinaryOperation(AAByteCode op, N lhs, N rhs) {
			switch (op) {
			case AAByteCode::ADD: {
				return lhs + rhs;
			}
			case AAByteCode::SUB: {
				return lhs - rhs;
			}
			case AAByteCode::MUL: {
				return lhs * rhs;
			}
			case AAByteCode::DIV: {
				return lhs / rhs;
			}
			default:
				break;
			}
			return lhs;
		}

		template <typename N>
		inline N Mod_BinaryOperation(N lhs, N rhs) {
			return lhs % rhs;
		}

		template <typename N>
		inline bool Logic_BinaryOperation(AAByteCode op, N lhs, N rhs) {
			switch (op) {
			case AAByteCode::CMPE:
				return lhs == rhs;
			case AAByteCode::LE: 
				return lhs < rhs;
			case AAByteCode::LEQ: 
				return lhs <= rhs;
			case AAByteCode::GE: 
				return lhs > rhs;
			case AAByteCode::GEQ:
				return lhs >= rhs;
			default:
				break;
			}
			return false;
		}

		template <typename N>
		inline N Arithmetic_UnaryOperation(AAByteCode op, N rhs) {
			switch (op) {
			case AAByteCode::NNEG:
				return -rhs;
			default:
				return rhs; // dunno what to do
			}
		}

		AAStackValue PopSomething(AAPrimitiveType primitiveType, any_stack& stack);

		void PushSomething(AAStackValue value, any_stack& stack);

		void PushConstant(AA_Literal lit, any_stack& stack);

	}

}
