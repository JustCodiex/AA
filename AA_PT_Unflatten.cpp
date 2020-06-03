#include "AA_PT_Unflatten.h"

#define REMOVE_NODE(i) delete nodes[i];nodes.erase(nodes.begin() + i)

void AA_PT_Unflatten::ApplyOrderOfOperationBindings(std::vector<AA_PT_NODE*>& nodes) {

	// Convert parenthesis blocks into singular expressions
	ApplyGroupings(nodes);

	// Apply bindings (eg. -1 => unary operation)
	ApplyUnaryBindings(nodes);

	// Apply mathematic rules So 5+5*5 = 30 and not 50
	ApplyArithemticRules(nodes);

	// Apply assignment order rule, so var x = 5+5 is treated as 5+5=x and not x=5, + 5
	ApplyAssignmentOrder(nodes);

	// Apply keyword bindings (so "var k = 5 - j match { ... }" will be parsed correctly)
	ApplyKeywordBindings(nodes);

}

void AA_PT_Unflatten::ApplyGroupings(std::vector<AA_PT_NODE*>& nodes) {

	// Pair parenthesis statements
	int i = 0;
	PairStatements(nodes, i, AA_PT_NODE_TYPE::parenthesis_start, AA_PT_NODE_TYPE::parenthesis_end, AA_PT_NODE_TYPE::expression);

	// Pair indexers
	i = 0;
	PairStatements(nodes, i, AA_PT_NODE_TYPE::indexer_start, AA_PT_NODE_TYPE::indexer_end, AA_PT_NODE_TYPE::indexing);

	// Pair possible function or keywords with arguments to their parenthesis (Because arguments to something binds the strongest)
	ApplyFunctionBindings(nodes);

	// Pair member access to their respective left->right associations
	ApplyAccessorBindings(nodes);

}

bool AA_PT_Unflatten::CanTreatKeywordAsIdentifier(AA_PT_NODE* node) {
	if (node->nodeType == AA_PT_NODE_TYPE::keyword) {
		return node->content.compare(L"this") == 0 || node->content.compare(L"base") == 0;
	} else {
		return false;
	}
}

bool AA_PT_Unflatten::IsIdentifier(AA_PT_NODE* node, bool checkKeyword) {
	if (node->nodeType == AA_PT_NODE_TYPE::identifier) {
		return true;
	} else {
		if (checkKeyword) {
			return CanTreatKeywordAsIdentifier(node);
		} else {
			return false;
		}
	}
}

bool AA_PT_Unflatten::IsAssignmentOperator(AA_PT_NODE* node) {
	if (node->nodeType == AA_PT_NODE_TYPE::binary_operation) {
		return node->content.compare(L"=") == 0 || node->content.compare(L"+=") == 0 || node->content.compare(L"-=") == 0 ||
			  node->content.compare(L"*=") == 0 || node->content.compare(L"/=") == 0 || node->content.compare(L"%=") == 0;
	} else {
		return false;
	}
}

void AA_PT_Unflatten::ApplyAccessorBindings(std::vector<AA_PT_NODE*>& nodes) {

	int i = 0;
	while (i < (int)nodes.size()) {

		if (nodes[i]->nodeType == AA_PT_NODE_TYPE::block) {

			// Apply recursively
			ApplyAccessorBindings(nodes[i]->childNodes);

		} else if (nodes[i]->nodeType == AA_PT_NODE_TYPE::accessor) {

			if (i - 1 >= 0 && (nodes[i - 1]->nodeType == AA_PT_NODE_TYPE::identifier || nodes[i - 1]->nodeType == AA_PT_NODE_TYPE::accessor || CanTreatKeywordAsIdentifier(nodes[i - 1]))) {

				if (i + 1 < (int)nodes.size() && nodes[i + 1]->nodeType == AA_PT_NODE_TYPE::identifier) {

					nodes[i]->childNodes.push_back(nodes[i - 1]);

					if (i + 2 < (int)nodes.size() && nodes[i + 2]->nodeType == AA_PT_NODE_TYPE::parameterlist) {

						AA_PT_NODE* rhsNode = new AA_PT_NODE(nodes[i + 1]->position);
						rhsNode->nodeType = AA_PT_NODE_TYPE::parameterlist;
						rhsNode->childNodes.push_back(nodes[i + 1]);
						rhsNode->childNodes.push_back(nodes[i + 2]);

						nodes[i]->childNodes.push_back(rhsNode);

						nodes.erase(nodes.begin() + i + 1, nodes.begin() + i + 3);

					} else {

						nodes[i]->childNodes.push_back(nodes[i + 1]);
						nodes.erase(nodes.begin() + i + 1);

					}

					nodes.erase(nodes.begin() + i - 1);
					i--;

				}

			}

		}

		i++;

	}

}

