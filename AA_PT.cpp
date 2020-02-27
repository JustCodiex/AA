#include "AA_PT.h"
#include <map>

AA_PT::AA_PT(std::vector<AALexicalResult> lexResult) {

	std::vector<AA_PT_NODE*> aa_pt_nodes = ToNodes(lexResult);

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
			node->nodeType = AA_PT_NODE_TYPE::seperator;
			break;
		default:
			break;
		}

		node->content = lexResult[i].content;

		aa_pt_nodes.push_back(node);

	}

	return aa_pt_nodes;

}

bool AA_PT::IsUnaryOperator(std::vector<AA_PT_NODE*> nodes) {

	std::vector<AA_PT_NODE_TYPE> binop = {
		AA_PT_NODE_TYPE::intliteral,
	};

	if (nodes.size() > 0) {
		return std::find(binop.begin(), binop.end(), nodes[nodes.size() - 1]->nodeType) == binop.end();
	}

	return false;

}

AA_PT_NODE* AA_PT::CreateTree(std::vector<AA_PT_NODE*>& nodes, int from) {

	size_t nodeIndex = from;
	AA_PT_NODE* root = 0;

	while (nodes.size() > 1 && nodeIndex < nodes.size()) {
		switch (nodes[nodeIndex]->nodeType) {
		case AA_PT_NODE_TYPE::binary_operation:

			nodes[nodeIndex - 1]->parent = nodes[nodeIndex];
			nodes[nodeIndex + 1]->parent = nodes[nodeIndex];

			nodes[nodeIndex]->childNodes.push_back(nodes[nodeIndex - 1]);
			nodes[nodeIndex]->childNodes.push_back(nodes[nodeIndex + 1]);

			nodes.erase(nodes.begin() + nodeIndex+1);
			nodes.erase(nodes.begin() + nodeIndex-1);

			break;
		case AA_PT_NODE_TYPE::seperator:
			if (nodes[nodeIndex]->content == L";") {
				delete nodes[nodeIndex];
				nodes.erase(nodes.begin() + nodeIndex);
			} else if (nodes[nodeIndex]->content == L"(") {

			} else if (nodes[nodeIndex]->content == L")") {

			}
			break;
		case AA_PT_NODE_TYPE::intliteral:
			nodeIndex++;
			break;
		default:
			break;
		}

	}

	root = nodes.at(0);

	return root;

}
