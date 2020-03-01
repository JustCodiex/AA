#include "AA_PT.h"
#include <map>

AA_PT::AA_PT(std::vector<AALexicalResult> lexResult) {

	// Convert lexical analysis to AA_PT_NODEs
	std::vector<AA_PT_NODE*> aa_pt_nodes = ToNodes(lexResult);

	// Apply mathematical arithmetic and binding rules
	this->ApplyOrderOfOperationBindings(aa_pt_nodes);

	// Apply flow control
	this->ApplyFlowControlBindings(aa_pt_nodes);

	// Create tree from AA_PT_NODEs
	m_root = this->CreateTree(aa_pt_nodes, 0);

}

std::vector<AA_PT_NODE*> AA_PT::ToNodes(std::vector<AALexicalResult> lexResult) {

	std::vector<AA_PT_NODE*> aa_pt_nodes;

	for (size_t i = 0; i < lexResult.size(); i++) {

		AA_PT_NODE* node = new AA_PT_NODE;

		switch (lexResult[i].token) {
		case AAToken::intlit:
			node->nodeType = AA_PT_NODE_TYPE::intliteral;
			break;
		case AAToken::floatlit:
			node->nodeType = AA_PT_NODE_TYPE::floatliteral;
			break;
		case AAToken::OP:
			if (!IsUnaryOperator(aa_pt_nodes)) {
				node->nodeType = AA_PT_NODE_TYPE::binary_operation;
			} else {
				node->nodeType = AA_PT_NODE_TYPE::unary_operation;
			}
			break;
		case AAToken::seperator:
			node->nodeType = GetSeperatorType(lexResult[i].content);
			break;
		case AAToken::identifier:
			node->nodeType = AA_PT_NODE_TYPE::identifier;
			break;
		case AAToken::keyword:
			if (IsBoolKeyword(lexResult[i].content)) {
				node->nodeType = AA_PT_NODE_TYPE::booliterral;
			} else {
				node->nodeType = AA_PT_NODE_TYPE::keyword;
			}
			break;
		default:
			break;
		}

		node->content = lexResult[i].content;

		aa_pt_nodes.push_back(node);

	}

	return aa_pt_nodes;

}

AA_PT_NODE_TYPE AA_PT::GetSeperatorType(std::wstring val) {
	if (val == L"{") {
		return AA_PT_NODE_TYPE::block_start;
	} else if (val == L"}") {
		return AA_PT_NODE_TYPE::block_end;
	} else if(val == L"(") {
		return AA_PT_NODE_TYPE::parenthesis_start;
	} else if (val == L")") {
		return AA_PT_NODE_TYPE::parenthesis_end;
	} else {
		return AA_PT_NODE_TYPE::seperator;
	}
}

bool AA_PT::IsUnaryOperator(std::vector<AA_PT_NODE*> nodes) {

	std::vector<AA_PT_NODE_TYPE> binop = {
		AA_PT_NODE_TYPE::intliteral,
		AA_PT_NODE_TYPE::floatliteral,
		AA_PT_NODE_TYPE::charliteral,
		AA_PT_NODE_TYPE::stringliteral,
		AA_PT_NODE_TYPE::parenthesis_end,
		AA_PT_NODE_TYPE::identifier,
	};

	if (nodes.size() > 0) {
		return std::find(binop.begin(), binop.end(), nodes[nodes.size() - 1]->nodeType) == binop.end();
	}

	return true;

}

bool AA_PT::IsBoolKeyword(std::wstring keyword) {
	return keyword == L"true" || keyword == L"false";
}

#define REMOVE_NODE(i) delete nodes[i];nodes.erase(nodes.begin() + i)

AA_PT_NODE* AA_PT::CreateTree(std::vector<AA_PT_NODE*>& nodes, int from) {

	size_t nodeIndex = from;
	AA_PT_NODE* root = 0;

	if (nodes.size() == 1 && nodes[0]->nodeType == AA_PT_NODE_TYPE::expression) {
		return CreateExpressionTree(nodes, 0);
	}

	if (nodes.size() > 0 && (nodes[0]->nodeType == AA_PT_NODE_TYPE::block)) {
		HandleTreeCase(nodes, nodeIndex);
	} else {
		while (nodes.size() > 1 && nodeIndex < nodes.size()) {
			HandleTreeCase(nodes, nodeIndex);
		}
	}

	root = nodes.at(0);

	return root;

}