void AA_PT_Unflatten::ApplyFunctionBindings(std::vector<AA_PT_NODE*>& nodes) {

	for (size_t i = 0; i < nodes.size(); i++) {
		if (nodes[i]->nodeType == AA_PT_NODE_TYPE::expression) {
			ApplyFunctionBindings(nodes[i]->childNodes);
		}
	}

	int i = 0;

	while (i < (int)nodes.size()) {

		if (nodes[i]->nodeType == AA_PT_NODE_TYPE::block) {

			// Apply recursively
			ApplyFunctionBindings(nodes[i]->childNodes);

		} else if (nodes[i]->nodeType == AA_PT_NODE_TYPE::identifier) {
			if (i + 1 < (int)nodes.size() && nodes[i + 1]->nodeType == AA_PT_NODE_TYPE::expression) {	
				nodes[i + 1]->nodeType = AA_PT_NODE_TYPE::parameterlist;
				if (i - 1 > 0 && nodes[i - 1]->nodeType == AA_PT_NODE_TYPE::binary_operation) {
					ApplyFunctionBindings(nodes[i + 1]->childNodes);
					nodes[i]->childNodes.push_back(nodes[i + 1]);
					nodes.erase(nodes.begin() + i + 1);	
				}
			}
		}

		i++;

	}

}

void AA_PT_Unflatten::ApplyUnaryBindings(std::vector<AA_PT_NODE*>& nodes) {

	for (size_t i = 0; i < nodes.size(); i++) {
		if (nodes[i]->nodeType == AA_PT_NODE_TYPE::expression) {
			ApplyUnaryBindings(nodes[i]->childNodes);
		}
	}

	size_t index = 0;

	while (index < nodes.size()) {

		if (nodes[index]->nodeType == AA_PT_NODE_TYPE::block) {

			// Apply unary bindings recursively
			ApplyUnaryBindings(nodes[index]->childNodes);

			index++;

		} else if (nodes[index]->nodeType == AA_PT_NODE_TYPE::unary_operation) {

			AA_PT_NODE* unaryExp = new AA_PT_NODE(nodes[index]->position);
			unaryExp->nodeType = AA_PT_NODE_TYPE::expression;
			unaryExp->childNodes.push_back(nodes[index]);
			unaryExp->childNodes.push_back(nodes[index + 1]);

			nodes.erase(nodes.begin() + index + 1);
			nodes.erase(nodes.begin() + index);

			nodes.insert(nodes.begin() + index, unaryExp);

		} else {

			index++;

		}

	}

}

void AA_PT_Unflatten::ApplyArithemticRules(std::vector<AA_PT_NODE*>& nodes) {

	for (size_t i = 0; i < nodes.size(); i++) {
		if (nodes[i]->nodeType == AA_PT_NODE_TYPE::expression) {
			for (size_t j = 0; j < nodes[i]->childNodes.size(); j++) {
				ApplyArithemticRules(nodes[i]->childNodes[j]->childNodes);
			}
		}
	}

	size_t index = 0;

	while (index < nodes.size()) {

		if (nodes[index]->nodeType == AA_PT_NODE_TYPE::block) {

			// Apply arithmetic rules recursively
			ApplyArithemticRules(nodes[index]->childNodes);

			index++;

		} else if (nodes[index]->nodeType == AA_PT_NODE_TYPE::binary_operation) {

			if (nodes[index]->content == L"*" || nodes[index]->content == L"/" || nodes[index]->content == L"%") {

				AA_PT_NODE* binOpNode = new AA_PT_NODE(nodes[index]->position);
				binOpNode->nodeType = AA_PT_NODE_TYPE::expression;
				binOpNode->childNodes.push_back(nodes[index - 1]);
				binOpNode->childNodes.push_back(nodes[index]);
				binOpNode->childNodes.push_back(nodes[index + 1]);

				nodes.erase(nodes.begin() + index + 1);
				nodes.erase(nodes.begin() + index);

				nodes.insert(nodes.begin() + index, binOpNode);

				nodes.erase(nodes.begin() + index - 1);

			} else {
				index++;
			}

		} else {
			index++;
		}

	}

}

void AA_PT_Unflatten::ApplyAssignmentOrder(std::vector<AA_PT_NODE*>& nodes) {

	size_t index = 0;

	while (index < nodes.size()) {

		if (nodes[index]->nodeType == AA_PT_NODE_TYPE::block) {

			// Apply the assignment order recursively
			ApplyAssignmentOrder(nodes[index]->childNodes);

		} else if (IsAssignmentOperator(nodes[index])) {

			size_t index2 = index;

			while (index2 < nodes.size()) {
				if (nodes[index2]->nodeType == AA_PT_NODE_TYPE::seperator) {
					break;
				} else {
					index2++;
				}
			}

			AA_PT_NODE* rhs = new AA_PT_NODE(nodes[index]->position);
			rhs->nodeType = AA_PT_NODE_TYPE::expression;
			rhs->childNodes = std::vector<AA_PT_NODE*>(nodes.begin() + index + 1, nodes.begin() + index2);

			nodes.erase(nodes.begin() + index + 1, nodes.begin() + index2);
			nodes.insert(nodes.begin() + index + 1, rhs);

		}

		index++;

	}

}

