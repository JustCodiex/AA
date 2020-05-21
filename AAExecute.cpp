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
            case AAPrimitiveType::tuple:
                return AAStackValue(stack.Pop<AATuple>());
            case AAPrimitiveType::__TRUEANY:
                return AAStackValue(AAPrimitiveType::__TRUEANY, stack.Pop<AAVal>());
            default:
                break;
            }

            return AAStackValue::None; // should be an error....

        }

        void PushSomething(const AAStackValue& value, any_stack& stack) {

            PushSomething(value.get_type(), value.as_val(), stack);

            /*switch (value.get_type()) {
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
            case AAPrimitiveType::tuple:
                stack.Push(value.to_cpp<AATuple>());
            default:
                break;
            }*/

        }

        void PushSomething(const AAPrimitiveType& type, const AAVal& value, any_stack& stack) {

            switch (type) {
            case AAPrimitiveType::boolean:
                stack.Push(value.Raw<bool>());
                break;
            case AAPrimitiveType::byte:
                stack.Push(value.Raw<unsigned char>());
                break;
            case AAPrimitiveType::sbyte:
                stack.Push(value.Raw<signed char>());
                break;
            case AAPrimitiveType::int16:
                stack.Push(value.Raw<int16_t>());
                break;
            case AAPrimitiveType::int32:
                stack.Push(value.Raw<int32_t>());
                break;
            case AAPrimitiveType::int64:
                stack.Push(value.Raw<int64_t>());
                break;
            case AAPrimitiveType::uint16:
                stack.Push(value.Raw<uint16_t>());
                break;
            case AAPrimitiveType::uint32:
                stack.Push(value.Raw<uint32_t>());
                break;
            case AAPrimitiveType::uint64:
                stack.Push(value.Raw<uint64_t>());
                break;
            case AAPrimitiveType::real32:
                stack.Push(value.Raw<float_t>());
                break;
            case AAPrimitiveType::real64:
                stack.Push(value.Raw<double_t>());
                break;
            case AAPrimitiveType::wchar:
                stack.Push(value.Raw<wchar_t>());
                break;
            case AAPrimitiveType::string:
            case AAPrimitiveType::refptr:
                stack.Push(value.Raw<AAMemoryPtr>());
                break;
            case AAPrimitiveType::intptr:
                stack.Push(value.Raw<AAIntPtr>());
                break;
            case AAPrimitiveType::tuple:
                stack.Push(value.Raw<AATuple>());
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
