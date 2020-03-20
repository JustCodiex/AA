#include "AACFunction.h"

namespace aa {

	list<AAVal> ToArgumentList(stack<AAVal> args) {

		list<AAVal> argl;
		while (args.Size() > 0) {
			argl.Add(args.Pop());
		}

		return argl;

	}

}
