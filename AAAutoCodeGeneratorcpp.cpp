#include "AAAutoCodeGenerator.h"
#include "AA_Node_Consts.h"

namespace aa {
	namespace compiler {
		namespace generator {


			AA_AST_NODE* NewFunctionNode(std::wstring name, std::wstring returnType, bool addBody) {

				AA_AST_NODE* fundecl = new AA_AST_NODE(name, AA_AST_NODE_TYPE::fundecl, AACodePosition::Undetermined);
				fundecl->expressions.push_back(new AA_AST_NODE(returnType, AA_AST_NODE_TYPE::typeidentifier, AACodePosition::Undetermined));
				fundecl->expressions.push_back(new AA_AST_NODE(L"", AA_AST_NODE_TYPE::funarglist, AACodePosition::Undetermined));
				fundecl->expressions.push_back(new AA_AST_NODE(L"", AA_AST_NODE_TYPE::modifierlist, AACodePosition::Undetermined));

				if (addBody) {
					fundecl->expressions.push_back(new AA_AST_NODE(L"<auto-func-body>", AA_AST_NODE_TYPE::funcbody, AACodePosition::Undetermined));
				}

				fundecl->tags["__ctorgen"] = true;

				return fundecl;

			}

			void AddFunctionArgument(AA_AST_NODE* pFuncDeclNode, std::wstring argName, std::wstring argType) {
				AA_AST_NODE* arg = new AA_AST_NODE(argName, AA_AST_NODE_TYPE::funarg, AACodePosition::Undetermined);
				arg->expressions.push_back(new AA_AST_NODE(argType, AA_AST_NODE_TYPE::typeidentifier, AACodePosition::Undetermined));
				pFuncDeclNode->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions.push_back(arg);
			}

			void AddFunctionArguments(AA_AST_NODE* pFuncDeclNode, std::vector<__name_type> args) {
				for (auto& pair : args) {
					AddFunctionArgument(pFuncDeclNode, pair.first, pair.second);
				}
			}

			void AddFunctionBodyElement(AA_AST_NODE* pFuncDeclNode, AA_AST_NODE* pNode) {
				if (AA_NODE_FUNNODE_BODY < pFuncDeclNode->expressions.size()) {
					pFuncDeclNode->expressions[AA_NODE_FUNNODE_BODY]->expressions.push_back(pNode);
				}
			}

			namespace expr {

				AA_AST_NODE* BinaryOperationNode(std::wstring op, AA_AST_NODE* pLeft, AA_AST_NODE* pRight) {
					AA_AST_NODE* pBinopNode = new AA_AST_NODE(op, AA_AST_NODE_TYPE::binop, AACodePosition::Undetermined);
					pBinopNode->expressions.push_back(pLeft);
					pBinopNode->expressions.push_back(pRight);
					return pBinopNode;
				}

				AA_AST_NODE* AssignmentNode(AA_AST_NODE* pLeft, AA_AST_NODE* pRight) {
					return BinaryOperationNode(L"=", pLeft, pRight);
				}

				AA_AST_NODE* AccessorNode(std::wstring accessorType, AA_AST_NODE* pFrom, AA_AST_NODE* pAccess) {
					AA_AST_NODE_TYPE atype = (pAccess->type == AA_AST_NODE_TYPE::funcall)?AA_AST_NODE_TYPE::callaccess : AA_AST_NODE_TYPE::fieldaccess;
					AA_AST_NODE* accessor = new AA_AST_NODE(accessorType, atype, AACodePosition::Undetermined);
					accessor->expressions.push_back(pFrom);
					accessor->expressions.push_back(pAccess);
					return accessor;
				}

				AA_AST_NODE* ThisAccessorNode(AA_AST_NODE* pAccess) {
					return AccessorNode(L".", new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::variable, AACodePosition::Undetermined), pAccess);
				}

				AA_AST_NODE* IdentifierNode(std::wstring identifier) {
					return new AA_AST_NODE(identifier, AA_AST_NODE_TYPE::variable, AACodePosition::Undetermined);
				}

				AA_AST_NODE* FieldNode(std::wstring field) {
					return new AA_AST_NODE(field, AA_AST_NODE_TYPE::field, AACodePosition::Undetermined);
				}

				AA_AST_NODE* StringLiteralNode(std::wstring lit) {
					return new AA_AST_NODE(lit, AA_AST_NODE_TYPE::stringliteral, AACodePosition::Undetermined);
				}

				AA_AST_NODE* BoolLiteralNode(bool b) {
					return new AA_AST_NODE((b)?L"true":L"false", AA_AST_NODE_TYPE::boolliteral, AACodePosition::Undetermined);
				}


				AA_AST_NODE* CallNode(std::wstring func) {
					return new AA_AST_NODE(func, AA_AST_NODE_TYPE::funcall, AACodePosition::Undetermined);
				}

				AA_AST_NODE* CtorCallNode(std::wstring func) {
					return new AA_AST_NODE(func, AA_AST_NODE_TYPE::classctorcall, AACodePosition::Undetermined);
				}

				void AddCallArgument(AA_AST_NODE* pCallNode, AA_AST_NODE* pArgNode) {
					pCallNode->expressions.push_back(pArgNode);
				}

				AA_AST_NODE* NewNode(AA_AST_NODE* pCtorCallNode) {
					AA_AST_NODE* pNewNode = new AA_AST_NODE(L"", AA_AST_NODE_TYPE::newstatement, AACodePosition::Undetermined);
					pNewNode->expressions.push_back(pCtorCallNode);
					return pNewNode;
				}

			}

		}

	}

}
