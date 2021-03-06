#include "AAB2F.h"
#include "AAVM.h"
#include "AAPrimitiveType.h"
#include <fstream>
#include <iostream>
#include <iomanip>

namespace aa {

	void dump_bytecode(std::wstring file, AAC_Out bytecode) {

		std::ofstream o = std::ofstream(file);

		if (o.is_open()) {

			for (size_t i = 0; i < bytecode.length; i++) {
				o << (char)bytecode.bytes[i];
			}

			o.flush();
			o.close();

		}

	}

	std::wstring OpToWString(CompiledAbstractExpression CAE) {
		std::wstring output = L"";
		switch (CAE.bc) {
		case AAByteCode::ADD:
			output = L"ADD";
			break;
		case AAByteCode::CALL:
			output = L"CALL";
			break;
		case AAByteCode::CMPE:
			output = L"CMPE";
			break;
		case AAByteCode::CMPNE:
			output = L"CMPNE";
			break;
		case AAByteCode::DEC:
			output = L"DEC";
			break;
		case AAByteCode::DIV:
			output = L"DIV";
			break;
		case AAByteCode::GE:
			output = L"GE";
			break;
		case AAByteCode::GEQ:
			output = L"GEQ";
			break;
		case AAByteCode::GETVAR:
			output = L"GETVAR";
			break;
		case AAByteCode::INC:
			output = L"INC";
			break;
		case AAByteCode::JMPF:
			output = L"JMPF";
			break;
		case AAByteCode::JMPT:
			output = L"JMPT";
			break;
		case AAByteCode::JMP:
			output = L"JMP";
			break;
		case AAByteCode::LAND:
			output = L"LAND";
			break;
		case AAByteCode::LE:
			output = L"LE";
			break;
		case AAByteCode::LEQ:
			output = L"LEQ";
			break;
		case AAByteCode::LJMP:
			output = L"LJMP";
			break;
		case AAByteCode::LNEG:
			output = L"LNEG";
			break;
		case AAByteCode::LOR:
			output = L"LOR";
			break;
		case AAByteCode::MOD:
			output = L"MOD";
			break;
		case AAByteCode::MUL:
			output = L"MUL";
			break;
		case AAByteCode::NNEG:
			output = L"NNEG";
			break;
		case AAByteCode::NOP:
			output = L"NOP";
			break;
		case AAByteCode::PUSHC:
			output = L"PUSHC";
			break;
		case AAByteCode::PUSHV:
			output = L"PUSHV";
			break;
		case AAByteCode::PUSHN:
			output = L"PUSHN";
			break;
		case AAByteCode::PUSHWS:
			output = L"PUSHWS";
			break;
		case AAByteCode::RET:
			output = L"RET";
			break;
		case AAByteCode::SETVAR:
			output = L"SETVAR";
			break;
		case AAByteCode::SUB:
			output = L"SUB";
			break;
		case AAByteCode::ALLOC:
			output = L"ALLOC";
			break;
		case AAByteCode::ALLOCARRAY:
			output = L"ALLOCARRAY";
			break;
		case AAByteCode::GETFIELD:
			output = L"GETFIELD";
			break;
		case AAByteCode::SETFIELD:
			output = L"SETFIELD";
			break;
		case AAByteCode::GETELEM:
			output = L"GETELEM";
			break;
		case AAByteCode::SETELEM:
			output = L"SETELEM";
			break;
		case AAByteCode::XCALL:
			output = L"XCALL";
			break;
		case AAByteCode::VCALL:
			output = L"VCALL";
			break;
		case AAByteCode::POP:
			output = L"POP";
			break;
		case AAByteCode::CONCAT:
			output = L"CONCAT";
			break;
		case AAByteCode::LEN:
			output = L"LEN";
			break;
		case AAByteCode::CTOR:
			output = L"CTOR";
			break;
		case AAByteCode::WRAP:
			output = L"WRAP";
			break;
		case AAByteCode::UNWRAP:
			output = L"UNWRAP";
			break;
		case AAByteCode::EXTTAG:
			output = L"EXTTAG";
			break;
		case AAByteCode::TUPLECMP:
			output = L"TUPLECMP";
			break;
		case AAByteCode::TUPLENEW:
			output = L"TUPLENEW";
			break;
		case AAByteCode::TUPLEGET:
			output = L"TUPLEGET";
			break;
		case AAByteCode::ACCEPT:
			output = L"ACCEPT";
			break;
		case AAByteCode::TUPLECMPORSET:
			output = L"TUPLECMPORSET";
			break;
		case AAByteCode::TAGTUPLECMP:
			output = L"TAGTUPLECMP";
			break;
		case AAByteCode::TAGTUPLECMPORSET:
			output = L"TAGTUPLECMPORSET";
			break;
		case AAByteCode::BINAND:
			output = L"BINAND";
			break;
		case AAByteCode::BINOR:
			output = L"BINOR";
			break;
		case AAByteCode::ANDTRUE:
			output = L"ANDTRUE";
			break;
		default:
			output = L"_undefined_(" + std::to_wstring((int)CAE.bc) + L")";
			break;
		}
		for (int i = 0; i < CAE.argCount; i++) {
			output += L", " + std::to_wstring(CAE.argValues[i]);
		}
		return output;
	}

