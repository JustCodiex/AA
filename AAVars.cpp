#include "AAVars.h"
#include "AAOperator.h"
#include "astring.h"

using aa::VarsEnviornment;

bool AAVars::Vars(AA_AST* pTree) {

	// Set last error to none
	m_error = NO_COMPILE_ERROR_MESSAGE;

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
		venv[pScope->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions[argCount - i - 1]->content] = aa::Var(venv.size(), true);
	}

	// Make sure there's a body to work with (it may be an abstract function)
	if (aa::parsing::Function_HasBody(pScope)) {
		if (!this->VarsScope(venv, pScope->expressions[AA_NODE_FUNNODE_BODY])) {
			return false;
		}
	}

	venv = _venv;

	return true;

}

bool AAVars::VarsClass(VarsEnviornment& venv, AA_AST_NODE* pScope) {

	// Backup current environment
	VarsEnviornment _venv = VarsEnviornment(venv);

	// Make sure the class actually has a body
	if (aa::parsing::Class_HasBody(pScope)) {

		// Run through all elements in class
		for (size_t i = 0; i < pScope->expressions[AA_NODE_CLASSNODE_BODY]->expressions.size(); i++) {

			// Run vars on all functions
			if (pScope->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]->type == AA_AST_NODE_TYPE::fundecl) {
				if (!this->VarsFunction(venv, pScope->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i])) {
					return false;
				}
			}

		}

	}

	// Restore enviornment
	venv = _venv;

	// Return true
	return true;

}

bool AAVars::VarsEnum(aa::VarsEnviornment& venv, AA_AST_NODE* pScope) {
	
	// Backup current environment
	VarsEnviornment _venv = VarsEnviornment(venv);

	// Make sure the class actually has a body
	if (aa::parsing::Enum_HasBody(pScope)) {

		// Run through all elements in class
		for (size_t i = 0; i < pScope->expressions[AA_NODE_ENUMNODE_BODY]->expressions.size(); i++) {

			// Run vars on all functions
			if (pScope->expressions[AA_NODE_ENUMNODE_BODY]->expressions[i]->type == AA_AST_NODE_TYPE::fundecl) {
				if (!this->VarsFunction(venv, pScope->expressions[AA_NODE_ENUMNODE_BODY]->expressions[i])) {
					return false;
				}
			}

		}

	}

	// Restore enviornment
	venv = _venv;

	// Return true
	return true;

}

bool AAVars::VarsNamespace(aa::VarsEnviornment& venv, AA_AST_NODE* pScope) {

	VarsEnviornment _venv = VarsEnviornment(venv);

	for (size_t i = 0; i < pScope->expressions.size(); i++) {

		if (!this->VarsNode(venv, pScope->expressions[i])) {
			return false;
		}

	}

	venv = _venv;

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

	// Copy environment
	VarsEnviornment _venv = VarsEnviornment(venv);

	// Vars the for statement
	if (!this->VarsNode(venv, pScope->expressions[AA_NODE_FOR_INIT]) | !this->VarsNode(venv, pScope->expressions[AA_NODE_FOR_CONDITION])
		| !this->VarsNode(venv, pScope->expressions[AA_NODE_FOR_AFTERTHOUGHT])) {
		return false;
	}

	// Vars body scope
	if (!this->VarsScope(venv, pScope->expressions[AA_NODE_FOR_BODY])) {
		return false;
	}

	// Restore environment
	venv = _venv;

	// Return true
	return true;

}

bool AAVars::VarsMatchCase(VarsEnviornment& venv, AA_AST_NODE* pScope) {

	// Cpoy current envionment
	VarsEnviornment _venv = VarsEnviornment(venv);

	// Save the scope size - so the compiler can determine if a variable is introduced or not
	pScope->tags["scope_to_now"] = venv.size();

	// For all condition elements
	for (size_t i = 0; i < pScope->expressions[0]->expressions.size(); i++) {
		if (!this->VarsMatchCondition(venv, pScope->expressions[0]->expressions[i])) {
			return false;
		}
	}

	// Now run vars on the scope
	if (!this->VarsNode(venv, pScope->expressions[1])) {
		return false;
	}

	// Restore environment
	venv = _venv;

	// Return true
	return true;

}

bool AAVars::VarsMatchCondition(aa::VarsEnviornment& venv, AA_AST_NODE* pScope) {

	if (pScope->type == AA_AST_NODE_TYPE::variable) {
		if (pScope->content.compare(L"_") != 0) { // Make sure it's not the wildcard				
			// In the condition, it's actually a variable declaration
			pScope->tags["varsi"] = venv[pScope->content] = aa::Var(venv.size(), false);
		} else {
			pScope->tags["varsi"] = venv[pScope->content] = aa::Var(venv.size(), false);
		}
	} else if (pScope->type == AA_AST_NODE_TYPE::objdeconstruct) {
		for (size_t i = 0; i < pScope->expressions.size(); i++) {
			this->VarsMatchCondition(venv, pScope->expressions[i]);
		}
	} else if (pScope->type == AA_AST_NODE_TYPE::tupleval) {
		for (size_t i = 0; i < pScope->expressions.size(); i++) {
			this->VarsMatchCondition(venv, pScope->expressions[i]);
		}
	}

	return true;

}