void AA_PT_Unflatten::ApplyStatementBindings(std::vector<AA_PT_NODE*>& nodes) {

	size_t index = 0;

	while (index < nodes.size()) {

		if (nodes[index]->nodeType == AA_PT_NODE_TYPE::block) {

			// Apply recursively
			ApplyStatementBindings(nodes[index]->childNodes);

		} else {

			size_t index2 = index;

			while (index2 < nodes.size()) {
				if (nodes[index2]->nodeType == AA_PT_NODE_TYPE::seperator) {
					index2++;
					break;
				} else {
					index2++;
				}
			}

			AA_PT_NODE* rhs = new AA_PT_NODE(nodes[index]->position);
			rhs->nodeType = AA_PT_NODE_TYPE::expression;
			rhs->childNodes = std::vector<AA_PT_NODE*>(nodes.begin() + index, nodes.begin() + index2);

			nodes.erase(nodes.begin() + index, nodes.begin() + index2);
			nodes.insert(nodes.begin() + index, rhs);

		}

		index++;

	}

}

void AA_PT_Unflatten::ApplyKeywordBindings(std::vector<AA_PT_NODE*>& nodes) {

	size_t index = 0;

	while (index < nodes.size()) {

		if (nodes[index]->nodeType == AA_PT_NODE_TYPE::block || nodes[index]->nodeType == AA_PT_NODE_TYPE::expression) { // TODO: Allow for more cases here...

			// Apply recursively
			ApplyKeywordBindings(nodes[index]->childNodes);

		} else if (IsIdentifier(nodes[index], true)) { // Is valid identifier (or reserved keyword to be treated as identifier)

			if (index + 2 < nodes.size() && nodes[index + 1]->nodeType == AA_PT_NODE_TYPE::keyword && nodes[index + 1]->content.compare(L"match") == 0 &&
				nodes[index+2]->nodeType == AA_PT_NODE_TYPE::block) {

				// Create expression node to bind these together
				AA_PT_NODE* expNode = new AA_PT_NODE(nodes[index + 1]->position);
				expNode->nodeType = AA_PT_NODE_TYPE::expression;
				expNode->childNodes.push_back(nodes[index]);
				expNode->childNodes.push_back(nodes[index+1]);
				expNode->childNodes.push_back(nodes[index+2]);

				// Assign node
				nodes[index] = expNode;

				// Remove subnodes from current node list
				nodes.erase(nodes.begin() + index + 1, nodes.begin() + index + 3);

			}

		}

		index++;

	}

}

std::vector<AA_PT_NODE*> AA_PT_Unflatten::PairStatements(std::vector<AA_PT_NODE*>& nodes, int& index, AA_PT_NODE_TYPE open, AA_PT_NODE_TYPE close, AA_PT_NODE_TYPE contentType) {

	std::vector<AA_PT_NODE*> subNodes;

	while (index < (int)nodes.size()) {

		if (nodes[index]->nodeType == open) {

			REMOVE_NODE(index);

			int pStart = index;

			AA_PT_NODE* expNode = new AA_PT_NODE(nodes[index]->position);
			expNode->nodeType = contentType;
			expNode->childNodes = PairStatements(nodes, index, open, close, contentType);

			nodes.erase(nodes.begin() + pStart, nodes.begin() + pStart + expNode->childNodes.size());
			nodes.insert(nodes.begin() + pStart, expNode);

			index = pStart;

		} else if (nodes[index]->nodeType == close) {

			REMOVE_NODE(index);

			return subNodes;

		} else {

			// Recursive call through blocks
			if (nodes[index]->nodeType == AA_PT_NODE_TYPE::block) {

				int j = 0;
				PairStatements(nodes[index]->childNodes, j, open, close, contentType);

			}

			subNodes.push_back(nodes[index]);
			index++;

		}

	}

	return subNodes;

}

void AA_PT_Unflatten::ApplySyntaxRules(std::vector<AA_PT_NODE*>& nodes) {

	// Dummy index
	int index = 0;

	// Create blocks (highest priority)
	PairStatements(nodes, index, AA_PT_NODE_TYPE::block_start, AA_PT_NODE_TYPE::block_end, AA_PT_NODE_TYPE::block);

	// Apply OOP bindings
	ApplyOrderOfOperationBindings(nodes);

	// Apply Statement bindings
	ApplyStatementBindings(nodes);

}
