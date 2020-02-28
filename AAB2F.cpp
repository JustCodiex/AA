#include "AAB2F.h"
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

}
