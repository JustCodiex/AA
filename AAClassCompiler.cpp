#include "AACodePosition.h"
#include "AAClassCompiler.h"
#include "AAVal.h"
#include "AA_Node_Consts.h"
#include "AAAutoCodeGenerator.h"

void AAClassCompiler::RedefineFunDecl(std::wstring className, AA_AST_NODE* pFuncDeclNode) {

	// Flag used to see if we're a constructor func
	bool isConstructor = false;

	// Is the name set to .ctor (marked as a constructor by AST)
	if (pFuncDeclNode->content == L".ctor") {
		pFuncDeclNode->content = className;
		pFuncDeclNode->expressions[AA_NODE_FUNNODE_RETURNTYPE] = new AA_AST_NODE(className, AA_AST_NODE_TYPE::typeidentifier, pFuncDeclNode->position);
		isConstructor = true;
	}

	// Fetch real function name
	pFuncDeclNode->content = className + L"::" + pFuncDeclNode->content;

	// Create 'this' reference for function decleration
	AA_AST_NODE* thisRef = new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::funarg, AACodePosition(0,0));
	thisRef->expressions.push_back(new AA_AST_NODE(className, AA_AST_NODE_TYPE::typeidentifier, AACodePosition(0, 0)));
	
	// Add 'this' reference to function decleration
	pFuncDeclNode->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions.insert(pFuncDeclNode->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions.begin(), thisRef);

	// Are we the constructor
	if (isConstructor) {

		// Set last statement to be a "return this;" statement
		AA_AST_NODE* returnThisKw = new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::variable, AACodePosition(0, 0));
		pFuncDeclNode->expressions[AA_NODE_FUNNODE_BODY]->expressions.push_back(returnThisKw);

	}

}

AAClassSignature* AAClassCompiler::FindClassFromCtor(std::wstring ctorname, aa::list<AAClassSignature*> classes) {

	for (size_t i = 0; i < classes.Size(); i++) {
		if (classes.At(i)->name == ctorname) {
			return classes.At(i);
		}
	}

	return 0;

}

AAFuncSignature* AAClassCompiler::FindBestCtor(AAClassSignature* pCC) {

	// TODO: Fix this so we may have multiple ctors
	return pCC->methods.FindFirst([](AAFuncSignature*& sig) { return sig->isClassCtor; });

}

size_t AAClassCompiler::CalculateMemoryUse(AAClassSignature* cc) {

	// Total size of class in memory
	size_t total = 0;

	// Right now we just have to account for fields
	total += cc->fields.Size() * sizeof(AAVal);

	// Return total
	return total;

}

void AAClassCompiler::CorrectReferences(aa::list<std::wstring> fields, aa::set<AAFuncSignature*> pFuncSignatures) {

	pFuncSignatures.ForEach(
		[fields, this](AAFuncSignature*& sig) {
			// Make sure there's something to verify
			if (sig->node) {
				aa::list<std::wstring> params = aa::list<AAFuncParam>(sig->parameters).Map<std::wstring>([](AAFuncParam& param) { return param.identifier; });
				this->CorrectFuncFieldReferences(fields, params, sig->node->expressions[AA_NODE_FUNNODE_BODY]);
			}
		}
	);

}

void AAClassCompiler::CorrectFuncFieldReferences(aa::list<std::wstring> fields, aa::list<std::wstring> args, AA_AST_NODE* pNode) {

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::block:
	case AA_AST_NODE_TYPE::funcbody:
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			this->CorrectFuncFieldReferences(fields, args, pNode->expressions[i]);
		}
		break;
	case AA_AST_NODE_TYPE::binop:
		this->CorrectFuncFieldReferences(fields, args, pNode->expressions[0]);
		this->CorrectFuncFieldReferences(fields, args, pNode->expressions[1]);
		break;
	case AA_AST_NODE_TYPE::unop:
		this->CorrectFuncFieldReferences(fields, args, pNode->expressions[0]);
		break;
	case AA_AST_NODE_TYPE::variable: {

		if (fields.Contains(pNode->content) && !args.Contains(pNode->content)) { // We also check if the args contain it (if it does, we wont do anything, because then it refers to the argument)
			UpdateIdentifierToThisFieldReference(pNode, fields.IndexOf(pNode->content));
		}

		break;
	}
	default:
		break;
	}

}

