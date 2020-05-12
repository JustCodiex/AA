#include "AAExecute.h"

namespace aa {

    namespace vm {

        AAStackValue PopSomething(AAPrimitiveType primitiveType, any_stack& stack) {

            switch (primitiveType) {
            case AAPrimitiveType::byte:
                return AAStackValue(stack.Pop<unsigned char>());
            case AAPrimitiveType::sbyte:
                return AAStackValue(stack.Pop<signed char>());
            case AAPrimitiveType::boolean:
                return AAStackValue(stack.Pop<bool>());
            case AAPrimitiveType::int16:
                return AAStackValue(stack.Pop<int16_t>());
            case AAPrimitiveType::uint16:
                return AAStackValue(stack.Pop<uint16_t>());
            case AAPrimitiveType::int32:
                return AAStackValue(stack.Pop<int32_t>());
            case AAPrimitiveType::uint32:
                return AAStackValue(stack.Pop<uint32_t>());
            case AAPrimitiveType::int64:
                return AAStackValue(stack.Pop<int64_t>());
            case AAPrimitiveType::uint64:
                return AAStackValue(stack.Pop<uint64_t>());
            case AAPrimitiveType::real32:
                return AAStackValue(stack.Pop<float_t>());
            case AAPrimitiveType::real64:
                return AAStackValue(stack.Pop<double_t>());
            case AAPrimitiveType::wchar:
                return AAStackValue(stack.Pop<wchar_t>());
            case AAPrimitiveType::string:
            case AAPrimitiveType::refptr:
                return AAStackValue(stack.Pop<AAMemoryPtr>());
            case AAPrimitiveType::intptr:
                return AAStackValue(stack.Pop<AAIntPtr>());
            default:
                break;
            }

            return AAStackValue::None; // should be an error....

        }

        void PushSomething(AAStackValue value, any_stack& stack) {

            switch (value.get_type()) {
            case AAPrimitiveType::boolean:
                stack.Push(value.to_cpp<bool>());
                break;
            case AAPrimitiveType::byte:
                stack.Push(value.to_cpp<unsigned char>());
                break;
            case AAPrimitiveType::sbyte:
                stack.Push(value.to_cpp<signed char>());
                break;
            case AAPrimitiveType::int16:
                stack.Push(value.to_cpp<int16_t>());
                break;
            case AAPrimitiveType::int32:
                stack.Push(value.to_cpp<int32_t>());
                break;
            case AAPrimitiveType::int64:
                stack.Push(value.to_cpp<int64_t>());
                break;
            case AAPrimitiveType::uint16:
                stack.Push(value.to_cpp<uint16_t>());
                break;
            case AAPrimitiveType::uint32:
                stack.Push(value.to_cpp<uint32_t>());
                break;
            case AAPrimitiveType::uint64:
                stack.Push(value.to_cpp<uint64_t>());
                break;
            case AAPrimitiveType::real32:
                stack.Push(value.to_cpp<float_t>());
                break;
            case AAPrimitiveType::real64:
                stack.Push(value.to_cpp<double_t>());
                break;
            case AAPrimitiveType::wchar:
                stack.Push(value.to_cpp<wchar_t>());
                break;
            case AAPrimitiveType::string:
            case AAPrimitiveType::refptr:
                stack.Push(value.to_cpp<AAMemoryPtr>());
                break;
            case AAPrimitiveType::intptr:
                stack.Push(value.to_cpp<AAIntPtr>());
                break;
            default:
                break;
            }

        }

        void PushConstant(AA_Literal lit, any_stack& stack) {

            switch (lit.tp) {
            case AALiteralType::Boolean:
                stack.Push(lit.lit.b.val);
                break;
            case AALiteralType::Char:
                stack.Push(lit.lit.c.val);
                break;
            case AALiteralType::Float:
                stack.Push(lit.lit.f.val);
                break;
            case AALiteralType::Int:
                stack.Push<int32_t>((int32_t)lit.lit.i.val);
                break;
            case AALiteralType::IntPtr:
                stack.Push(AAIntPtr(lit.lit.ptr.ptr));
                break;
            case AALiteralType::Null:
                stack.Push(AAMemoryPtr(0));
                break;
            case AALiteralType::String:
                stack.Push(std::wstring(lit.lit.s.val));
                break;
            default:
                break;
            }

        }

    }

}
