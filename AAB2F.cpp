#include "AAB2F.h"
#include "AAVM.h"
#include <fstream>

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

	std::wstring OpToWString(AAC::CompiledAbstractExpression CAE) {
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
		case AAByteCode::RET:
			output = L"RET";
			break;
		case AAByteCode::SETVAR:
			output = L"SETVAR";
			break;
		case AAByteCode::SUB:
			output = L"SUB";
			break;
		case AAByteCode::HALLOC:
			output = L"HALLOC";
			break;
		case AAByteCode::SALLOC:
			output = L"SALLOC";
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
		default:
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

	void dump_instructions(std::wstring file, std::vector<AAC::CompiledProcedure> procedures, AAVM* pAAVM) {

		std::wofstream o = std::wofstream(file);

		if (o.is_open()) {

			for (size_t i = 0; i < procedures.size(); i++) {

				AAC::CompiledProcedure proc = procedures[i];
				o << L"START PROCEDURE " << std::to_wstring(i) << " (" << procedures[i].node->content << L"):\n";
				o << "CONSTANTS:";
				for (size_t j = 0; j < proc.procEnvironment.constValues.Size(); j++) {
					AA_Literal lit = proc.procEnvironment.constValues.At(j);
					o << L"\t[" << getLitType(lit.tp) << L", " << getLitValue(lit) << L"]";
				}
				o << L"\nIDENTIFIERS:\n";
				for (size_t j = 0; j < proc.procEnvironment.identifiers.Size(); j++) {
					o << L"\t" << proc.procEnvironment.identifiers.At(j) << L";";
				}
				o << L"\nOPERATIONS:\n";
				for (size_t j = 0; j < proc.procOperations.Size(); j++) {
					o << L"\t" << L"[" << std::to_wstring(j) << L"]\t" << OpToWString(proc.procOperations.At(j));
					if (proc.procOperations.At(j).bc == AAByteCode::XCALL) {
						o << L"\t\t;; Calls: " << pAAVM->GetBuiltinFuncByIndex(proc.procOperations.At(j).argValues[0]).name;
					} else if (proc.procOperations.At(j).bc == AAByteCode::CALL) {
						o << L"\t\t;; Calls: " << procedures.at(proc.procOperations.At(j).argValues[0]).node->content;
					}
					o << "\n";
				}
				o << L"END PROCEDURE;\n\n";

			}

		}

	}

}
