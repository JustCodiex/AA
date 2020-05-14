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
		inline bool Class_HasBody(const AA_PT_NODE*& pNode);

		/// <summary>
		/// Check if the node has a body
		/// </summary>
		/// <param name="pNode">AA_AST_NODE with type classdecl</param>
		/// <returns></returns>
		inline bool Class_HasBody(const AA_AST_NODE*& pNode);

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
		inline bool Function_HasBody(const AA_PT_NODE*& pNode);

		/// <summary>
		/// Check if the node has a body
		/// </summary>
		/// <param name="pNode">AA_AST_NODE with type fundecl</param>
		/// <returns></returns>
		inline bool Function_HasBody(const AA_AST_NODE*& pNode);

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
		/// <param name="pNode">AA_PT_NODE with type classdecl</param>
		/// <returns></returns>
		inline bool Enum_HasBody(const AA_PT_NODE*& pNode);

		/// <summary>
		/// Check if the node has a body
		/// </summary>
		/// <param name="pNode">AA_AST_NODE with type classdecl</param>
		/// <returns></returns>
		inline bool Enum_HasBody(const AA_AST_NODE*& pNode);

	}
}

#pragma endregion

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
