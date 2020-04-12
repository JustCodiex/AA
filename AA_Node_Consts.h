#pragma once

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

#pragma endregion
