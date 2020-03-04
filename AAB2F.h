#pragma once
#include "AAC.h"

namespace aa {

	// Dumps bytecode to a file
	void dump_bytecode(std::wstring file, AAC_Out bytecode);

	// Dumps operations in a readable format
	void dump_instructions(std::wstring file, std::vector<AAC::CompiledProcedure> procedures);

}
