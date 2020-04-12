#include "AA_PT.h"
#include <map>

bool AA_PT::g_hasEnyErr = false;
AA_PT::Error AA_PT::g_errMsg = AA_PT::Error();

AA_PT::AA_PT(AA_PT_NODE* root) {
	m_root = root;
}

void AA_PT::Clear() {
	this->ClearNode(m_root);
}

void AA_PT::ClearNode(AA_PT_NODE* node) {

	for (AA_PT_NODE* child : node->childNodes) {
		this->ClearNode(child);
	}

	delete node;

}

std::vector<AA_PT_NODE*> AA_PT::ToNodes(std::vector<AALexicalResult> lexResult) {

	std::vector<AA_PT_NODE*> aa_pt_nodes;

	for (size_t i = 0; i < lexResult.size(); i++) {

		if (lexResult[i].token == AAToken::whitespace) {
			continue;
		}

		AA_PT_NODE* node = new AA_PT_NODE(lexResult[i].position);

		switch (lexResult[i].token) {
		case AAToken::intlit:
			node->nodeType = AA_PT_NODE_TYPE::intliteral;
			break;
		case AAToken::floatlit:
			node->nodeType = AA_PT_NODE_TYPE::floatliteral;
			break;
		case AAToken::stringlit:
			node->nodeType = AA_PT_NODE_TYPE::stringliteral;
			break;
		case AAToken::charlit:
			node->nodeType = AA_PT_NODE_TYPE::charliteral;
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
			} else if (lexResult[i].content.compare(L"null") == 0) {
				node->nodeType = AA_PT_NODE_TYPE::nullliteral;
			} else {
				node->nodeType = AA_PT_NODE_TYPE::keyword;
			}
			break;
		case AAToken::accessor:
			node->nodeType = AA_PT_NODE_TYPE::accessor;
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
	} else if (val == L"[") {
		return AA_PT_NODE_TYPE::indexer_start;
	} else if (val == L"]") {
		return AA_PT_NODE_TYPE::indexer_end;
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
		AA_PT_NODE_TYPE::indexer_end,
	};

	if (nodes.size() > 0) {
		return std::find(binop.begin(), binop.end(), nodes[nodes.size() - 1]->nodeType) == binop.end();
	}

	return true;

}

bool AA_PT::IsBoolKeyword(std::wstring keyword) {
	return keyword == L"true" || keyword == L"false";
}

std::vector<AA_PT*> AA_PT::CreateTrees(std::vector<AA_PT_NODE*>& nodes) {

	size_t nodeIndex = 0;
	std::vector<AA_PT*> parseTrees;
	
	AA_PT pt = AA_PT(NULL);
	AA_PT_NODE* root = pt.CreateTree(nodes, 0);

	if (nodes.at(0) == root) {
		for (size_t i = 0; i < nodes.size(); i++) {
			parseTrees.push_back(new AA_PT(nodes.at(i)));
		}
	} else {
		if (nodes.at(0)->nodeType == AA_PT_NODE_TYPE::expression) {
			for (size_t i = 0; i < nodes.at(0)->childNodes.size(); i++) {
				parseTrees.push_back(new AA_PT(nodes.at(0)->childNodes.at(i)));
			}
		}
	}

	return parseTrees;

}

#define REMOVE_NODE(i) delete nodes[i];nodes.erase(nodes.begin() + i)

AA_PT_NODE* AA_PT::CreateTree(std::vector<AA_PT_NODE*>& nodes, size_t from) {

	size_t nodeIndex = from;

	if (nodes.size() == 1 && nodes[from]->nodeType == AA_PT_NODE_TYPE::expression) {
		return CreateExpressionTree(nodes, 0);
	}

	if (nodes.size() > 0 && (nodes[from]->nodeType == AA_PT_NODE_TYPE::block)) {
		HandleTreeCase(nodes, nodeIndex);
		if (nodeIndex == AA_PT_NODE_OUT_OF_BOUNDS_INDEX) {
			return 0;
		}
	} else {
		while (nodes.size() > 1 && nodeIndex < nodes.size()) {
			HandleTreeCase(nodes, nodeIndex);
			if (nodeIndex == AA_PT_NODE_OUT_OF_BOUNDS_INDEX) {
				return 0;
			}
		}
	}

	if (nodes.size() > 0) {
		return nodes.at(from);
	} else {
		return 0;
	}

}