	std::wstring getLitType(AALiteralType lt) {
		switch (lt) {
		case AALiteralType::Int:
			return L"I32";
		case AALiteralType::String:
			return L"UTF-8";
		case AALiteralType::Float:
			return L"F32";
		case AALiteralType::Boolean:
			return L"B";
		case AALiteralType::Char:
			return L"C";
		case AALiteralType::Null:
			return L"NULL";
		default:
			return L"???";
		}
	}

	std::wstring getLitValue(AA_Literal l) {
		switch (l.tp) {
		case AALiteralType::Int:
			return std::to_wstring(l.lit.i.val);
		case AALiteralType::String:
			return L"\"" + std::wstring(l.lit.s.val) + L"\"";
		case AALiteralType::Float:
			return std::to_wstring(l.lit.f.val);
		case AALiteralType::Boolean:
			return (l.lit.b.val == true)?L"T":L"F";
		case AALiteralType::Char:
			if (l.lit.c.val == '\n') {
				return L"\\n";
			} else if (l.lit.c.val == '\t') {
				return L"\\t";
			} else {
				return std::wstring(1, l.lit.c.val);
			}
		case AALiteralType::Null:
			return L"";
		default:
			return L"???";
		}
	}

	std::wstring getTypeVariant(AAByteTypeVariant v) {
		switch (v) {
		case AAByteTypeVariant::Val:
			return L"ValueType";
		case AAByteTypeVariant::Ref:
			return L"ReferenceType";
		case AAByteTypeVariant::Enum:
			return L"EnumType";
		case AAByteTypeVariant::Interface:
			return L"InterfaceType";
		case AAByteTypeVariant::Object:
			return L"ObjectType";
		default:
			return L"";
		}
	}

	void dump_types(std::wofstream& o, std::vector<AAByteType> types) {

		if (types.size() > 1) {

			o << L"\n\nBEGIN EXPORT TYPES:\n";

			for (size_t i = 1; i < types.size(); i++) {

				o << L"\tBEGIN TYPE (UID: " << types[i].constID << L"):\n";
				o << L"\t\t[" << getTypeVariant(types[i].typeVariant) << L"] " << std::wstring(types[i].typeName) << L"\n";
				o << L"\t\tUnmanaged size: " << types[i].unmanagedSize << L"\n";
				o << L"\tEND TYPE;";
				o << L"\n";

				if (i < types.size() - 1) {
					o << L"\n";
				}

			}

			o << L"END EXPORT TYPES;\n\n";

		}

	}