void AA_PT::HandleTreeCase(std::vector<AA_PT_NODE*>& nodes, size_t& nodeIndex) {

	switch (nodes[nodeIndex]->nodeType) {
	case AA_PT_NODE_TYPE::binary_operation:

		nodes[nodeIndex - 1]->parent = nodes[nodeIndex];
		nodes[nodeIndex + 1]->parent = nodes[nodeIndex];

		nodes[nodeIndex]->childNodes.push_back(nodes[nodeIndex - 1]);
		nodes[nodeIndex]->childNodes.push_back(this->CreateExpressionTree(nodes, nodeIndex + 1));

		nodes.erase(nodes.begin() + nodeIndex + 1);
		nodes.erase(nodes.begin() + nodeIndex - 1);

		break;
	case AA_PT_NODE_TYPE::unary_operation:

		nodes[nodeIndex + 1]->parent = nodes[nodeIndex];
		nodes[nodeIndex]->childNodes.push_back(this->CreateExpressionTree(nodes, nodeIndex + 1));
		nodes.erase(nodes.begin() + nodeIndex + 1);

		nodeIndex++;

		break;
	case AA_PT_NODE_TYPE::seperator:
		if (nodes[nodeIndex]->content == L";") {
			REMOVE_NODE(nodeIndex);
		}
		break;
	case AA_PT_NODE_TYPE::identifier:
		if (nodeIndex > 0 && nodes[nodeIndex-1]->nodeType == AA_PT_NODE_TYPE::identifier) {
			nodes[nodeIndex - 1] = this->CreateVariableDecl(nodes, nodeIndex - 1);
		} else {
			nodeIndex++;
		}
		break;
	case AA_PT_NODE_TYPE::intliteral:
	case AA_PT_NODE_TYPE::floatliteral:
	case AA_PT_NODE_TYPE::booliterral:
	case AA_PT_NODE_TYPE::stringliteral:
	case AA_PT_NODE_TYPE::charliteral:
		nodeIndex++;
		break;
	case AA_PT_NODE_TYPE::keyword:
		if (nodes[nodeIndex]->content == L"var") {
			nodes[nodeIndex] = this->CreateVariableDecl(nodes, nodeIndex);
		}
		nodeIndex++;
		break;
	case AA_PT_NODE_TYPE::expression: {
		nodes[nodeIndex] = this->CreateExpressionTree(nodes, nodeIndex);
		nodeIndex++;
		break;
	}
	case AA_PT_NODE_TYPE::block: {
		for (size_t i = 0; i < nodes[nodeIndex]->childNodes.size(); i++) {
			if (nodes[nodeIndex]->childNodes[i]->nodeType == AA_PT_NODE_TYPE::expression) {
				nodes[nodeIndex]->childNodes[i] = this->CreateTree(nodes[nodeIndex]->childNodes[i]->childNodes, 0);
			} else {
				printf("Uhhh....");
			}
		}
		nodeIndex++;
		break;
	}
	default:
		break;
	}

}

AA_PT_NODE* AA_PT::CreateExpressionTree(std::vector<AA_PT_NODE*>& nodes, int from) {

	if (nodes[from]->nodeType == AA_PT_NODE_TYPE::expression) {

		AA_PT_NODE* exp = CreateTree(nodes[from]->childNodes, 0); // Create a tree for the expression
		while (exp->nodeType == AA_PT_NODE_TYPE::expression) { // As long as we have a single expression here, we break it down to smaller bits, incase we get input like (((5+5))).
			exp = CreateTree(exp->childNodes, 0); // Create the tree for the sub expression
		}

		return exp;

	} else {

		return nodes[from];

	}

}

AA_PT_NODE* AA_PT::CreateVariableDecl(std::vector<AA_PT_NODE*>& nodes, int from) {

	if (nodes[from + 1]->nodeType != AA_PT_NODE_TYPE::identifier) {
		printf("Err: Expected identifier");
	}

	AA_PT_NODE* varDeclExp = new AA_PT_NODE;
	varDeclExp->nodeType = AA_PT_NODE_TYPE::decleration;
	varDeclExp->childNodes.push_back(nodes[from]);
	varDeclExp->childNodes.push_back(nodes[from+1]);

	nodes.erase(nodes.begin() + from + 1);

	return varDeclExp;

}

void AA_PT::ApplyOrderOfOperationBindings(std::vector<AA_PT_NODE*>& nodes) {

	// Convert parenthesis blocks into singular expressions
	this->ApplyGroupings(nodes);

	// Apply bindings (eg. -1 => unary operation)
	this->ApplyUnaryBindings(nodes);

	// Apply mathematic rules So 5+5*5 = 30 and not 50
	this->ApplyArithemticRules(nodes);

	// Apply assignment order rule, so var x = 5+5 is treated as 5+5=x and not x=5, + 5
	this->ApplyAssignmentOrder(nodes);

}

void AA_PT::ApplyGroupings(std::vector<AA_PT_NODE*>& nodes) {

	int i = 0;
	this->PairStatements(nodes, i, AA_PT_NODE_TYPE::parenthesis_start, AA_PT_NODE_TYPE::parenthesis_end, AA_PT_NODE_TYPE::expression);

}