void AAClassCompiler::UpdateIdentifierToThisFieldReference(AA_AST_NODE* pNode, int fieldId) {

	pNode->type = AA_AST_NODE_TYPE::fieldaccess;
	pNode->expressions.push_back(new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::variable, pNode->position));
	pNode->expressions.push_back(new AA_AST_NODE(pNode->content, AA_AST_NODE_TYPE::field, pNode->position));
	pNode->expressions[pNode->expressions.size() - 1]->tags["fieldid"] = fieldId;
	pNode->tags["compiler_generated"] = true;
	pNode->content = L"."; // We remember to set the field access type

}

bool AAClassCompiler::AddInheritanceCall(AAClassSignature* cc, AA_AST_NODE* pCtorDeclNode) {

	return cc->extends.ForAll(
		[pCtorDeclNode, this](AAClassSignature*& sig) {
			if (!aa::modifiers::ContainsFlag(sig->storageModifier, AAStorageModifier::ABSTRACT)) { // We cannot call the instructor of an abstract class
				AAFuncSignature* ctor = this->FindBestCtor(sig);
				if (ctor) {
					return this->AddInheritanceCallNode(ctor, pCtorDeclNode);
				} else {
					return false;
				}
			}
		}
	);

}

bool AAClassCompiler::AddInheritanceCallNode(AAFuncSignature* ctor, AA_AST_NODE* pCtorDeclNode) {

	if (ctor) {

		AA_AST_NODE* pCallNode = new AA_AST_NODE(ctor->name, AA_AST_NODE_TYPE::classctorcall, pCtorDeclNode->position);
		pCallNode->tags["compiler_generated"] = true;
		pCallNode->tags["inheritance_call"] = true;
		pCallNode->tags["calls"] = (int)ctor->procID;
		pCallNode->tags["isVM"] = ctor->isVMFunc;
		pCallNode->tags["args"] = (int)ctor->parameters.size();
		pCallNode->tags["returns"] = 1; // Ctor calls will always return 1

		AA_AST_NODE* pThisArgNode = new AA_AST_NODE(L"this", AA_AST_NODE_TYPE::variable, pCtorDeclNode->position);
		pThisArgNode->tags["compiler_generated"] = true;
		pCallNode->expressions.push_back(pThisArgNode);

		// Add inheritance call node to start
		pCtorDeclNode->expressions[AA_NODE_FUNNODE_BODY]->expressions.insert(pCtorDeclNode->expressions[AA_NODE_FUNNODE_BODY]->expressions.begin(), pCallNode);

		return true;

	} else {

		return false;

	}

}

