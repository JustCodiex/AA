#pragma once

struct AA_AST_NODE; // forward decleration of AST node
struct AA_PT_NODE; // forward decleration of PT node

#pragma region CLASSDECL CONSTS

/// <summary>
/// Fixed index for fetching the modifier list of a AA_PT_NODE with type classdecleration
/// </summary>
const int AA_NODE_CLASSNODE_MODIFIER = 0;

/// <summary>
/// Fixed index for fetching the inheritance list of a AA_PT_NODE with type classdecleration
/// </summary>
const int AA_NODE_CLASSNODE_INHERITANCE = 1;

/// <summary>
/// Fixed index for fetching the body of a AA_PT_NODE with type classdecleration
/// </summary>
const int AA_NODE_CLASSNODE_BODY = 2;

namespace aa {
	namespace parsing {

		/// <summary>
		/// Check if the node has a body
		/// </summary>
		/// <param name="pNode">AA_PT_NODE with type classdecl</param>
		/// <returns></returns>
		bool Class_HasBody(const AA_PT_NODE* pNode);

		/// <summary>
		/// Check if the node has a body
		/// </summary>
		/// <param name="pNode">AA_AST_NODE with type classdecl</param>
		/// <returns></returns>
		bool Class_HasBody(const AA_AST_NODE* pNode);

	}
}

#pragma endregion

#pragma region FUNDECL CONSTS

/// <summary>
/// Fixed index for fetching the returntype of a AA_PT_NODE with type fundecleration
/// </summary>
const int AA_NODE_FUNNODE_RETURNTYPE = 0;

/// <summary>
/// Fixed index for fetching the argument list of a AA_PT_NODE with type fundecleration
/// </summary>
const int AA_NODE_FUNNODE_ARGLIST = 1;

/// <summary>
/// Fixed index for fetching the modifier list of a AA_PT_NODE with type fundecleration
/// </summary>
const int AA_NODE_FUNNODE_MODIFIER = 2;

/// <summary>
/// Fixed index for fetching the body of a AA_PT_NODE with type fundecleration
/// </summary>
const int AA_NODE_FUNNODE_BODY = 3;

namespace aa {
	namespace parsing {

		/// <summary>
		/// Check if the node has a body
		/// </summary>
		/// <param name="pNode">AA_PT_NODE with type fundecl</param>
		/// <returns></returns>
		bool Function_HasBody(const AA_PT_NODE* pNode);

		/// <summary>
		/// Check if the node has a body
		/// </summary>
		/// <param name="pNode">AA_AST_NODE with type fundecl</param>
		/// <returns></returns>
		bool Function_HasBody(const AA_AST_NODE* pNode);

	}
}

#pragma endregion

#pragma region ENUMDECL CONSTS

/// <summary>
/// Fixed index for fetching the value node of a AA_PT_NODE with type enumdecleration
/// </summary>
const int AA_NODE_ENUMNODE_VALUES = 0;

/// <summary>
/// Fixed index for fetching the body node of a AA_PT_NODE with type enumdecleration
/// </summary>
const int AA_NODE_ENUMNODE_BODY = 1;

namespace aa {
	namespace parsing {

		/// <summary>
		/// Check if the node has a body
		/// </summary>
		/// <param name="pNode">AA_PT_NODE with type enumdecl</param>
		/// <returns></returns>
		bool Enum_HasBody(const AA_PT_NODE* pNode);

		/// <summary>
		/// Check if the node has a body
		/// </summary>
		/// <param name="pNode">AA_AST_NODE with type enumdecl</param>
		/// <returns></returns>
		bool Enum_HasBody(const AA_AST_NODE* pNode);

	}
}

#pragma endregion

#pragma region VARDECL CONSTS

/// <summary>
/// Fixed index for fetching the type node of a AA_PT_NODE with type vardeclaration
/// </summary>
const int AA_NODE_VARDECL_TYPE = 0;

/// <summary>
/// Fixed index for fetching the identifier node of a AA_PT_NODE with type vardeclaration
/// </summary>
const int AA_NODE_VARDECL_IDENTIFIER = 1;

/// <summary>
/// Fixed index for fetching the modifierlist node of a AA_PT_NODE with type vardeclaration
/// </summary>
const int AA_NODE_VARDECL_MODIFIERLIST = 2;

namespace aa {
	namespace parsing {

		/// <summary>
		/// Check if the node has modifiers
		/// </summary>
		/// <param name="pNode">AA_PT_NODE with type enumdecl</param>
		/// <returns></returns>
		bool Var_HasModifiers(const AA_PT_NODE* pNode);

		/// <summary>
		/// Check if the node has modifiers
		/// </summary>
		/// <param name="pNode">AA_AST_NODE with type enumdecl</param>
		/// <returns></returns>
		bool Var_HasModifiers(const AA_AST_NODE* pNode);

	}
}

#pragma region BRANCH CONSTS

/// <summary>
/// Fixed index for fetching the condition node of AA_NODE with type ifstatement
/// </summary>
const int AA_NODE_IF_CONDITION = 0;

/// <summary>
/// Fixed index for fetching the body node of AA_NODE with type ifstatement
/// </summary>
const int AA_NODE_IF_BODY = 1;

#pragma endregion

#pragma region FOR CONSTS

/// <summary>
/// Fixed index for fetching the init node of a for-loop node
/// </summary>
const int AA_NODE_FOR_INIT = 0;

/// <summary>
/// Fixed index for fetching the condition of a for-loop node
/// </summary>
const int AA_NODE_FOR_CONDITION = 1;

/// <summary>
/// Fixed index for fetching the afterthought of a for-loop node
/// </summary>
const int AA_NODE_FOR_AFTERTHOUGHT = 2;

/// <summary>
/// Fixed index for fetching the body of a for-loop node
/// </summary>
const int AA_NODE_FOR_BODY = 3;

#pragma endregion
