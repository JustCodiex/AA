#include "AAVars.h"

using aa::VarsEnviornment;

bool AAVars::Vars(AA_AST* pTree) {

	// Get the root
	AA_AST_NODE* pRoot = pTree->GetRoot();

	// Base variable environment
	VarsEnviornment baseVenv;

	// Run the vars scope
	if (!this->VarsNode(baseVenv, pRoot)) {
		return false;
	}

	// Return true --> no scope errors
	return true;

}

bool AAVars::VarsScope(VarsEnviornment& venv, AA_AST_NODE* pScope) {

	// Copy environment before entering scope
	VarsEnviornment _venv = VarsEnviornment(venv);

	// For all expressions in block
	for (size_t i = 0; i < pScope->expressions.size(); i++) {

		// Run vars check on expression node
		if (!this->VarsNode(venv, pScope->expressions[i])) {
			return false;
		}

	}

	// Restore environment
	venv = _venv;

	// Return true --> No errors
	return true;

}

bool AAVars::VarsFunction(VarsEnviornment& venv, AA_AST_NODE* pScope) {

	VarsEnviornment _venv = VarsEnviornment(venv);
	const size_t argCount = pScope->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions.size();

	for (size_t i = 0; i < argCount; i++) {
		venv[pScope->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions[argCount - i - 1]->content] = venv.size();
	}

	if (pScope->expressions.size() >= AA_NODE_FUNNODE_BODY) {

		if (!this->VarsScope(venv, pScope->expressions[AA_NODE_FUNNODE_BODY])) {
			return false;
		}

	}

	venv = _venv;

	return true;

}

bool AAVars::VarsClass(VarsEnviornment& venv, AA_AST_NODE* pScope) {

	return true;

}

bool AAVars::VarsBranch(VarsEnviornment& venv, AA_AST_NODE* pScope) {

	VarsEnviornment _venv = VarsEnviornment(venv);

	if (!this->VarsNode(venv, pScope->expressions[AA_NODE_IF_CONDITION]) | !this->VarsNode(venv, pScope->expressions[AA_NODE_IF_BODY])) {
		return false;
	}

	for (size_t i = 2; i < pScope->expressions.size(); i++) {
		if (pScope->expressions[i]->type == AA_AST_NODE_TYPE::elsestatement) {
			if (!this->VarsNode(venv, pScope->expressions[i]->expressions[0])) {
				return false;
			}
		} else {
			if (!this->VarsNode(venv, pScope->expressions[i]->expressions[AA_NODE_IF_CONDITION]) | !this->VarsNode(venv, pScope->expressions[i]->expressions[AA_NODE_IF_BODY])) {
				return false;
			}
		}
	}

	venv = _venv;

	return true;

}

bool AAVars::VarsForLoop(VarsEnviornment& venv, AA_AST_NODE* pScope) {

	VarsEnviornment _venv = VarsEnviornment(venv);



	venv = _venv;

	return true;

}

bool AAVars::VarsWhileLoop(aa::VarsEnviornment& venv, AA_AST_NODE* pCondition, AA_AST_NODE* pBody) {

	VarsEnviornment _venv = VarsEnviornment(venv);



	venv = _venv;

	return true;

}

bool AAVars::VarsNode(VarsEnviornment& venv, AA_AST_NODE* pScope) {
	if (pScope->type == AA_AST_NODE_TYPE::block || pScope->type == AA_AST_NODE_TYPE::funcbody) {
		return this->VarsScope(venv, pScope);
	} else if (pScope->type == AA_AST_NODE_TYPE::fundecl) {
		return this->VarsFunction(venv, pScope);
	} else if (pScope->type == AA_AST_NODE_TYPE::classdecl) {
		return this->VarsClass(venv, pScope);
	} else {
		switch (pScope->type) {
		case AA_AST_NODE_TYPE::binop: // Because of free/bound variables we have to evaluate RHS before LHS
			return this->VarsNode(venv, pScope->expressions[1]) & this->VarsNode(venv, pScope->expressions[0]);
		case AA_AST_NODE_TYPE::unop:
			return this->VarsNode(venv, pScope->expressions[0]);
		case AA_AST_NODE_TYPE::boolliteral:
		case AA_AST_NODE_TYPE::charliteral:
		case AA_AST_NODE_TYPE::floatliteral:
		case AA_AST_NODE_TYPE::intliteral:
		case AA_AST_NODE_TYPE::nullliteral:
		case AA_AST_NODE_TYPE::stringliteral:
			return true;
		case AA_AST_NODE_TYPE::variable: {
			auto lookup = venv.find(pScope->content);
			if (lookup != venv.end()) {
				pScope->tags["varsi"] = (*lookup).second;
				return true;
			} else {
				return false;
			}
		}
		case AA_AST_NODE_TYPE::vardecl:
			venv[pScope->content] = venv.size();
			return true;
		case AA_AST_NODE_TYPE::callaccess: 
			return this->VarsNode(venv, pScope->expressions[0]) && this->VarsNode(venv, pScope->expressions[1]);
		case AA_AST_NODE_TYPE::funcall: {
			// if function identifier is valid --> do this when closures and such have been introduced
			for (size_t i = 0; i < pScope->expressions.size(); i++) {
				if (!this->VarsNode(venv, pScope->expressions[i])) {
					return false;
				}
			}
			return true;
		}
		case AA_AST_NODE_TYPE::ifstatement: {
			return this->VarsBranch(venv, pScope);
		}
		case AA_AST_NODE_TYPE::condition:
			for (size_t i = 0; i < pScope->expressions.size(); i++) {
				if (!this->VarsNode(venv, pScope->expressions[i])) {
					return false;
				}
			}
			return true;
		case AA_AST_NODE_TYPE::forstatement:
			return this->VarsForLoop(venv, pScope);
		case AA_AST_NODE_TYPE::whilestatement:
			return this->VarsWhileLoop(venv, pScope->expressions[AA_NODE_IF_CONDITION], pScope->expressions[AA_NODE_IF_BODY]);
			break;
		case AA_AST_NODE_TYPE::dowhilestatement: {
			// Simple ==> scope followed by some condition (neither can depend on variables defined inside each other)
			VarsEnviornment _venv = VarsEnviornment(venv);
			if (!this->VarsNode(venv, pScope->expressions[AA_NODE_IF_CONDITION]) | !this->VarsScope(venv, pScope->expressions[AA_NODE_IF_BODY])) {
				return false;
			} else {
				venv = _venv;
				return true;
			}
		}
		default:
			break;
		}
		return false;
	}
}