	bool __is_primitive_code(AAByteCode code) {
		return code == AAByteCode::ADD || code == AAByteCode::SUB || code == AAByteCode::MUL || code == AAByteCode::DIV || code == AAByteCode::LE ||
			code == AAByteCode::LEQ || code == AAByteCode::GE || code == AAByteCode::GEQ ||	code == AAByteCode::MOD || code == AAByteCode::NNEG || 
			code == AAByteCode::SETVAR || code == AAByteCode::SETFIELD || code == AAByteCode::GETFIELD;
	}

	bool __is_arithmetic_code(AAByteCode code) {
		return code == AAByteCode::ADD || code == AAByteCode::SUB || code == AAByteCode::MUL || code == AAByteCode::DIV || code == AAByteCode::MOD || code == AAByteCode::LE ||
			code == AAByteCode::LEQ || code == AAByteCode::GE || code == AAByteCode::GEQ || code == AAByteCode::NNEG ||	code == AAByteCode::CMPE || code == AAByteCode::CMPNE;
	}

	void dump_instructions(std::wstring file, std::vector<CompiledProcedure> procedures, std::vector<AAByteType> types, AAVM* pAAVM) {

		std::wofstream o = std::wofstream(file);

		if (o.is_open()) {

			o << L"BYTECODE PROCEDURES:\n";

			for (size_t i = 0; i < procedures.size(); i++) {

				CompiledProcedure proc = procedures[i];
				o << L"\tSTART PROCEDURE " << std::to_wstring(i) << " (" << procedures[i].name << L"):\n";
				if (proc.procEnvironment.constValues.Size() > 0) {
					o << "\tCONSTANTS:\t";
					for (size_t j = 0; j < proc.procEnvironment.constValues.Size(); j++) {
						AA_Literal lit = proc.procEnvironment.constValues.At(j);
						o << L"[" << getLitType(lit.tp) << L", " << getLitValue(lit) << L"]";
					}
					o << "\n";
				}
				if (proc.procEnvironment.identifiers.Size() > 0) {
					o << L"\tIDENTIFIERS:\n";
					for (size_t j = 0; j < proc.procEnvironment.identifiers.Size(); j++) {
						o << L"\t\t" << proc.procEnvironment.identifiers.At(j) << L";";
					}
					o << "\n";
				}
				o << L"\tOPERATIONS:\n";
				for (size_t j = 0; j < proc.procOperations.Size(); j++) {
					o << L"\t\t" << L"[" << std::right << std::setw(4) << std::setfill(L'0') << std::to_wstring(j) << L"]  ";
					o << std::left << std::setw(64) << std::setfill(L' ') << OpToWString(proc.procOperations.At(j));
					if (proc.procOperations.At(j).bc == AAByteCode::XCALL) {
						o << L";; Calls: " << pAAVM->GetBuiltinFuncByIndex(proc.procOperations.At(j).argValues[0]).name;
					} else if (proc.procOperations.At(j).bc == AAByteCode::CALL) {
						size_t target = proc.procOperations.At(j).argValues[0];
						if (target < procedures.size()) {
							o << L";; Calls: " << procedures.at(target).name;
						} else {
							o << L"t;; Calls: Uncompiled function [Uncaught compile error!]";
						}
					} else if (__is_arithmetic_code(proc.procOperations.At(j).bc)) {
						o << L";; primitive: " << aa::runtime::name_of_type((AAPrimitiveType)proc.procOperations.At(j).argValues[0]);
						o << L" [" << aa::runtime::size_of_type((AAPrimitiveType)proc.procOperations.At(j).argValues[0]) << L" bytes]";
					} else if (__is_primitive_code(proc.procOperations.At(j).bc)) {
						o << L";; primitive: " << aa::runtime::name_of_type((AAPrimitiveType)proc.procOperations.At(j).argValues[1]);
						o << L" [" << aa::runtime::size_of_type((AAPrimitiveType)proc.procOperations.At(j).argValues[1]) << L" bytes]";
					}
					o << "\n";
				}
				o << L"\tEND PROCEDURE;";
				if (i < procedures.size() - 1) {
					o << L"\n\n";
				}
			}

			o << L"\nEND BYTECODE PROCEDURES";

			// Dump the exported types
			dump_types(o, types);

		}

	}

}
