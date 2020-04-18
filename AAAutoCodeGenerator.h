#pragma once
#include "AA_AST_NODE.h"
#include "AACNamespace.h"

namespace aa {
	namespace compiler {
		namespace generator {

			struct __name_type {
				std::wstring first; // field
				std::wstring second; // type
				__name_type() {}
				__name_type(std::wstring f, std::wstring s) {
					this->first = f;
					this->second = s;
				}
			};

			/// <summary>
			/// Createsa a new function declaration node
			/// </summary>
			/// <param name="name">Name of function</param>
			/// <param name="rt">Return type of function</param>
			/// <param name="addBody">Add a body node (if true)</param>
			/// <returns>Node with base function data</returns>
			AA_AST_NODE* NewFunctionNode(std::wstring name, std::wstring rt, bool addBody);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="pFuncDeclNode"></param>
			/// <param name="argName"></param>
			/// <param name="argType"></param>
			/// <returns></returns>
			void AddFunctionArgument(AA_AST_NODE* pFuncDeclNode, std::wstring argName, std::wstring argType);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="pFuncDeclNode"></param>
			/// <param name="args"></param>
			void AddFunctionArguments(AA_AST_NODE* pFuncDeclNode, std::vector<__name_type> args);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="pFuncDeclNode"></param>
			/// <param name="pNode"></param>
			void AddFunctionBodyElement(AA_AST_NODE* pFuncDeclNode, AA_AST_NODE* pNode);

			namespace expr {

				AA_AST_NODE* BinaryOperationNode(std::wstring op, AA_AST_NODE* pLeft, AA_AST_NODE* pRight);

				AA_AST_NODE* AssignmentNode(AA_AST_NODE* pLeft, AA_AST_NODE* pRight);

				AA_AST_NODE* AccessorNode(std::wstring method, AA_AST_NODE* pFrom, AA_AST_NODE* pAccess);

				AA_AST_NODE* ThisAccessorNode(AA_AST_NODE* pAccess);

				AA_AST_NODE* CallNode(std::wstring func);

				AA_AST_NODE* CtorCallNode(std::wstring func);

				void AddCallArgument(AA_AST_NODE* pCallNode, AA_AST_NODE* pArgNode);

				AA_AST_NODE* NewNode(AA_AST_NODE* pCtorCallNode);

				AA_AST_NODE* IdentifierNode(std::wstring identifier);

				AA_AST_NODE* StringLiteralNode(std::wstring lit);

				AA_AST_NODE* BoolLiteralNode(bool b);

			}

		}
	}
}
