#include "AA_PT.h"
#include <map>

AA_PT::AA_PT(std::vector<AALexicalResult> lexResult) {

	// Convert lexical analysis to AA_PT_NODEs
	std::vector<AA_PT_NODE*> aa_pt_nodes = ToNodes(lexResult);

	// Convert parenthesis blocks into singular expressions
	this->Parenthesise(aa_pt_nodes);

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
				node->nodeType = AA_PT_NODE_TYPE::unary_operator;
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

	return false;

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

			if (nodes[nodeIndex+1]->nodeType == AA_PT_NODE_TYPE::expression) {
				nodes[nodeIndex]->childNodes.push_back(CreateTree(nodes[nodeIndex+1]->childNodes, 0));
			} else {
				nodes[nodeIndex]->childNodes.push_back(nodes[nodeIndex + 1]);
			}

			nodes.erase(nodes.begin() + nodeIndex + 1);
			nodes.erase(nodes.begin() + nodeIndex - 1);

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
			AA_PT_NODE* exp = CreateTree(nodes[nodeIndex]->childNodes, 0);
			while (exp->nodeType == AA_PT_NODE_TYPE::expression) {
				exp = CreateTree(exp->childNodes, 0);
			}
			nodes[nodeIndex] = exp;
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

void AA_PT::Parenthesise(std::vector<AA_PT_NODE*>& nodes) {

	int i = 0;
	this->Parenthesise(nodes, i);

}

std::vector<AA_PT_NODE*> AA_PT::Parenthesise(std::vector<AA_PT_NODE*>& nodes, int& index) {

	std::vector<AA_PT_NODE*> subNodes;

	while (index < nodes.size()) {

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