bool AAClassCompiler::AutoTaggedClass(AAClassSignature* taggedClassSignature, AA_AST_NODE*& pOutClassFuncCtor) {

	using namespace aa::compiler::generator;
	using namespace aa::compiler::generator::expr;

	/*

		Å Code:
		tagged class Class(<...>);

		Expands to:

		class Class {
			<...> // fields
			Class(Class this, <...>) {
				this.<...> = <...>;
			}
			bool Equals(Class _c){
				this.<...> == _c.<...>;
			}
			string ToString(){
				#nameof(Class) + "(" + #beautystring(<...>) + ")";
			}
		}

		Class Class(<...>){
			new Class(<...>);
		}

	*/

	// Get the fields
	std::vector<__name_type> fields = this->GetTaggedFields(taggedClassSignature->pSourceNode);

	// Class functions
	AA_AST_NODE* classCtor = NewFunctionNode(L".ctor", taggedClassSignature->name, true);
	AddFunctionArguments(classCtor, fields);

	// Class equal method
	AA_AST_NODE* classEquals = NewFunctionNode(L"Equals", L"bool", true);
	AddFunctionArgument(classEquals, L"_other", taggedClassSignature->name);

	// Class to string method
	AA_AST_NODE* classToString = NewFunctionNode(L"ToString", L"string", true);
	AA_AST_NODE* funcToStringBinop = BinaryOperationNode(L"+", StringLiteralNode(taggedClassSignature->name + L"("), StringLiteralNode(L")"));
	AddFunctionBodyElement(classToString, funcToStringBinop);

	// The class constructor that eliminates 'new' usage
	pOutClassFuncCtor = NewFunctionNode(taggedClassSignature->name, taggedClassSignature->name, true);
	AddFunctionArguments(pOutClassFuncCtor, fields);
	AA_AST_NODE* ctorCall = CtorCallNode(taggedClassSignature->name);
	AA_AST_NODE* newStatement = NewNode(ctorCall);
	AddFunctionBodyElement(pOutClassFuncCtor, newStatement);

	// flag for adding comma
	bool commaflg = false;

	// Contains all the nodes where an equal operation is used
	std::vector<AA_AST_NODE*> equalsNode;

	// For all fields
	for (auto& field : fields) {

		// Add the assignment node
		AA_AST_NODE* fieldAssign = AssignmentNode(ThisAccessorNode(IdentifierNode(field.first)), IdentifierNode(field.first));
		AddFunctionBodyElement(classCtor, fieldAssign);

		// String node
		AA_AST_NODE* strAssign = (commaflg) ? BinaryOperationNode(L"+", StringLiteralNode(L", "), IdentifierNode(field.first)) : IdentifierNode(field.first);
		commaflg = true;

		// Add field name in stringification
		funcToStringBinop->expressions[1] = BinaryOperationNode(L"+", strAssign, funcToStringBinop->expressions[1]);

		// Add field as argument to no-new class call
		AddCallArgument(ctorCall, IdentifierNode(field.first));

		// Create equals node
		AA_AST_NODE* eq = BinaryOperationNode(L"==", ThisAccessorNode(IdentifierNode(field.first)), AccessorNode(L".", IdentifierNode(L"_other"), IdentifierNode(field.first)));
		equalsNode.push_back(eq);

	}

	if (equalsNode.size() > 0) {
		AA_AST_NODE* finalEqualsNode = equalsNode[0];
		for (size_t i = 1; i < equalsNode.size(); i++) {
			finalEqualsNode = BinaryOperationNode(L"&&", finalEqualsNode, equalsNode[i]);
		}
		AddFunctionBodyElement(classEquals, finalEqualsNode);
	} else {
		AddFunctionBodyElement(classEquals, BoolLiteralNode(true));
	}

	// Add all the nodes
	taggedClassSignature->pSourceNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions.push_back(classCtor);
	taggedClassSignature->pSourceNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions.push_back(classEquals);
	taggedClassSignature->pSourceNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions.push_back(classToString);

	// Return true
	return true;

}

std::vector<aa::compiler::generator::__name_type> AAClassCompiler::GetTaggedFields(AA_AST_NODE* pClassSource) {

	std::vector<aa::compiler::generator::__name_type> result;

	if (AA_NODE_CLASSNODE_BODY < pClassSource->expressions.size()) {

		for (auto& field : pClassSource->expressions[AA_NODE_CLASSNODE_BODY]->expressions) {
			if (field->type == AA_AST_NODE_TYPE::vardecl && field->HasTag("ctor_field")) {
				result.push_back(aa::compiler::generator::__name_type(field->content, field->expressions[0]->content));
			}
		}

	}

	return result;

}

bool AAClassCompiler::HasField(AAClassSignature* cc, std::wstring fieldname, int& fieldID) {
	return cc->fields.FindFirstIndex([fieldname](AAClassFieldSignature& sig) { return sig.name == fieldname; }, fieldID);
}
