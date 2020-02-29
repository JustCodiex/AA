#include "AA_PT.h"
#include <map>

AA_PT::AA_PT(std::vector<AALexicalResult> lexResult) {

	// Convert lexical analysis to AA_PT_NODEs
	std::vector<AA_PT_NODE*> aa_pt_nodes = ToNodes(lexResult);

	// Apply mathematical arithmetic and binding rules
	this->PrioritizeBinding(aa_pt_nodes);

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
		default:
			break;
		}

		node->content = lexResult[i].content;

		aa_pt_nodes.push_back(node);

	}

	return aa_pt_nodes;

}

AA_PT_NODE_TYPE AA_PT::GetSeperatorType(std::wstring val) {
	if (val == L"(") {
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
		AA_PT_NODE_TYPE::parenthesis_end,
	};

	if (nodes.size() > 0) {
		return std::find(binop.begin(), binop.end(), nodes[nodes.size() - 1]->nodeType) == binop.end();
	}

	return true;

}

#define REMOVE_NODE(i) delete nodes[i];nodes.erase(nodes.begin() + i)

AA_PT_NODE* AA_PT::CreateTree(std::vector<AA_PT_NODE*>& nodes, int from) {

	size_t nodeIndex = from;
	AA_PT_NODE* root = 0;

	while (nodes.size() > 1 && nodeIndex < nodes.size()) {
		switch (nodes[nodeIndex]->nodeType) {
		case AA_PT_NODE_TYPE::binary_operation:

			nodes[nodeIndex - 1]->parent = nodes[nodeIndex];
			nodes[nodeIndex + 1]->parent = nodes[nodeIndex];

			nodes[nodeIndex]->childNodes.push_back(nodes[nodeIndex - 1]);
			nodes[nodeIndex]->childNodes.push_back(CreateExpressionTree(nodes, nodeIndex + 1));

			nodes.erase(nodes.begin() + nodeIndex + 1);
			nodes.erase(nodes.begin() + nodeIndex - 1);

			break;
		case AA_PT_NODE_TYPE::unary_operation:

			nodes[nodeIndex + 1]->parent = nodes[nodeIndex];
			nodes[nodeIndex]->childNodes.push_back(CreateExpressionTree(nodes, nodeIndex + 1));
			nodes.erase(nodes.begin() + nodeIndex + 1);

			nodeIndex++;

			break;
		case AA_PT_NODE_TYPE::seperator:
			if (nodes[nodeIndex]->content == L";") {
				REMOVE_NODE(nodeIndex);
			}
			break;
		case AA_PT_NODE_TYPE::intliteral:
			nodeIndex++;
			break;
		case AA_PT_NODE_TYPE::expression: {
			nodes[nodeIndex] = CreateExpressionTree(nodes, nodeIndex);
			nodeIndex++;
			break;
		}
		default:
			break;
		}

	}

	root = nodes.at(0);

	return root;

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

void AA_PT::Parenthesise(std::vector<AA_PT_NODE*>& nodes) {

	int i = 0;
	this->Parenthesise(nodes, i);

}

std::vector<AA_PT_NODE*> AA_PT::Parenthesise(std::vector<AA_PT_NODE*>& nodes, int& index) {

	std::vector<AA_PT_NODE*> subNodes;

	while (index < (int)nodes.size()) {

		if (nodes[index]->nodeType == AA_PT_NODE_TYPE::parenthesis_start) {

			REMOVE_NODE(index);

			int pStart = index;

			AA_PT_NODE* expNode = new AA_PT_NODE;
			expNode->nodeType = AA_PT_NODE_TYPE::expression;
			expNode->childNodes = Parenthesise(nodes, index);

			nodes.erase(nodes.begin() + pStart, nodes.begin() + pStart + expNode->childNodes.size());
			nodes.insert(nodes.begin() + pStart, expNode);

			index = pStart;

		} else if (nodes[index]->nodeType == AA_PT_NODE_TYPE::parenthesis_end) {
		
			REMOVE_NODE(index);

			return subNodes;

		} else {
			subNodes.push_back(nodes[index]);
			index++;
		}
		
	}

	return subNodes;

}

void AA_PT::PrioritizeBinding(std::vector<AA_PT_NODE*>& nodes) {

	// Convert parenthesis blocks into singular expressions
	this->Parenthesise(nodes);

	// Apply bindings (eg. -1 => unary operation)
	this->ApplyBindings(nodes);

	// Apply mathematic rules So 5+5*5 = 30 and not 50
	this->ApplyArithemticRules(nodes);

}

void AA_PT::ApplyBindings(std::vector<AA_PT_NODE*>& nodes) {

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

		} else if (nodes[index]->nodeType == AA_PT_NODE_TYPE::expression) {

			index++;

		} else {

			index++;

		}

	}

}

void AA_PT::ApplyArithemticRules(std::vector<AA_PT_NODE*>& nodes) {



}
