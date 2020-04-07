#include "AAStaticAnalysis.h"
#include "astring.h"
#include "AAC.h"

aa::list<AACType*> _getdeftypeenv() {
	aa::list<AACType*> types;
	types.Add(AACTypeDef::Bool);
	types.Add(AACTypeDef::Char);
	types.Add(AACTypeDef::Float32);
	types.Add(AACTypeDef::Int32);
	return types;
}

AAStaticAnalysis::AAStaticAnalysis(AAC* pCompiler) {

	// Set pointer to compiler
	this->m_compilerPointer = pCompiler;

}

void AAStaticAnalysis::Reset(std::vector<AAFuncSignature*> funcs, std::vector<AAClassSignature*> classes) {

	// Copy functions
	m_preregisteredFunctions.FromVector(funcs);

	// Copy classes
	m_preregisteredClasses.FromVector(classes);

}

AAC_CompileErrorMessage AAStaticAnalysis::RunStaticAnalysis(std::vector<AA_AST*>& trees) {

	// Compile error container
	AAC_CompileErrorMessage err;

	// Extract the global scope
	this->ExtractGlobalScope(trees);

	// The global domain
	AACNamespace* globalDomain = 0;

	// Get the static environment
	AAStaticEnvironment senv = this->NewStaticEnvironment(globalDomain);

	// Fetch static declerations
	if (COMPILE_ERROR(err = this->FetchStaticDeclerationsFromTrees(trees, globalDomain, senv))) {
		return err;
	}

	// For all input trees, run static type checker
	for (size_t i = 0; i < trees.size(); i++) {

		// Any possible type error
		AATypeChecker::Error tErr;

		// Register functions
		if (!this->RunTypecheckAnalysis(trees[i], senv, tErr)) {

			// Create error message based on type error
			err.errorMsg = tErr.errMsg;
			err.errorSource = tErr.errSrc;
			err.errorType = tErr.errType;

			// Return error message
			return err;

		}

	}

	// For all input trees, optimize
	for (size_t i = 0; i < trees.size(); i++) {

		// Simplify the abstract tree
		trees[i]->Simplify();

	}

	// Update last static environment
	m_lastStaticEnv = senv;

	// Return no errors
	return NO_COMPILE_ERROR_MESSAGE;

}

bool AAStaticAnalysis::RunTypecheckAnalysis(AA_AST* pTree, AAStaticEnvironment& senv, AATypeChecker::Error& typeError) {

	// Run a complete type-check on the tree
	AATypeChecker checker = AATypeChecker(pTree, &senv);

	// Run type checker
	if (!checker.TypeCheck()) {

		// Get error
		typeError = checker.GetErrorMessage();

		// Return false => there were errors
		return false;

	} else {

		// Return true => no errors
		return true;

	}

}

AAStaticEnvironment AAStaticAnalysis::NewStaticEnvironment(AACNamespace*& globalDomain) {

	// Create the global domain
	globalDomain = new AACNamespace(L"", NULL);

	// Static environment
	AAStaticEnvironment env;

	// Static function checks
	env.availableTypes.FromList(_getdeftypeenv());

	// Add all available types to the global domain
	if (!env.availableTypes.ForAll([globalDomain](AACType*& type) { return globalDomain->AddType(type); })) {
		throw std::exception("Failed to add primitive types to the global domain!");
	}

	// For all preloaded classes
	for (size_t i = 0; i < this->m_preregisteredClasses.Size(); i++) {

		// Add class and class type to domain
		globalDomain->AddClass(this->m_preregisteredClasses.Apply(i));

		// Add type name
		env.availableTypes.Add(this->m_preregisteredClasses.Apply(i)->type);

		// Add the class
		env.availableClasses.Add(this->m_preregisteredClasses.Apply(i));

	}

	// For all preloaded functions
	for (size_t i = 0; i < this->m_preregisteredFunctions.Size(); i++) {

		// Add function to domain
		globalDomain->AddFunction(this->m_preregisteredFunctions.Apply(i));

		// Add function
		env.availableFunctions.Add(this->m_preregisteredFunctions.Apply(i));

	}

	// Return static environment
	return env;

}