void AA_PT::ApplyUnaryBindings(std::vector<AA_PT_NODE*>& nodes) {

	for (size_t i = 0; i < nodes.size(); i++) {
		if (nodes[i]->nodeType == AA_PT_NODE_TYPE::expression) {
			ApplyUnaryBindings(nodes[i]->childNodes);
		}
	}

	size_t index = 0;

	while (index < nodes.size()) {

		if (nodes[index]->nodeType == AA_PT_NODE_TYPE::unary_operation) {

			AA_PT_NODE* unaryExp = new AA_PT_NODE;
			unaryExp->nodeType = AA_PT_NODE_TYPE::expression;
			unaryExp->childNodes.push_back(nodes[index]);
			unaryExp->childNodes.push_back(nodes[index+1]);

			nodes.erase(nodes.begin() + index+1);
			nodes.erase(nodes.begin() + index);

			nodes.insert(nodes.begin() + index, unaryExp);

		} else {

			index++;

		}

	}

}

void AA_PT::ApplyArithemticRules(std::vector<AA_PT_NODE*>& nodes) {

	for (size_t i = 0; i < nodes.size(); i++) {
		if (nodes[i]->nodeType == AA_PT_NODE_TYPE::expression) {
			for (size_t j = 0; j < nodes[i]->childNodes.size(); j++) {
				ApplyArithemticRules(nodes[i]->childNodes[j]->childNodes);
			}
		}
	}

	size_t index = 0;

	while (index < nodes.size()) {

		if (nodes[index]->nodeType == AA_PT_NODE_TYPE::binary_operation) {

			if (nodes[index]->content == L"*" || nodes[index]->content == L"/" || nodes[index]->content == L"%") {

				AA_PT_NODE* binOpNode = new AA_PT_NODE;
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

void AA_PT::ApplyAssignmentOrder(std::vector<AA_PT_NODE*>& nodes) {

	size_t index = 0;

	while (index < nodes.size()) {

		if (nodes[index]->nodeType == AA_PT_NODE_TYPE::binary_operation) {

			if (nodes[index]->content == L"=") {

				size_t index2 = index;

				while (index2 < nodes.size()) {
					if (nodes[index2]->nodeType == AA_PT_NODE_TYPE::seperator) {
						break;
					} else {
						index2++;
					}
				}

				AA_PT_NODE* rhs = new AA_PT_NODE;
				rhs->nodeType = AA_PT_NODE_TYPE::expression;
				rhs->childNodes = std::vector<AA_PT_NODE*>(nodes.begin() + index + 1, nodes.begin() + index2);

				nodes.erase(nodes.begin() + index + 1, nodes.begin() + index2);
				nodes.insert(nodes.begin() + index + 1, rhs);

			}

		}

		index++;

	}

}

void AA_PT::ApplyFlowControlBindings(std::vector<AA_PT_NODE*>& nodes) {

	int index = 0;
	PairStatements(nodes, index, AA_PT_NODE_TYPE::block_start, AA_PT_NODE_TYPE::block_end, AA_PT_NODE_TYPE::block);

	ApplyStatementBindings(nodes);

}

void AA_PT::ApplyStatementBindings(std::vector<AA_PT_NODE*>& nodes) {

	size_t index = 0;

	while (index < nodes.size()) {

		if (nodes[index]->nodeType == AA_PT_NODE_TYPE::block) {
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

			AA_PT_NODE* rhs = new AA_PT_NODE;
			rhs->nodeType = AA_PT_NODE_TYPE::expression;
			rhs->childNodes = std::vector<AA_PT_NODE*>(nodes.begin() + index, nodes.begin() + index2);

			nodes.erase(nodes.begin() + index, nodes.begin() + index2);
			nodes.insert(nodes.begin() + index, rhs);

		}

		index++;

	}

}

std::vector<AA_PT_NODE*> AA_PT::PairStatements(std::vector<AA_PT_NODE*>& nodes, int& index, AA_PT_NODE_TYPE open, AA_PT_NODE_TYPE close, AA_PT_NODE_TYPE contentType) {

	std::vector<AA_PT_NODE*> subNodes;

	while (index < (int)nodes.size()) {

		if (nodes[index]->nodeType == open) {

			REMOVE_NODE(index);

			int pStart = index;

			AA_PT_NODE* expNode = new AA_PT_NODE;
			expNode->nodeType = contentType;
			expNode->childNodes = PairStatements(nodes, index, open, close, contentType);

			nodes.erase(nodes.begin() + pStart, nodes.begin() + pStart + expNode->childNodes.size());
			nodes.insert(nodes.begin() + pStart, expNode);

			index = pStart;

		} else if (nodes[index]->nodeType == close) {

			REMOVE_NODE(index);

			return subNodes;

		} else {
			subNodes.push_back(nodes[index]);
			index++;
		}

	}

	return subNodes;

}
