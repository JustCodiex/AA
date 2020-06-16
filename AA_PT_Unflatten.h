#pragma once
#include "AA_PT_NODE.h"

const int AA_PT_NODE_OUT_OF_BOUNDS_INDEX = 9999;

/// <summary>
/// Utility class for unflattening a vector containing parse tree nodes
/// </summary>
class AA_PT_Unflatten {

public:

	/*
	** Mathematical and language binding functions
	*/

	static void ApplyOrderOfOperationBindings(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Incorrect structure fix
	*/
	static void ApplySyntaxRules(std::vector<AA_PT_NODE*>& nodes);


	/*
	** Mathematical and language binding functions
	*/

	static void ApplyGroupings(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyAccessorBindings(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyFunctionBindings(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyUnaryBindings(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyArithemticRules(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyAssignmentOrder(std::vector<AA_PT_NODE*>& nodes);
	static void ApplyKeywordBindings(std::vector<AA_PT_NODE*>& nodes);

	static bool CanTreatKeywordAsIdentifier(AA_PT_NODE* node);
	static bool IsIdentifier(AA_PT_NODE* node, bool checkKeyword);

	/*
	** Flow control parsing
	*/
	static void ApplyStatementBindings(std::vector<AA_PT_NODE*>& nodes);

	/*
	** Open/Close operator pairing operations - eg. { <some content> } => BlockExpr, ( <some content> ) => Expr, [ <some content> ] => IndexerExpr
	*/
	static std::vector<AA_PT_NODE*> PairStatements(std::vector<AA_PT_NODE*>& nodes, int& index, AA_PT_NODE_TYPE open, AA_PT_NODE_TYPE close, AA_PT_NODE_TYPE contentType);

};