bool AAStaticAnalysis::ExtractGlobalScope(std::vector<AA_AST*>& trees) {

	size_t i = 0;
	AA_AST_NODE* pGlobalScope = new AA_AST_NODE(L"<GlobalScope>", AA_AST_NODE_TYPE::block, AACodePosition(0, 0));

	while (i < trees.size()) {

		AA_AST_NODE_TYPE nodeType = trees[i]->GetRoot()->type;

		if (nodeType != AA_AST_NODE_TYPE::fundecl && nodeType != AA_AST_NODE_TYPE::classdecl &&
			nodeType != AA_AST_NODE_TYPE::usingspecificstatement && nodeType != AA_AST_NODE_TYPE::usingstatement && // Using statements need to preserve their position in code
			nodeType != AA_AST_NODE_TYPE::name_space) { // namespaces are per definition not part of the global namespace/scope

			pGlobalScope->expressions.push_back(trees[i]->GetRoot());
			trees.erase(trees.begin() + i);

		} else {
			i++;
		}

	}

	if (pGlobalScope->expressions.size() > 0) {
		trees.insert(trees.begin(), new AA_AST(pGlobalScope));
	}

	// Return true if there were more than one element in the global scope
	return pGlobalScope->expressions.size() > 0;

}

AAC_CompileErrorMessage AAStaticAnalysis::FetchStaticDeclerationsFromTrees(std::vector<AA_AST*> trees, AACNamespace* globalDomain, AAStaticEnvironment& senv) {

	// Error container
	AAC_CompileErrorMessage err;

	// Fetch static declerations from all tree roots, register (but do not verify) their types
	for (size_t i = 0; i < trees.size(); i++) {

		// Fetch static decleration from node and put it into the global domain. Return compile error if any
		if (COMPILE_ERROR(err = this->PreregisterTypes(trees[i]->GetRoot(), globalDomain, senv))) {
			return err;
		}
	}

	// Fetch static declerations from all tree roots
	for (size_t i = 0; i < trees.size(); i++) {

		// Fetch static decleration from node and put it into the global domain. Return compile error if any
		if (COMPILE_ERROR(err = this->FetchStaticDeclerationsFromASTNode(trees[i]->GetRoot(), globalDomain, senv))) {
			return err;
		}
	}

	// Set the global namespace
	senv.globalNamespace = globalDomain;

	// no compile error messages
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::PreregisterTypes(AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv) {

	// Error container
	AAC_CompileErrorMessage err;

	// Get the node type
	AA_AST_NODE_TYPE rootType = pNode->type;

	// Is it a namespace
	if (rootType == AA_AST_NODE_TYPE::name_space) {

		// Create domain
		AACNamespace* subDomain = new AACNamespace(pNode->content, domain);

		// For all sub-elements of domain
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			// Fetch static decleration from node and put it into the global domain. Return compile error if any
			if (COMPILE_ERROR(err = this->PreregisterTypes(pNode->expressions[i], subDomain, senv))) {
				return err;
			}
		}

		// Add subdomain to owning domain
		domain->childspaces.Add(subDomain);

	} else if (rootType == AA_AST_NODE_TYPE::classdecl) {

		// Create a barebone class signature (will be modified later)
		AAClassSignature* classSig = new AAClassSignature(pNode->content);
		
		// Add to domain
		if (domain->AddClass(classSig)) {

			if (domain->IsGlobalSpace()) {
				senv.availableClasses.Add(classSig);
				senv.availableTypes.Add(classSig->type);
			}

		} else { // Already exists in domain -> redefinition NOT allowed

			err.errorMsg = "Duplicate class definition found";
			err.errorSource = pNode->position;
			err.errorType = 1;
			return err;

		}

	} else if (rootType == AA_AST_NODE_TYPE::vardecl) {

		wprintf(L"Not implemented. AAStaticAnalysis.Cpp@%i", __LINE__);

	}

	// No compile errors
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::FetchStaticDeclerationsFromASTNode(AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv) {

	// Error container
	AAC_CompileErrorMessage err;

	// Get the node type
	AA_AST_NODE_TYPE rootType = pNode->type;

	// Is it a namespace
	if (rootType == AA_AST_NODE_TYPE::name_space) {

		// Create domain
		AACNamespace* subDomain = domain->childspaces.FindFirst([pNode](AACNamespace*& d) { return d->name.compare(pNode->content) == 0; });
		
		// For all sub-elements of domain
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			// Fetch static decleration from node and put it into the global domain. Return compile error if any
			if (COMPILE_ERROR(err = this->FetchStaticDeclerationsFromASTNode(pNode->expressions[i], subDomain, senv))) {
				return err;
			}
		}

	} else if (rootType == AA_AST_NODE_TYPE::classdecl) {

		// Get the class
		AAClassSignature* cc;
		
		// Register class and make sure it compiles OK
		if (COMPILE_OK(err = this->RegisterClass(pNode, cc, domain, senv))) {

			// Add all methods to domain
			if (!cc->methods.ForAll([domain](AAFuncSignature*& sig) { return domain->AddFunction(sig); })) {
				err.errorMsg = "Duplicate method definition found";
				err.errorSource = pNode->position;
				err.errorType = 1;
				return err;
			}

			// If in the global space we'll add it directly to the static checks
			if (domain->IsGlobalSpace()) {

				// Register class methods into static environment as well
				for (size_t j = 0; j < cc->methods.Size(); j++) {

					// Register method
					if (!senv.availableFunctions.Add(cc->methods.Apply(j))) {

						err.errorMsg = "Duplicate method definition";
						err.errorSource = pNode->position;
						err.errorType = 1;
						return err;

					}

				}

			}

		} else {

			return err;

		}

	} else if (rootType == AA_AST_NODE_TYPE::fundecl) {

		// Get function signature
		AAFuncSignature* func;
		
		// Register the function (and make sure no compiler error occured)
		if (COMPILE_OK(err = this->RegisterFunction(pNode, func, domain, senv))) {

			// Add function to domain
			if (domain->AddFunction(func)) {

				// If in the global space we'll add it directly to the static checks
				if (domain->IsGlobalSpace()) {

					// Register functions
					senv.availableFunctions.Add(func);

				}

			}

		} else {

			// Return error
			return err;

		}

	} else if (rootType == AA_AST_NODE_TYPE::vardecl) {

		wprintf(L"Not implemented. AAStaticAnalysis.Cpp@%i", __LINE__);

	}

	// No compile errors
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::RegisterClass(AA_AST_NODE* pNode, AAClassSignature*& cc, AACNamespace* domain, AAStaticEnvironment& senv) {

	// Potential compiler error container
	AAC_CompileErrorMessage err;

	int classId;
	if (!domain->classes.FindFirstIndex([pNode](AAClassSignature*& sig) { return sig->name.compare(pNode->content) == 0; }, classId)) {
		printf("fatal error");
	}

	// Fetch the class from domain
	cc = domain->classes.Apply(classId);

	// Compiled Class data size
	cc->classByteSz = 0;

	// Function bodies to correct
	std::vector<AA_AST_NODE*> funcBodyNodes;

	// Make sure we have a class body to work with
	if (pNode->expressions.size() == 1 && pNode->expressions[0]->type == AA_AST_NODE_TYPE::classbody) {

		// For all elements in class body
		for (size_t i = 0; i < pNode->expressions[0]->expressions.size(); i++) {

			// If function
			if (pNode->expressions[0]->expressions[i]->type == AA_AST_NODE_TYPE::fundecl) {

				// We need to push a "this" into the method params list
				m_compilerPointer->GetClassCompilerInstance()->RedefineFunDecl(cc->name, pNode->expressions[0]->expressions[i]);

				// Register function
				AAFuncSignature* sig;
				if (COMPILE_OK(err = this->RegisterFunction(pNode->expressions[0]->expressions[i], sig, domain, senv))) {

					// Update signature data
					sig->isClassMethod = true;
					sig->node = pNode->expressions[0]->expressions[i];
					sig->isClassCtor = sig->name == (cc->name + L"::" + cc->name);

					// Update return count in case of a constructor
					if (sig->isClassCtor) {
						pNode->expressions[0]->expressions[i]->tags["returncount"] = 1;
					}

					// Add method to class definition
					cc->methods.Add(sig);

					if (pNode->expressions[0]->expressions[i]->expressions.size() >= 3) { // make sure it's declared
						funcBodyNodes.push_back(pNode->expressions[0]->expressions[i]->expressions[2]);
					}

				} else {
					return err;
				}

			} else if (pNode->expressions[0]->expressions[i]->type == AA_AST_NODE_TYPE::vardecl) {

				// Create field signature
				AAClassFieldSignature field;
				field.fieldID = (int)cc->fields.Size();
				field.name = pNode->expressions[0]->expressions[i]->content;
				field.type = this->GetTypeFromName(pNode->expressions[0]->expressions[i]->expressions[0]->content, domain, senv);

				// Did we get an invalid field?
				if (field.type == AACType::ErrorType) {
					err.errorMsg = ("Undefined field type '" + string_cast(pNode->expressions[0]->content) + "'").c_str();
					err.errorSource = pNode->expressions[0]->position;
					err.errorType = 0;
					return err;
				}

				// Add field to class fields
				cc->fields.Add(field);

			} else {

				printf("Unknown class member type found");

			}

		}

	}

	// Correct incorrect references (eg. field access)
	m_compilerPointer->GetClassCompilerInstance()->CorrectReferences(cc, funcBodyNodes);

	// Calculate the class size in memory
	cc->classByteSz = m_compilerPointer->GetClassCompilerInstance()->CalculateMemoryUse(cc);

	// Return no errors
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::RegisterFunction(AA_AST_NODE* pNode, AAFuncSignature*& sig, AACNamespace* domain, AAStaticEnvironment& senv) {

	// Potential compiler error container
	AAC_CompileErrorMessage err;

	// Create new function signature
	sig = new AAFuncSignature;

	// Set basic function data
	sig->name = pNode->content;
	sig->returnType = this->GetTypeFromName(pNode->expressions[0]->content, domain, senv);
	sig->node = pNode;

	// Did the function return the error type?
	if (sig->returnType == AACType::ErrorType) {
		err.errorMsg = ("Undefined return type '" + string_cast(pNode->expressions[0]->content) + "'").c_str();
		err.errorSource = pNode->expressions[0]->position;
		err.errorType = 0;
		return err;
	}

	// Setup parameters
	for (AA_AST_NODE* arg : pNode->expressions[1]->expressions) {

		AAFuncParam param;
		param.identifier = arg->content;
		param.type = this->GetTypeFromName(arg->expressions[0]->content, domain, senv);

		// Did the function return the error type?
		if (param.type == AACType::ErrorType) {
			err.errorMsg = ("Undefined argument type '" + string_cast(arg->expressions[0]->content) + "'").c_str();
			err.errorSource = pNode->expressions[0]->position;
			err.errorType = 0;
			return err;
		}

		sig->parameters.push_back(param);

	}

	// Set return count
	pNode->tags["returncount"] = this->GetReturnCount(sig);

	// Get the next procedure ID
	sig->procID = this->m_compilerPointer->GetNextProcID();

	// Verify function control structure
	if (!this->VerifyFunctionControlPath(sig, err)) {
		return err;
	} else {
		return NO_COMPILE_ERROR_MESSAGE;
	}

}

int AAStaticAnalysis::GetReturnCount(AAFuncSignature* funcSig) {
	return (funcSig->returnType == AACType::Void) ? 0 : 1;
}

bool AAStaticAnalysis::VerifyFunctionControlPath(AAFuncSignature* sig, AAC_CompileErrorMessage& err) {

	// If it's a VM function, this will be handled by the C++ compiler
	if (sig->isVMFunc) {
		return true;
	}

	// No node to run test on
	if (sig->node == 0) {
		return false;
	}

	// Get the return count the function expects
	int expectedReturncount = this->GetReturnCount(sig);

	// Get the highest return count detected
	int highestReturncount = this->VerifyFunctionControlPath(sig->node->expressions[2], err);

	// Does the function not return the expected amount?
	if (expectedReturncount != highestReturncount) {

		// Did we compile OK?
		if (COMPILE_OK(err)) {

			// Did we not expect anything?
			if (expectedReturncount == 0) {
				err.errorMsg = "Incorrect return count. Type 'void' should return no value.";
			} else {
				err.errorMsg = "Incorrect return count. Expected at least one return value.";
			}

			// Set error source and type
			err.errorSource = sig->node->position;
			err.errorType = 24;

		}

		return false;

	} else {

		return true;

	}

}

int AAStaticAnalysis::VerifyFunctionControlPath(AA_AST_NODE* pNode, AAC_CompileErrorMessage& err) {

	// Set error message to none
	err = NO_COMPILE_ERROR_MESSAGE;

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::block:
	case AA_AST_NODE_TYPE::funcbody: {
		int j = 0;
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			int k = 0;
			if ((k = VerifyFunctionControlPath(pNode->expressions[i], err)) > j && i == pNode->expressions.size() - 1) {
				j = k;
			}
		}
		return j;
	}
	case AA_AST_NODE_TYPE::binop:
	case AA_AST_NODE_TYPE::unop:
	case AA_AST_NODE_TYPE::intliteral:
	case AA_AST_NODE_TYPE::floatliteral:
	case AA_AST_NODE_TYPE::stringliteral:
	case AA_AST_NODE_TYPE::charliteral:
	case AA_AST_NODE_TYPE::boolliteral:
	case AA_AST_NODE_TYPE::variable:
	case AA_AST_NODE_TYPE::fieldaccess: // Will return one
		return 1;
	case AA_AST_NODE_TYPE::callaccess: // NEEDS PRROPER CHECKING
	case AA_AST_NODE_TYPE::funcall: // NEEDS PROPER CHECKING
		return 1;
	case AA_AST_NODE_TYPE::elsestatement:
		return this->VerifyFunctionControlPath(pNode->expressions[0], err);
	case AA_AST_NODE_TYPE::elseifstatement:
		return this->VerifyFunctionControlPath(pNode->expressions[1], err);
	case AA_AST_NODE_TYPE::ifstatement: {
		printf(""); // This is where we'd potentially determine if a control block leads nowhere
		int ret = pNode->expressions.size() - 2;
		int c = this->VerifyFunctionControlPath(pNode->expressions[1], err);
		for (size_t i = 2; i < pNode->expressions.size(); i++) {
			int j = this->VerifyFunctionControlPath(pNode->expressions[i], err);
			if (c != j) {
				printf("err AAStaticAnalysis.Cpp@%i", __LINE__);
			}
		}
		return c;
	}
	default:
		break;
	}

	return 0;

}

AACType* AAStaticAnalysis::GetTypeFromName(std::wstring tName, AACNamespace* domain, AAStaticEnvironment& senv) {

	// If the type is void, simply return that
	if (tName.compare(L"void") == 0) {
		return AACType::Void;
	}

	// Index of matching type
	int i;

	// We look in the domain first
	if (domain->types.FindFirstIndex([tName](AACType*& type) { return type->name.compare(tName) == 0; }, i)) { // Todo: more checks
		return domain->types.Apply(i);
	}

	// Then we look in the global scope
	if (senv.availableTypes.FindFirstIndex([tName](AACType*& type) { return type->name.compare(tName) == 0; }, i)) { // Todo: more checks
		return senv.availableTypes.Apply(i);
	}

	// Found nothing => Error type
	return AACType::ErrorType;

}