bool AAVars::VarsAssignment(aa::VarsEnviornment& venv, AA_AST_NODE* pScope) {

	// First we need to treat it normally
	bool r = this->VarsNode(venv, pScope->expressions[1]) & this->VarsNode(venv, pScope->expressions[0]);

	// Exit early => don't overwrite error messages
	if (!r) {
		return false;
	}

	// Now we can check if we're doing something to a non-mutable variable
	if (pScope->expressions[0]->type == AA_AST_NODE_TYPE::variable) {
		if (!venv[pScope->expressions[0]->content].isMutable) {
			m_error = AAC_CompileErrorMessage(
				aa::compiler_err::C_Vars_ConstMutation,
				"Attempt to reassign value of constant variable '" + string_cast(pScope->expressions[0]->content) + "'", 
				pScope->position				
			);
			return false;
		}
	}

	return true;

}

bool AAVars::VarsUnary(aa::VarsEnviornment& venv, AA_AST_NODE* pScope) {

	// Run vars on this
	bool result = this->VarsNode(venv, pScope->expressions[0]);

	// Early exit in case of error
	if (!result) {
		return false;
	}

	bool isIncrement = pScope->content.compare(L"++") == 0;
	bool isDecrement = pScope->content.compare(L"--") == 0;
	if (pScope->expressions[0]->type == AA_AST_NODE_TYPE::variable && (isIncrement || isDecrement)) {
		if (!venv[pScope->expressions[0]->content].isMutable) {
			m_error = AAC_CompileErrorMessage(
				aa::compiler_err::C_Vars_ConstMutation,
				"Attempt to " + std::string((isDecrement) ? "decrement" : "increment") + " value of constant variable '" + string_cast(pScope->expressions[0]->content) + "'",
				pScope->position
			);
			return false;
		}
	}

	return true;

}

bool AAVars::VarsLambdaExpression(aa::VarsEnviornment& venv, AA_AST_NODE* pScope) {

	VarsEnviornment _venv = VarsEnviornment(venv);

	// Register lambda params
	for (size_t i = 0; i < pScope->expressions[0]->expressions.size(); i++) { // Note: Capturing variables will have to be handled somewhere else (and correct variable indices)
		pScope->expressions[0]->expressions[i]->tags["varsi"] = venv[pScope->expressions[0]->expressions[i]->content] = aa::Var(venv.size(), false);
	}

	// Vars run on nodes
	if (!this->VarsNode(venv, pScope->expressions[1])) {
		return false;
	}

	venv = _venv;

	return true;

}

