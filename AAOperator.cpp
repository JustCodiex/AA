#include "AAOperator.h"

namespace aa {
	namespace op {

		bool IsAssignmentOperator(std::wstring str) {
			return str.compare(L"=") == 0 || str.compare(L"+=") == 0 || str.compare(L"-=") == 0
				|| str.compare(L"*=") == 0 || str.compare(L"/=") == 0 || str.compare(L"%=") == 0;
		}

	}
}