void AA_PT::HandleTreeCase(std::vector<AA_PT_NODE*>& nodes, size_t& nodeIndex) {

	if (nodeIndex >= nodes.size()) {
		return;
	}

	switch (nodes[nodeIndex]->nodeType) {
	case AA_PT_NODE_TYPE::binary_operation:

		nodes[nodeIndex]->childNodes.push_back(nodes[nodeIndex - 1]);
		nodes[nodeIndex]->childNodes.push_back(this->CreateExpressionTree(nodes, nodeIndex + 1));

		nodes.erase(nodes.begin() + nodeIndex + 1);
		nodes.erase(nodes.begin() + nodeIndex - 1);

		break;
	case AA_PT_NODE_TYPE::unary_operation:

		nodes[nodeIndex]->childNodes.push_back(this->CreateExpressionTree(nodes, nodeIndex + 1));
		nodes.erase(nodes.begin() + nodeIndex + 1);

		nodeIndex++;

		break;
	case AA_PT_NODE_TYPE::seperator:
		if (nodes[nodeIndex]->content == L";" || nodes[nodeIndex]->content == L",") {
			REMOVE_NODE(nodeIndex);
		}
		break;
	case AA_PT_NODE_TYPE::accessor:

		if (nodes[nodeIndex]->childNodes[0]->childNodes.size() > 0 && nodes[nodeIndex]->childNodes[0]->nodeType != AA_PT_NODE_TYPE::identifier) {
			size_t t = 0;
			this->HandleTreeCase(nodes[nodeIndex]->childNodes, t);
		}

		if (nodes[nodeIndex]->childNodes[1]->childNodes.size() > 0 && nodes[nodeIndex]->childNodes[1]->nodeType != AA_PT_NODE_TYPE::identifier) {
			nodes[nodeIndex]->childNodes[1] = this->CreateTree(nodes[nodeIndex]->childNodes[1]->childNodes, 0);
		}

		// Goto next element
		nodeIndex++;

		break;
	case AA_PT_NODE_TYPE::identifier:
		if (nodeIndex + 1 < nodes.size() && (nodes[nodeIndex + 1]->nodeType == AA_PT_NODE_TYPE::identifier || IsDeclarativeIndexer(nodes[nodeIndex+1]))) {

			// Handle array decleration if it's the case
			this->HandleIndexDecl(nodes, nodeIndex);

			// if the next segment is an argument list (expression), it's a func decl with type != void
			// If not, it's most likely a variable decleration

			if (nodeIndex + 2 < nodes.size() && nodes[nodeIndex + 2]->nodeType == AA_PT_NODE_TYPE::expression) {
				nodes[nodeIndex] = this->HandleFunctionDecleration(nodes, nodeIndex);
			} else {
				nodes[nodeIndex] = this->HandleVariableDecleration(nodes, nodeIndex);
			}

			// Goto next element
			nodeIndex++;

		} else if (nodeIndex + 1 < nodes.size() && nodes[nodeIndex + 1]->nodeType == AA_PT_NODE_TYPE::expression) {
			
			if (nodeIndex + 2 < nodes.size() && nodes[nodeIndex + 2]->nodeType == AA_PT_NODE_TYPE::block) {

				// This is something of the type ([identifier] [expression] [block])
				// Which is how you'd usually specify a class constructor

				nodes[nodeIndex] = this->CreateConstructorDecl(nodes, nodeIndex);

			} else {

				AA_PT_NODE* funcallNode = new AA_PT_NODE(nodes[nodeIndex]->position);
				funcallNode->content = nodes[nodeIndex]->content;
				funcallNode->nodeType = AA_PT_NODE_TYPE::funccall;
				funcallNode->childNodes = this->CreateArgumentTree(nodes[nodeIndex + 1]);

				nodes.erase(nodes.begin() + nodeIndex, nodes.begin() + nodeIndex + 2);
				nodes.insert(nodes.begin() + nodeIndex, funcallNode);

			}

			nodeIndex++;

		} else if (nodeIndex + 1 < nodes.size() && nodes[nodeIndex + 1]->nodeType == AA_PT_NODE_TYPE::indexing) {
		
			AA_PT_NODE* indexatNode = new AA_PT_NODE(nodes[nodeIndex]->position);
			indexatNode->nodeType = AA_PT_NODE_TYPE::indexat;
			indexatNode->childNodes.push_back(nodes[nodeIndex]);
			indexatNode->childNodes.push_back(this->CreateTree(nodes[nodeIndex + 1]->childNodes, 0));

			nodes.erase(nodes.begin() + nodeIndex + 1);

			nodes[nodeIndex] = indexatNode;

			nodeIndex++;

		} else {
			int t = nodeIndex;
			if (t - 1 >= 0 && nodes[t-1]->nodeType == AA_PT_NODE_TYPE::accessor) {
				if (t + 1 < (int)nodes.size() && nodes[t + 1]->nodeType == AA_PT_NODE_TYPE::binary_operation && nodes[t + 1]->content.compare(L"=") == 0) {
					nodeIndex--;
					nodes[nodeIndex] = this->HandleVariableDecleration(nodes, nodeIndex);
				} else {
					printf("Something not right, AA_PT.Cpp@%i\n", __LINE__);
				}
			}
			nodeIndex++;
		}
		break;
	case AA_PT_NODE_TYPE::intliteral:
	case AA_PT_NODE_TYPE::floatliteral:
	case AA_PT_NODE_TYPE::booliterral:
	case AA_PT_NODE_TYPE::stringliteral:
	case AA_PT_NODE_TYPE::charliteral:
	case AA_PT_NODE_TYPE::nullliteral:
		nodeIndex++;
		break;
	case AA_PT_NODE_TYPE::keyword:
		this->HandleKeywordCase(nodes, nodeIndex);
		
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

void AA_PT::HandleKeywordCase(std::vector<AA_PT_NODE*>& nodes, size_t& nodeIndex) {

	if (nodes[nodeIndex]->content.compare(L"var") == 0) {

		nodes[nodeIndex] = this->HandleVariableDecleration(nodes, nodeIndex);

	} else if (nodes[nodeIndex]->content.compare(L"val") == 0) {

		if (nodeIndex + 1 < nodes.size() && nodes[nodeIndex + 1]->nodeType == AA_PT_NODE_TYPE::identifier) {
			nodes[nodeIndex] = this->HandleVariableDecleration(nodes, nodeIndex);
		} else {
			nodes[nodeIndex]->nodeType = AA_PT_NODE_TYPE::identifier;
		}

	} else if (nodes[nodeIndex]->content.compare(L"class") == 0) {
	
		nodes[nodeIndex] = this->CreateClassDecl(nodes, nodeIndex);

	} else if (nodes[nodeIndex]->content.compare(L"namespace") == 0) {
	
		nodes[nodeIndex] = this->CreateNamespaceDecl(nodes, nodeIndex);

	} else if (nodes[nodeIndex]->content.compare(L"enum") == 0) {

		nodes[nodeIndex] = this->CreateEnumDecl(nodes, nodeIndex);

	} else if (nodes[nodeIndex]->content.compare(L"void") == 0) {
		
		AA_PT_NODE* funcDeclNode = this->CreateFunctionDecl(nodes, nodeIndex);
		if (funcDeclNode) {
			nodes[nodeIndex] = funcDeclNode;
		}

	} else if (nodes[nodeIndex]->content.compare(L"Any") == 0) {
	
		// Make it be an identifier instead
		nodes[nodeIndex]->nodeType = AA_PT_NODE_TYPE::identifier;

	} else if (nodes[nodeIndex]->content.compare(L"if") == 0) {
		
		// Create conditional block (This automatically includes following else-if and else statements
		nodes[nodeIndex] = this->CreateConditionBlock(nodes, nodeIndex);

	} else if (nodes[nodeIndex]->content.compare(L"for") == 0) {

		// Create for block
		nodes[nodeIndex] = this->CreateForStatement(nodes, nodeIndex);

	} else if (nodes[nodeIndex]->content.compare(L"while") == 0) {

		// Create while block
		nodes[nodeIndex] = this->CreateWhileStatement(nodes, nodeIndex);

	} else if (nodes[nodeIndex]->content.compare(L"do") == 0) {

		// Is it a do-while loop?
		if (nodeIndex + 2 < nodes.size() && nodes[nodeIndex + 2]->nodeType == AA_PT_NODE_TYPE::keyword && nodes[nodeIndex + 2]->content == L"while") {

			// Create a do-while block
			nodes[nodeIndex] = this->CreateDoWhileStatement(nodes, nodeIndex);

		}

	} else if (nodes[nodeIndex]->content.compare(L"new") == 0) {

		if (nodeIndex + 2 < nodes.size() && (nodes[nodeIndex + 2]->nodeType == AA_PT_NODE_TYPE::expression || nodes[nodeIndex + 2]->nodeType == AA_PT_NODE_TYPE::indexing)) {

			if (nodes[nodeIndex + 1]->nodeType == AA_PT_NODE_TYPE::identifier && nodes[nodeIndex+2]->nodeType != AA_PT_NODE_TYPE::indexing) {

				AA_PT_NODE* funcallNode = new AA_PT_NODE(nodes[nodeIndex+1]->position);
				funcallNode->content = nodes[nodeIndex+1]->content;
				funcallNode->nodeType = AA_PT_NODE_TYPE::funccall;
				funcallNode->childNodes = this->CreateArgumentTree(nodes[nodeIndex + 2]);

				nodes.erase(nodes.begin() + nodeIndex + 1, nodes.begin() + nodeIndex + 3);

				nodes[nodeIndex]->childNodes.push_back(funcallNode);
				nodes[nodeIndex]->nodeType = AA_PT_NODE_TYPE::newstatement;

			} else if (nodes[nodeIndex + 1]->nodeType == AA_PT_NODE_TYPE::identifier && nodes[nodeIndex + 2]->nodeType == AA_PT_NODE_TYPE::indexing) {
				
				size_t i = nodeIndex + 1;
				this->HandleTreeCase(nodes, i);

				nodes[nodeIndex]->childNodes.push_back(nodes[nodeIndex + 1]);
				nodes[nodeIndex]->nodeType = AA_PT_NODE_TYPE::newstatement;

				nodes.erase(nodes.begin() + nodeIndex + 1);

				return;

			} else {

				SetError(AA_PT::Error("Expected identifier following 'new' keyword!", 1, nodes[nodeIndex]->position));
				nodeIndex = AA_PT_NODE_OUT_OF_BOUNDS_INDEX;
				return;

			}

		} else {

			SetError(AA_PT::Error("Invalid usage of new!", 0, nodes[nodeIndex]->position));
			nodeIndex = AA_PT_NODE_OUT_OF_BOUNDS_INDEX;
			return;

		}

	} else if (nodes[nodeIndex]->content.compare(L"using") == 0) {

		AA_PT_NODE_TYPE useType = nodes[nodeIndex + 1]->nodeType;
		bool isIdentifier = useType == AA_PT_NODE_TYPE::identifier;

		if (nodeIndex + 1 < nodes.size() && (isIdentifier || useType == AA_PT_NODE_TYPE::accessor)) {

			if (nodeIndex + 2 < nodes.size() && nodes[nodeIndex + 2]->nodeType == AA_PT_NODE_TYPE::keyword) {

				if (nodes[nodeIndex + 2]->content == L"from" && isIdentifier) {

					if (nodeIndex + 3 < nodes.size() && nodes[nodeIndex + 3]->nodeType == AA_PT_NODE_TYPE::identifier || nodes[nodeIndex + 3]->nodeType == AA_PT_NODE_TYPE::accessor) {

						// Make the using statement
						nodes[nodeIndex]->nodeType = AA_PT_NODE_TYPE::usingstatement;
						nodes[nodeIndex]->content = nodes[nodeIndex + 1]->content;

						// Make the from statement
						nodes[nodeIndex + 2]->nodeType = AA_PT_NODE_TYPE::fromstatement;
						nodes[nodeIndex + 2]->content = nodes[nodeIndex + 3]->content;
						nodes[nodeIndex + 2]->childNodes.push_back(nodes[nodeIndex + 3]);

						// Add form statement as substatement to the using statement
						nodes[nodeIndex]->childNodes.push_back(nodes[nodeIndex + 2]);

						nodes.erase(nodes.begin() + nodeIndex + 1, nodes.begin() + nodeIndex + 4);

					} else {
						SetError(AA_PT::Error("Expected identifier following 'from' keyword!", 2, nodes[nodeIndex]->position));
						nodeIndex = AA_PT_NODE_OUT_OF_BOUNDS_INDEX;
						return;
					}

				} else {
					SetError(AA_PT::Error("Expected 'from' keyword in 'using' statement", 3, nodes[nodeIndex]->position));
					nodeIndex = AA_PT_NODE_OUT_OF_BOUNDS_INDEX;
					return;
				}

			} else {

				// Create using directive and add namespace as child element
				nodes[nodeIndex]->nodeType = AA_PT_NODE_TYPE::usingstatement;
				nodes[nodeIndex]->content = nodes[nodeIndex + 1]->content;
				nodes[nodeIndex]->childNodes.push_back(nodes[nodeIndex + 1]);

				nodes.erase(nodes.begin() + nodeIndex + 1);

			}

		} else {
			SetError(AA_PT::Error("Expected identifier following 'using' keyword!", 4, nodes[nodeIndex]->position));
			nodeIndex = AA_PT_NODE_OUT_OF_BOUNDS_INDEX;
			return;
		}

	} else if (nodes[nodeIndex]->content.compare(L"match") == 0) {

		// Create pattern matching block (Note! we take whatever was before this lement and pattern matches with it, therefore we must assign to the previous element)
		nodes[nodeIndex-1] = this->CreatePatternMatchBlock(nodes, nodeIndex);

	} else if (IsModifierKeyword(nodes[nodeIndex]->content)) { // Is it a modifier (eg. virtual, override etc)

		// Store modifier value
		std::wstring content = nodes[nodeIndex]->content;

		// Store location in node set
		size_t n = nodeIndex;

		// Goto next node
		nodeIndex++;

		// Parse next modifier OR next function decleration
		this->HandleTreeCase(nodes, nodeIndex);

		// Remove self
		REMOVE_NODE(n);

		// Add modifier to decleration's modifier list
		if (nodes[n]->nodeType == AA_PT_NODE_TYPE::fundecleration) {
			nodes[n]->childNodes[2]->childNodes.push_back(new AA_PT_NODE(AA_PT_NODE_TYPE::modifier, content.c_str(), nodes[n]->position));
		} else if (nodes[n]->nodeType == AA_PT_NODE_TYPE::classdecleration) {
			nodes[n]->childNodes[0]->childNodes.push_back(new AA_PT_NODE(AA_PT_NODE_TYPE::modifier, content.c_str(), nodes[n]->position));
		}

	}

	nodeIndex++;

}

void AA_PT::HandleIndexDecl(std::vector<AA_PT_NODE*>& nodes, size_t nodeIndex) {

	if (nodes[nodeIndex + 1]->nodeType == AA_PT_NODE_TYPE::indexing) {
		if (nodes[nodeIndex + 1]->childNodes.size() == 0) {
			nodes[nodeIndex]->content += L"[]";
			nodes.erase(nodes.begin() + nodeIndex + 1);
		} else {
			printf("Unexpected number of arguments!");
		}
	}

}

AA_PT_NODE* AA_PT::HandleFunctionDecleration(std::vector<AA_PT_NODE*>& nodes, size_t& from) {
	return this->CreateFunctionDecl(nodes, from);
}

AA_PT_NODE* AA_PT::HandleVariableDecleration(std::vector<AA_PT_NODE*>& nodes, size_t& from) {
	return this->CreateVariableDecl(nodes, from);
}

AA_PT_NODE* AA_PT::CreateExpressionTree(std::vector<AA_PT_NODE*>& nodes, size_t from) {

	if (nodes[from]->nodeType == AA_PT_NODE_TYPE::expression && nodes[from]->childNodes.size() > 0) {

		AA_PT_NODE* exp = CreateTree(nodes[from]->childNodes, 0); // Create a tree for the expression
		while (exp->nodeType == AA_PT_NODE_TYPE::expression) { // As long as we have a single expression here, we break it down to smaller bits, incase we get input like (((5+5))).
			exp = CreateTree(exp->childNodes, 0); // Create the tree for the sub expression
		}

		return exp;

	} else if (nodes[from]->nodeType == AA_PT_NODE_TYPE::identifier && nodes[from]->childNodes.size() > 0) {
	
		AA_PT_NODE* funcallNode = new AA_PT_NODE(nodes[from]->position);
		funcallNode->content = nodes[from]->content;
		funcallNode->nodeType = AA_PT_NODE_TYPE::funccall;
		funcallNode->childNodes = this->CreateArgumentTree(nodes[from]);

		nodes[from] = funcallNode;

		return funcallNode;

	} else if (nodes[from]->nodeType == AA_PT_NODE_TYPE::accessor) {
	
		size_t d = 1;
		this->HandleTreeCase(nodes[from]->childNodes, d);

		return nodes[from];

	} else {

		return nodes[from];

	}

}

std::vector<AA_PT_NODE*> AA_PT::CreateDeclerativeBody(std::vector<AA_PT_NODE*> nodes) {

	size_t i = 0;
	while (i < nodes.size()) {
		this->HandleTreeCase(nodes, i);
	}

	return nodes;

}

std::vector<AA_PT_NODE*> AA_PT::CreateArgumentTree(AA_PT_NODE* pExpNode) {

	std::vector<AA_PT_NODE*> nodes;
	this->ApplyStatementBindings(pExpNode->childNodes);

	for (size_t i = 0; i < pExpNode->childNodes.size(); i++) {
		
		std::vector<AA_PT_NODE*> argElements;
		
		for (AA_PT_NODE* n : pExpNode->childNodes[i]->childNodes) {
			argElements.push_back(n);
		}

		ApplySyntaxRules(argElements);

		AA_PT_NODE* arg = this->CreateTree(argElements, 0);

		// For some reason we get some problems with expressions in accessors here => so we have to add in this. TODO: Fix later
		if (arg->nodeType == AA_PT_NODE_TYPE::accessor && arg->childNodes[1]->nodeType == AA_PT_NODE_TYPE::expression) {
			arg->childNodes[1] = this->CreateTree(arg->childNodes[1]->childNodes, 0);
		}

		nodes.push_back(arg);

	}

	return nodes;

}

AA_PT_NODE* AA_PT::CreateVariableDecl(std::vector<AA_PT_NODE*>& nodes, size_t from) {

	if (nodes[from + 1]->nodeType != AA_PT_NODE_TYPE::identifier) {
		printf("Err: Expected identifier");
	}

	AA_PT_NODE* varDeclExp = new AA_PT_NODE(nodes[from]->position);
	varDeclExp->nodeType = AA_PT_NODE_TYPE::vardecleration;
	varDeclExp->childNodes.push_back(nodes[from]);
	varDeclExp->childNodes.push_back(nodes[from+1]);

	nodes.erase(nodes.begin() + from + 1);

	return varDeclExp;

}

AA_PT_NODE* AA_PT::CreateClassDecl(std::vector<AA_PT_NODE*>& nodes, size_t from) {

	// Create the class decl node (The actual class decleration
	AA_PT_NODE* classDeclExp = new AA_PT_NODE(nodes[from]->position);
	classDeclExp->nodeType = AA_PT_NODE_TYPE::classdecleration;

	// Create the modifier list
	AA_PT_NODE* modifierList = new AA_PT_NODE(nodes[from]->position);
	modifierList->nodeType = AA_PT_NODE_TYPE::modifierlist;

	// Create the inheritance list node
	AA_PT_NODE* inheritanceList = new AA_PT_NODE(nodes[from]->position);
	inheritanceList->nodeType = AA_PT_NODE_TYPE::classinheritancelist;

	// Add modifier and inheritance lists
	classDeclExp->childNodes.push_back(modifierList);
	classDeclExp->childNodes.push_back(inheritanceList);

	// Set the name of the class we're declaring
	classDeclExp->content = this->FindClassDeclName(nodes, from, classDeclExp);

	// Do we then have a body to evaluate?
	if (from + 2 < nodes.size() && nodes[from + 2]->nodeType == AA_PT_NODE_TYPE::block) {

		// Create class body
		AA_PT_NODE* classBody = new AA_PT_NODE(nodes[from + 2]->position);
		classBody->nodeType = AA_PT_NODE_TYPE::classbody;
		classBody->childNodes = this->CreateDeclerativeBody(nodes[from + 2]->childNodes);

		// Add class body
		classDeclExp->childNodes.push_back(classBody);

	}

	// Erase all the unnessecary stuff
	nodes.erase(nodes.begin() + from + 1, nodes.begin() + from + 3);

	// Return class definition
	return classDeclExp;

}

std::wstring AA_PT::FindClassDeclName(std::vector<AA_PT_NODE*>& nodes, const size_t from, AA_PT_NODE* pClassDeclNode) {

	if (from + 1 < nodes.size()) {
		if (nodes[from + 1]->nodeType == AA_PT_NODE_TYPE::identifier) {
			return nodes[from + 1]->content;
		} else if (nodes[from+1]->nodeType == AA_PT_NODE_TYPE::accessor) {
			// TODO: Add support for multiple inheritance
			pClassDeclNode->childNodes[1]->childNodes.push_back(new AA_PT_NODE(AA_PT_NODE_TYPE::identifier, nodes[from + 1]->childNodes[1]->content.c_str(), nodes[from + 1]->position));
			return nodes[from + 1]->childNodes[0]->content;
		}
	} else {
		printf("Found class keyword without a valid identifying name!"); // TODO: Throw error
	}

	// TODO: Throw error
	return L"NO NAME CLASS";

}

AA_PT_NODE* AA_PT::CreateConstructorDecl(std::vector<AA_PT_NODE*>& nodes, size_t from) {

	if (nodes[from + 1]->nodeType != AA_PT_NODE_TYPE::expression) {
		printf("Err: Expected function argument list");
		return 0;
	}

	if (nodes[from + 2]->nodeType != AA_PT_NODE_TYPE::block) {
		printf("Err: Expected function body");
		return 0;
	}

	AA_PT_NODE* modifierList = new AA_PT_NODE(nodes[from]->position);
	modifierList->nodeType = AA_PT_NODE_TYPE::modifierlist;

	AA_PT_NODE* classType = new AA_PT_NODE(nodes[from]->position);
	classType->nodeType = AA_PT_NODE_TYPE::identifier;
	classType->content = L"ctor";

	AA_PT_NODE* ctorDecl = new AA_PT_NODE(nodes[from]->position);
	ctorDecl->nodeType = AA_PT_NODE_TYPE::fundecleration;
	ctorDecl->content = L".ctor";
	ctorDecl->childNodes.push_back(classType);
	ctorDecl->childNodes.push_back(CreateFunctionArgList(nodes[from + 1])); // argument list
	ctorDecl->childNodes.push_back(modifierList);

	if (from + 2 < (int)nodes.size() && nodes[from + 2]->nodeType == AA_PT_NODE_TYPE::block) {
		size_t n = 0;
		AA_PT_NODE* p = this->CreateTree(nodes[from + 2]->childNodes, n);
		nodes[from + 2]->nodeType = AA_PT_NODE_TYPE::funcbody;
		ctorDecl->childNodes.push_back(nodes[from + 2]); // function body
		nodes.erase(nodes.begin() + from + 2);
	}

	nodes.erase(nodes.begin() + from + 1, nodes.begin() + from + 2);
	
	return ctorDecl;

}

AA_PT_NODE* AA_PT::CreateFunctionDecl(std::vector<AA_PT_NODE*>& nodes, size_t from) {

	if (nodes[from + 1]->nodeType != AA_PT_NODE_TYPE::identifier) {
		printf("Err: Expected identifier");
		return 0;
	}

	if (nodes[from + 2]->nodeType != AA_PT_NODE_TYPE::expression) {
		printf("Err: Expected argument list");
		return 0;
	}

	AA_PT_NODE* modifierList = new AA_PT_NODE(nodes[from]->position);
	modifierList->nodeType = AA_PT_NODE_TYPE::modifierlist;

	AA_PT_NODE* funDecl = new AA_PT_NODE(nodes[from]->position);
	funDecl->nodeType = AA_PT_NODE_TYPE::fundecleration;
	funDecl->content = nodes[from + 1]->content;
	funDecl->childNodes.push_back(nodes[from]); // return type
	funDecl->childNodes.push_back(CreateFunctionArgList(nodes[from+2])); // argument list
	funDecl->childNodes.push_back(modifierList);

	if (from + 3 < (int)nodes.size() && nodes[from + 3]->nodeType == AA_PT_NODE_TYPE::block) {
		size_t n = 0;
		AA_PT_NODE* p = this->CreateTree(nodes[from + 3]->childNodes, n);
		nodes[from + 3]->nodeType = AA_PT_NODE_TYPE::funcbody;
		funDecl->childNodes.push_back(nodes[from + 3]); // function body
		nodes.erase(nodes.begin() + from + 3);
	}

	nodes.erase(nodes.begin() + from, nodes.begin() + from + 3);
	nodes.insert(nodes.begin() + from, funDecl);

	return funDecl;

}

AA_PT_NODE* AA_PT::CreateFunctionArgList(AA_PT_NODE* pExpNode) {

	AA_PT_NODE* argList = new AA_PT_NODE(pExpNode->position);
	argList->nodeType = AA_PT_NODE_TYPE::funarglist;

	size_t i = 0;
	while (i < pExpNode->childNodes.size()) {

		if (pExpNode->childNodes[i]->nodeType == AA_PT_NODE_TYPE::identifier) {
			if (i + 1 < pExpNode->childNodes.size() && pExpNode->childNodes[i + 1]->nodeType == AA_PT_NODE_TYPE::identifier) {

				AA_PT_NODE* arg = new AA_PT_NODE(pExpNode->childNodes[i]->position);
				arg->nodeType = AA_PT_NODE_TYPE::funarg;
				arg->content = pExpNode->childNodes[i + 1]->content;
				arg->childNodes.push_back(pExpNode->childNodes[i]);

				argList->childNodes.push_back(arg);

				i++;
				i++;

			}
		} else if (pExpNode->childNodes[i]->nodeType == AA_PT_NODE_TYPE::seperator && pExpNode->childNodes[i]->content == L",") {
			i++;
		} else {
			printf("Some annoying error");
			break;
		}

	}

	return argList;

}

AA_PT_NODE* AA_PT::CreateNamespaceDecl(std::vector<AA_PT_NODE*>& nodes, size_t from) {

	if (from + 1 < nodes.size() && nodes[from + 1]->nodeType == AA_PT_NODE_TYPE::identifier) {

		if (from + 2 < nodes.size() && nodes[from + 2]->nodeType == AA_PT_NODE_TYPE::block) {

			nodes[from]->nodeType = AA_PT_NODE_TYPE::namespacedecleration;
			nodes[from]->content = nodes[from + 1]->content;
			nodes[from]->childNodes = this->CreateDeclerativeBody(nodes[from + 2]->childNodes);

			nodes.erase(nodes.begin() + from + 1, nodes.begin() + from + 3);

		} else {
			SetError(AA_PT::Error("Expected namespace content", 0, nodes[from]->position));
			return NULL;
		}

	} else if (from + 1 < nodes.size() && nodes[from + 1]->nodeType == AA_PT_NODE_TYPE::accessor) {
	
		if (from + 2 >= nodes.size() || nodes[from + 2]->nodeType != AA_PT_NODE_TYPE::block) {
			SetError(AA_PT::Error("Expected namespace content", 0, nodes[from]->position));
			return NULL;
		}

		AA_PT_NODE* n = nodes[from + 1]->childNodes[0];
		std::vector<AA_PT_NODE*> subnodes;
		subnodes.push_back(nodes[from]);
		subnodes.push_back(n);

		AA_PT_NODE* body = new AA_PT_NODE(n->position);
		body->nodeType = AA_PT_NODE_TYPE::block;

		AA_PT_NODE* subnamespace = new AA_PT_NODE(n->position);
		subnamespace->nodeType = AA_PT_NODE_TYPE::keyword;
		subnamespace->content = L"namespace";

		AA_PT_NODE* spacename = new AA_PT_NODE(n->position);
		spacename->nodeType = AA_PT_NODE_TYPE::identifier;
		spacename->content = nodes[from + 1]->childNodes[1]->content;

		body->childNodes.push_back(subnamespace);
		body->childNodes.push_back(spacename);
		body->childNodes.push_back(nodes[from + 2]);

		subnodes.push_back(body);

		nodes[from + 2] = this->CreateNamespaceDecl(subnodes, 0);
		nodes.erase(nodes.begin() + from + 1, nodes.begin() + from + 3);

	} else {
		SetError(AA_PT::Error("Expected namespace identifier", 0, nodes[from]->position));
		return NULL;
	}

	// Return the namespace decleration
	return nodes[from];

}

AA_PT_NODE* AA_PT::CreateEnumDecl(std::vector<AA_PT_NODE*>& nodes, size_t from) {

	if (from + 2 < nodes.size() && nodes[from + 1]->nodeType == AA_PT_NODE_TYPE::identifier && nodes[from + 2]->nodeType == AA_PT_NODE_TYPE::block) {

		// Define this as an enum decleration
		nodes[from]->nodeType = AA_PT_NODE_TYPE::enumdecleration;
		nodes[from]->content = nodes[from + 1]->content;

		// Get enum values
		AA_PT_NODE* pValListNode = this->CreateEnumValueList(nodes[from + 2]);
		nodes[from]->childNodes.push_back(pValListNode);

		// Create enum body
		AA_PT_NODE* pEnumBodyNode = new AA_PT_NODE(nodes[from + 2]->position);
		pEnumBodyNode->nodeType = AA_PT_NODE_TYPE::enumbody;
		pEnumBodyNode->childNodes = this->CreateDeclerativeBody(nodes[from + 2]->childNodes);

		// Add enum body
		nodes[from]->childNodes.push_back(pEnumBodyNode);

		// Remove modified elements
		nodes.erase(nodes.begin() + from + 1, nodes.begin() + from + 3);

	} else {
		SetError(AA_PT::Error("Expected namespace identifier", 0, nodes[from]->position));
		return NULL;
	}

	return nodes[from];

}

AA_PT_NODE* AA_PT::CreateEnumValueList(AA_PT_NODE* pBlockNode) {

	// Create the new node
	AA_PT_NODE* pValListNode = new AA_PT_NODE(pBlockNode->position);
	pValListNode->nodeType = AA_PT_NODE_TYPE::enumvallist;

	// For all subelements
	size_t i = 0;
	while (i < pBlockNode->childNodes.size()) {

		if (i + 1 < pBlockNode->childNodes.size()) {
			if (pBlockNode->childNodes[i+1]->nodeType == AA_PT_NODE_TYPE::seperator && pBlockNode->childNodes[i+1]->content.compare(L",") == 0) {
				if (pBlockNode->childNodes[i]->nodeType == AA_PT_NODE_TYPE::identifier) {
					pValListNode->childNodes.push_back(pBlockNode->childNodes[i]);
					pBlockNode->childNodes.erase(pBlockNode->childNodes.begin() + i, pBlockNode->childNodes.begin() + i + 2);
				} else {
					i++;
				}
			} else {
				if (pBlockNode->childNodes[i]->nodeType == AA_PT_NODE_TYPE::identifier) {
					pValListNode->childNodes.push_back(pBlockNode->childNodes[i]);
					pBlockNode->childNodes.erase(pBlockNode->childNodes.begin() + i);
					break;
				} else {
					break;
				}
			}
		} else {
			if (pBlockNode->childNodes[i]->nodeType == AA_PT_NODE_TYPE::identifier) {
				pValListNode->childNodes.push_back(pBlockNode->childNodes[i]);
				pBlockNode->childNodes.erase(pBlockNode->childNodes.begin() + i);
				break;
			} else {
				i++;
			}
		}

	}
	 // TODO: Throw error if we have to identifiers following each other
	return pValListNode;

}

AA_PT_NODE* AA_PT::CreatePatternMatchBlock(std::vector<AA_PT_NODE*>& nodes, size_t from) { // TODO: Make so we don't replace the previous statement (because we could be pattern matching different things)

	// Make sure there's an identifier to work with
	if (from == 0) {
		SetError(AA_PT::Error("Expected identifier to pattern match", 0, nodes[from]->position));
		return NULL;
	}

	// Create match statement and add match target
	AA_PT_NODE* matchStatement = new AA_PT_NODE(nodes[from]->position);
	matchStatement->nodeType = AA_PT_NODE_TYPE::matchstatement;
	matchStatement->childNodes.push_back(nodes[from - 1]);

	// Remove match statement
	nodes.erase(nodes.begin() + from);

	// Create match case list
	AA_PT_NODE* matchblock = new AA_PT_NODE(nodes[from-1]->position);
	matchblock->nodeType = AA_PT_NODE_TYPE::matchcaselist;
	matchblock->childNodes = this->CreatePatternCases(nodes[from]);

	// Erase the block
	nodes.erase(nodes.begin() + from);

	// Add matchblock to match statement
	matchStatement->childNodes.push_back(matchblock);

	// Update previous statement
	nodes[from - 1] = matchStatement;

	// Return the match case
	return nodes[from-1];

}

std::vector<AA_PT_NODE*> AA_PT::CreatePatternCases(AA_PT_NODE* pBlockNode) {

	std::vector<AA_PT_NODE*> cases;

	for (size_t i = 0; i < pBlockNode->childNodes.size(); i++) {

		if (pBlockNode->childNodes[i]->nodeType == AA_PT_NODE_TYPE::keyword && pBlockNode->childNodes[i]->content.compare(L"case") == 0) {

			std::vector<AA_PT_NODE*> subnodes;

			while (i < pBlockNode->childNodes.size() && (pBlockNode->childNodes[i]->nodeType != AA_PT_NODE_TYPE::seperator && pBlockNode->childNodes[i]->content.compare(L",") != 0)) {
				subnodes.push_back(pBlockNode->childNodes[i]);
				i++;
			}

			cases.push_back(this->CreatePatternCase(subnodes));

		} else {

			// Write out error
			this->SetError(AA_PT::Error("Expected 'case' keyword", 0, pBlockNode->childNodes[i]->position));

			// Return empty list
			return std::vector<AA_PT_NODE*>();

		}

	}

	return cases;

}

AA_PT_NODE* AA_PT::CreatePatternCase(std::vector<AA_PT_NODE*> nodes) {

	AA_PT_NODE* pCaseNode = new AA_PT_NODE(nodes[0]->position);
	pCaseNode->nodeType = AA_PT_NODE_TYPE::matchcasestatement;

	std::vector<AA_PT_NODE*> condition;
	std::vector<AA_PT_NODE*> expression;

	bool isReadingCondition = true;

	for (size_t i = 1; i < nodes.size(); i++) {

		if (nodes[i]->nodeType == AA_PT_NODE_TYPE::binary_operation && nodes[i]->content.compare(L"=>") == 0) {
			isReadingCondition = false;
		} else {
			if (isReadingCondition) {
				condition.push_back(nodes[i]);
			} else {
				expression.push_back(nodes[i]);
			}
		}

	}

	// Create condition node
	AA_PT_NODE* pConditionNode = new AA_PT_NODE(condition[0]->position);
	pConditionNode->nodeType = AA_PT_NODE_TYPE::condition;
	pConditionNode->childNodes.push_back(this->CreateTree(condition, 0));

	// Create expression node
	AA_PT_NODE* pExpressionNode = new AA_PT_NODE(expression[0]->position);
	pExpressionNode->nodeType = AA_PT_NODE_TYPE::block; // Technically speaking, it's an expression, but the AST'fier rejects those
	pExpressionNode->childNodes.push_back(this->CreateTree(expression, 0));

	// For some reason we get some problems with expressions in accessors here => so we have to add in this. TODO: Fix later
	if (pExpressionNode->childNodes[0]->nodeType == AA_PT_NODE_TYPE::accessor && pExpressionNode->childNodes[0]->childNodes[1]->nodeType == AA_PT_NODE_TYPE::expression) {
		pExpressionNode->childNodes[0]->childNodes[1] = this->CreateTree(pExpressionNode->childNodes[0]->childNodes[1]->childNodes, 0);
	}

	// Add two sub elements to case node
	pCaseNode->childNodes.push_back(pConditionNode);
	pCaseNode->childNodes.push_back(pExpressionNode);

	// Return case node
	return pCaseNode;

}

AA_PT_NODE* AA_PT::CreateIfStatement(std::vector<AA_PT_NODE*>& nodes, size_t from) {

	if (from + 1 < nodes.size() && nodes[from + 1]->nodeType == AA_PT_NODE_TYPE::expression) {

		nodes[from + 1] = this->CreateTree(nodes[from + 1]->childNodes, 0);

		AA_PT_NODE* pConditionNode = new AA_PT_NODE(nodes[from + 1]->position);
		pConditionNode->nodeType = AA_PT_NODE_TYPE::condition;
		pConditionNode->childNodes.push_back(nodes[from + 1]);

		nodes[from]->childNodes.push_back(pConditionNode);

		nodes.erase(nodes.begin() + from + 1);

	} else {
		printf("if-keyword not followed by a condition - NOT allowed");
	}

	if (from + 1 < nodes.size() && (nodes[from + 1]->nodeType == AA_PT_NODE_TYPE::expression || nodes[from + 1]->nodeType == AA_PT_NODE_TYPE::block)) {

		nodes[from + 1] = this->CreateTree(nodes[from + 1]->childNodes, 0);
		nodes[from]->childNodes.push_back(nodes[from + 1]);

		nodes.erase(nodes.begin() + from + 1);

	} else {
		printf("if-statement not followed by an expression or block");
	}

	nodes[from]->nodeType = AA_PT_NODE_TYPE::ifstatement;

	return nodes[from];

}

AA_PT_NODE* AA_PT::CreateConditionBlock(std::vector<AA_PT_NODE*>& nodes, size_t from) {

	nodes[from] = this->CreateIfStatement(nodes, from);

	while (from + 1 < nodes.size() && nodes[from + 1]->nodeType == AA_PT_NODE_TYPE::keyword) {

		if (nodes[from + 1]->content == L"else") {

			AA_PT_NODE* pElseNode = new AA_PT_NODE(nodes[from + 1]->position);
			pElseNode->content = L"else";
			pElseNode->nodeType = AA_PT_NODE_TYPE::elsestatement;

			nodes[from + 2] = this->CreateTree(nodes[from + 2]->childNodes, 0);
			pElseNode->childNodes.push_back(nodes[from + 2]);

			nodes.erase(nodes.begin() + from + 1, nodes.begin() + from + 3);

			nodes[from]->childNodes.push_back(pElseNode);

		} else if (nodes[from + 1]->content == L"elseif") {

			AA_PT_NODE* pElseIfNode = this->CreateIfStatement(nodes, from + 1);

			nodes.erase(nodes.begin() + from + 1);

			nodes[from]->childNodes.push_back(pElseIfNode);

		} else {

			printf("Something!");

			break;

		}

	}

	return nodes[from];

}

AA_PT_NODE* AA_PT::CreateForStatement(std::vector<AA_PT_NODE*>& nodes, size_t nodeIndex) {

	AA_PT_NODE* forStatement = new AA_PT_NODE(nodes[nodeIndex]->position);
	forStatement->nodeType = AA_PT_NODE_TYPE::forstatement;

	if (nodeIndex + 1 < nodes.size() && nodes[nodeIndex + 1]->nodeType == AA_PT_NODE_TYPE::expression) {

		// Set type to block so we may extract more trees
		nodes[nodeIndex + 1]->nodeType = AA_PT_NODE_TYPE::block;

		// Make sure the statements follow syntax rules
		ApplyOrderOfOperationBindings(nodes[nodeIndex + 1]->childNodes); // [Temporary HACK to fix something in the ApplyFunctionalBindings method)

		// Get loop expressions
		std::vector<AA_PT_NODE*> forLoopExpr = this->CreateArgumentTree(nodes[nodeIndex + 1]);

		if (forLoopExpr.size() == 3) {
			forStatement->childNodes.push_back(forLoopExpr[0]); // initialise
			forStatement->childNodes.push_back(forLoopExpr[1]); // condition
			forStatement->childNodes.push_back(forLoopExpr[2]); // afterthought
		} else {
			printf("Missing statements!");
		}

	} else {

		printf("for-statement missing expressions");

	}

	if (nodeIndex + 2 < nodes.size() && nodes[nodeIndex + 2]->nodeType == AA_PT_NODE_TYPE::block) {

		forStatement->childNodes.push_back(this->CreateTree(nodes[nodeIndex + 2]->childNodes, 0));

	} else {

		printf("for-statement missing body");

	}

	nodes.erase(nodes.begin() + nodeIndex + 1, nodes.begin() + nodeIndex + 3);

	return forStatement;

}

AA_PT_NODE* AA_PT::CreateWhileStatement(std::vector<AA_PT_NODE*>& nodes, size_t nodeIndex) {

	AA_PT_NODE* whileStatement = new AA_PT_NODE(nodes[nodeIndex]->position);
	whileStatement->nodeType = AA_PT_NODE_TYPE::whilestatement;

	if (nodeIndex + 1 < nodes.size() && nodes[nodeIndex + 1]->nodeType == AA_PT_NODE_TYPE::expression) {

		whileStatement->childNodes.push_back(this->CreateTree(nodes[nodeIndex + 1]->childNodes, 0));

	} else {
		printf("while-statement missing condition");
	}

	if (nodeIndex + 2 < nodes.size() && nodes[nodeIndex + 2]->nodeType == AA_PT_NODE_TYPE::block) {

		whileStatement->childNodes.push_back(this->CreateTree(nodes[nodeIndex + 2]->childNodes, 0));

	} else {

		printf("while-statement missing body");

	}

	nodes.erase(nodes.begin() + nodeIndex + 1, nodes.begin() + nodeIndex + 3);

	return whileStatement;

}

AA_PT_NODE* AA_PT::CreateDoWhileStatement(std::vector<AA_PT_NODE*>& nodes, size_t nodeIndex) {
	
	AA_PT_NODE* dowhileStatement = new AA_PT_NODE(nodes[nodeIndex]->position);
	dowhileStatement->nodeType = AA_PT_NODE_TYPE::dowhilestatement;

	if (nodeIndex + 1 < nodes.size() && nodes[nodeIndex + 1]->nodeType == AA_PT_NODE_TYPE::block) {
		dowhileStatement->childNodes.push_back(this->CreateTree(nodes[nodeIndex + 1]->childNodes, 0));
	} else {
		printf("do-while statement without a body!");
	}

	if (nodeIndex + 3 < nodes.size() && nodes[nodeIndex + 3]->nodeType == AA_PT_NODE_TYPE::expression) {
		dowhileStatement->childNodes.push_back(this->CreateTree(nodes[nodeIndex + 3]->childNodes, 0));
	}

	nodes.erase(nodes.begin() + nodeIndex + 1, nodes.begin() + nodeIndex + 4);

	return dowhileStatement;

}

bool AA_PT::IsDeclarativeIndexer(AA_PT_NODE* pNode) {

	if (pNode->nodeType == AA_PT_NODE_TYPE::indexing) {
		return pNode->childNodes.size() == 0;
	} else {
		return false;
	}

}

bool AA_PT::IsModifierKeyword(std::wstring ws) {
	return ws.compare(L"override") == 0 || ws.compare(L"virtual") == 0;
}

void AA_PT::ApplyOrderOfOperationBindings(std::vector<AA_PT_NODE*>& nodes) {

	// Convert parenthesis blocks into singular expressions
	ApplyGroupings(nodes);

	// Apply bindings (eg. -1 => unary operation)
	ApplyUnaryBindings(nodes);
	
	// Apply mathematic rules So 5+5*5 = 30 and not 50
	ApplyArithemticRules(nodes);
	
	// Apply assignment order rule, so var x = 5+5 is treated as 5+5=x and not x=5, + 5
	ApplyAssignmentOrder(nodes);

}

void AA_PT::ApplyGroupings(std::vector<AA_PT_NODE*>& nodes) {

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

bool AA_PT::CanTreatKeywordAsIdentifier(AA_PT_NODE* node) {
	if (node->nodeType == AA_PT_NODE_TYPE::keyword) {
		return node->content.compare(L"this") == 0 || node->content.compare(L"base") == 0;
	} else {
		return false;
	}
}

void AA_PT::ApplyAccessorBindings(std::vector<AA_PT_NODE*>& nodes) {

	int i = 0;
	while (i < (int)nodes.size()) {

		if (nodes[i]->nodeType == AA_PT_NODE_TYPE::accessor) {

			if (i - 1 >= 0 && (nodes[i-1]->nodeType == AA_PT_NODE_TYPE::identifier || nodes[i-1]->nodeType == AA_PT_NODE_TYPE::accessor || CanTreatKeywordAsIdentifier(nodes[i-1]))) {

				if (i + 1 < (int)nodes.size() && nodes[i + 1]->nodeType == AA_PT_NODE_TYPE::identifier) {

					nodes[i]->childNodes.push_back(nodes[i - 1]);

					if (i + 2 < (int)nodes.size() && nodes[i + 2]->nodeType == AA_PT_NODE_TYPE::expression) {

						AA_PT_NODE* rhsNode = new AA_PT_NODE(nodes[i + 1]->position);
						rhsNode->nodeType = AA_PT_NODE_TYPE::expression;
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

void AA_PT::ApplyFunctionBindings(std::vector<AA_PT_NODE*>& nodes) {

	int i = 0;

	while (i < (int)nodes.size()) {

		if (nodes[i]->nodeType == AA_PT_NODE_TYPE::identifier) {
			if (i + 1 < (int)nodes.size() && nodes[i + 1]->nodeType == AA_PT_NODE_TYPE::expression) {
				if (i - 1 > 0 && nodes[i - 1]->nodeType == AA_PT_NODE_TYPE::binary_operation) {
					ApplyFunctionBindings(nodes[i + 1]->childNodes);
					nodes[i]->childNodes.push_back(nodes[i + 1]);
					//nodes.erase(nodes.begin() + i + 1);
				}
			}
		} /*else if (nodes[i]->nodeType == AA_PT_NODE_TYPE::keyword) {
			if (i + 1 < nodes.size() && nodes[i + 1]->nodeType == AA_PT_NODE_TYPE::expression) {
				if (i - 1 > 0 && nodes[i - 1]->nodeType == AA_PT_NODE_TYPE::binary_operation) {
					nodes[i]->childNodes.push_back(nodes[i + 1]);
					nodes.erase(nodes.begin() + i + 1);
				}
			}
		}*/

		i++;

	}

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

			AA_PT_NODE* unaryExp = new AA_PT_NODE(nodes[index]->position);
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

				AA_PT_NODE* rhs = new AA_PT_NODE(nodes[index]->position);
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

			AA_PT_NODE* rhs = new AA_PT_NODE(nodes[index]->position);
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
			subNodes.push_back(nodes[index]);
			index++;
		}

	}

	return subNodes;

}

void AA_PT::ApplySyntaxRules(std::vector<AA_PT_NODE*>& nodes) {

	// Apply OOP bindings
	ApplyOrderOfOperationBindings(nodes);

	// Apply FC bindings
	ApplyFlowControlBindings(nodes);

}
