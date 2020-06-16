#include "AA_Node_Consts.h"
#include "AA_AST_NODE.h"
#include "AA_PT_NODE.h"

namespace aa {
	namespace parsing {

		bool Class_HasBody(const AA_PT_NODE* pNode) {
			return pNode->childNodes.size() > AA_NODE_CLASSNODE_BODY;
		}

		bool Class_HasBody(const AA_AST_NODE* pNode) {
			return pNode->expressions.size() > AA_NODE_CLASSNODE_BODY;
		}

		bool Function_HasBody(const AA_PT_NODE* pNode) {
			return pNode->childNodes.size() > AA_NODE_FUNNODE_BODY;
		}

		bool Function_HasBody(const AA_AST_NODE* pNode) {
			return pNode->expressions.size() > AA_NODE_FUNNODE_BODY;
		}

		bool Enum_HasBody(const AA_PT_NODE* pNode) {
			return pNode->childNodes.size() > AA_NODE_ENUMNODE_BODY;
		}

		bool Enum_HasBody(const AA_AST_NODE* pNode) {
			return pNode->expressions.size() > AA_NODE_ENUMNODE_BODY;
		}

		bool Var_HasModifiers(const AA_PT_NODE* pNode) {
			return pNode->childNodes.size() > AA_NODE_VARDECL_MODIFIERLIST;
		}

		bool Var_HasModifiers(const AA_AST_NODE* pNode) {
			return pNode->expressions.size() > AA_NODE_VARDECL_MODIFIERLIST;
		}

	}
}