bool AAVars::VarsNode(VarsEnviornment& venv, AA_AST_NODE* pScope) {
	if (pScope->type == AA_AST_NODE_TYPE::compile_unit ||  pScope->type == AA_AST_NODE_TYPE::block || 
		pScope->type == AA_AST_NODE_TYPE::funcbody || pScope->type == AA_AST_NODE_TYPE::lambdabody) {
		return this->VarsScope(venv, pScope);
	} else if (pScope->type == AA_AST_NODE_TYPE::fundecl) {
		return this->VarsFunction(venv, pScope);
	} else if (pScope->type == AA_AST_NODE_TYPE::classdecl) {
		return this->VarsClass(venv, pScope);
	} else if (pScope->type == AA_AST_NODE_TYPE::enumdecleration) {
		return this->VarsEnum(venv, pScope);
	} else if (pScope->type == AA_AST_NODE_TYPE::name_space) {
		return this->VarsNamespace(venv, pScope);
	} else {
		switch (pScope->type) {
		case AA_AST_NODE_TYPE::binop: // Because of free/bound variables we have to evaluate RHS before LHS
			if (aa::op::IsAssignmentOperator(pScope->content)) {
				return this->VarsAssignment(venv, pScope);
			} else {
				return this->VarsNode(venv, pScope->expressions[1]) & this->VarsNode(venv, pScope->expressions[0]);
			}
		case AA_AST_NODE_TYPE::unop_pre:
		case AA_AST_NODE_TYPE::unop_post:
			return VarsUnary(venv, pScope);
		case AA_AST_NODE_TYPE::boolliteral:
		case AA_AST_NODE_TYPE::charliteral:
		case AA_AST_NODE_TYPE::floatliteral:
		case AA_AST_NODE_TYPE::intliteral:
		case AA_AST_NODE_TYPE::nullliteral:
		case AA_AST_NODE_TYPE::stringliteral:
		case AA_AST_NODE_TYPE::enumvalueaccess:
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
		case AA_AST_NODE_TYPE::fieldaccess: {
			return this->VarsNode(venv, pScope->expressions[0]);
		}
		case AA_AST_NODE_TYPE::tuplevardecl:
		case AA_AST_NODE_TYPE::lambdadecl:
		case AA_AST_NODE_TYPE::vardecl: {
			pScope->tags["varsi"] = venv[pScope->content] = aa::Var(venv.size(), !pScope->HasTag("modifier_const"));
			return true;
		}
		case AA_AST_NODE_TYPE::callaccess: 
			return this->VarsNode(venv, pScope->expressions[0]) && this->VarsNode(venv, pScope->expressions[1]);
		case AA_AST_NODE_TYPE::classctorcall:
		case AA_AST_NODE_TYPE::funcall: {
			// if function identifier is valid --> do this when closures and such have been introduced
			for (size_t i = 0; i < pScope->expressions.size(); i++) {
				if (!this->VarsNode(venv, pScope->expressions[i])) {
					return false;
				}
			}
			return true;
		}
		case AA_AST_NODE_TYPE::lambdacall: {
			for (size_t i = 0; i < pScope->expressions.size(); i++) {
				if (!this->VarsNode(venv, pScope->expressions[i])) {
					return false;
				}
			} // find the variable
			auto lookup = venv.find(pScope->content);
			if (lookup != venv.end()) {
				pScope->tags["varsi"] = (*lookup).second;
				return true;
			} else {
				return false;
			}
		}
		case AA_AST_NODE_TYPE::lambdaexpression:
			return this->VarsLambdaExpression(venv, pScope);
		case AA_AST_NODE_TYPE::ifstatement:
			return this->VarsBranch(venv, pScope);
		case AA_AST_NODE_TYPE::forinit:
		case AA_AST_NODE_TYPE::forafterthought:
		case AA_AST_NODE_TYPE::condition:
		case AA_AST_NODE_TYPE::matchcaselist:
			for (size_t i = 0; i < pScope->expressions.size(); i++) {
				if (!this->VarsNode(venv, pScope->expressions[i])) {
					return false;
				}
			}
			return true;
		case AA_AST_NODE_TYPE::forstatement:
			return this->VarsForLoop(venv, pScope);
		case AA_AST_NODE_TYPE::whilestatement: {
			VarsEnviornment _venv = VarsEnviornment(venv);
			if (!this->VarsNode(venv, pScope->expressions[AA_NODE_IF_CONDITION]) | !this->VarsNode(venv, pScope->expressions[AA_NODE_IF_BODY])) {
				return false;
			}
			venv = _venv;
			return true;
		}
		case AA_AST_NODE_TYPE::dowhilestatement: {
			// Simple ==> scope followed by some condition (neither can depend on variables defined inside each other)
			VarsEnviornment _venv = VarsEnviornment(venv);
			if (!this->VarsNode(venv, pScope->expressions[AA_NODE_IF_BODY])) {
				return false;
			}
			venv = _venv; // forget whatever was found in the scope
			if (!this->VarsScope(venv, pScope->expressions[AA_NODE_IF_CONDITION])) {
				return false;
			}
			venv = _venv;
			return true;
		}
		case AA_AST_NODE_TYPE::newstatement:
			if (pScope->expressions[0]->type == AA_AST_NODE_TYPE::index) {
				return this->VarsNode(venv, pScope->expressions[0]->expressions[1]); // cant just let next case handle it, as the x[y] will have x = some type and not a variable.
			} else {
				return this->VarsNode(venv, pScope->expressions[0]);
			}
		case AA_AST_NODE_TYPE::index:
			if (pScope->expressions.size() == 2) {
				return this->VarsNode(venv, pScope->expressions[0]) && this->VarsNode(venv, pScope->expressions[1]);
			} else {
				return false;
			}
		case AA_AST_NODE_TYPE::memberaccess:
		case AA_AST_NODE_TYPE::usingspecificstatement:
		case AA_AST_NODE_TYPE::usingstatement:
			return true; // nothing to do in these cases
		case AA_AST_NODE_TYPE::matchstatement:
			return this->VarsNode(venv, pScope->expressions[0]) && this->VarsNode(venv, pScope->expressions[1]);
		case AA_AST_NODE_TYPE::matchcasestatement:
			return this->VarsMatchCase(venv, pScope);
		case AA_AST_NODE_TYPE::tupleval:
			for (size_t i = 0; i < pScope->expressions.size(); i++) {
				if (!this->VarsNode(venv, pScope->expressions[i])) {
					return false;
				}
			}
			return true;
		case AA_AST_NODE_TYPE::tupleaccess:
			return this->VarsNode(venv, pScope->expressions[0]); // We only need to do vars on the tuple reference - not the index
		default:
			break;
		}
		return false;
	}
}
