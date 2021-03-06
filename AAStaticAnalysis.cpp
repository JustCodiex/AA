#include "AAStaticAnalysis.h"
#include "astring.h"
#include "AAC.h"
#include "AAVal.h"
#include "AAControlpath.h"
#include "AAPrimitiveType.h"
#include "AAInheritanceTree.h"

using namespace aa::modifiers;

aa::list<AACType*> _getdeftypeenv() {
	aa::list<AACType*> types;
	types.Add(AACTypeDef::Byte);
	types.Add(AACTypeDef::SByte);
	types.Add(AACTypeDef::Bool);
	types.Add(AACTypeDef::Char);
	types.Add(AACTypeDef::Float32);
	types.Add(AACTypeDef::Float64);
	types.Add(AACTypeDef::Int16);
	types.Add(AACTypeDef::Int32);
	types.Add(AACTypeDef::Int64);
	types.Add(AACTypeDef::UInt16);
	types.Add(AACTypeDef::UInt32);
	types.Add(AACTypeDef::UInt64);
	return types;
}

AAStaticAnalysis::AAStaticAnalysis(AAC* pCompiler) {

	// Set pointer to compiler
	this->m_compilerPointer = pCompiler;

	// Point to null until defined
	this->m_objectInheritFrom = NULL;

	// Point to nothing
	this->m_workTrees = NULL;

	// Currently nothing
	this->m_currentTreeIndex = 0;

	// Set type index to 0
	this->m_typeIndex = 0;

	// Create the dynamic type environment
	this->m_dynamicTypeEnvironment = new AADynamicTypeEnvironment;

	// No global scope by default
	this->m_foundGlobalScope = false;

}

void AAStaticAnalysis::Reset(std::vector<AAFuncSignature*> funcs, std::vector<AAClassSignature*> classes, std::vector<AACNamespace*> namespaces) {

	// Copy functions
	this->m_preregisteredFunctions.FromVector(funcs);

	// Copy classes
	this->m_preregisteredClasses.FromVector(classes);

	// Copy namespaces
	this->m_preregisteredNamespaces.FromVector(namespaces);

	// Point to none
	this->m_workTrees = 0;

	// Not working on one
	this->m_currentTreeIndex = 0;

	// Reset the type index
	this->m_typeIndex = 0;

	// Resets the dynamic environment
	this->m_dynamicTypeEnvironment->WipeClean();

	// No global scope yet
	this->m_foundGlobalScope = false;

}

AAC_CompileErrorMessage AAStaticAnalysis::RunStaticAnalysis(std::vector<AA_AST*>& trees) {

	// Compile error container
	AAC_CompileErrorMessage err;

	// Extract the global scope (across all files - must be executed first)
	this->m_foundGlobalScope = this->ExtractGlobalScope(trees);

	// Set to point to the working trees
	this->m_workTrees = &trees;

	// The global domain
	AACNamespace* globalDomain = 0;

	// Get the static environment
	AAStaticEnvironment senv = this->NewStaticEnvironment(globalDomain);

	// Fetch static declerations
	if (COMPILE_ERROR(err = this->FetchStaticDeclerationsFromTrees(globalDomain, senv))) {
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

	// Verifier tool
	AAControlpath pathVerifier;

	// For all input trees, optimize
	for (size_t i = 0; i < trees.size(); i++) {

		// Verifies the control path of the tree
		if (!pathVerifier.VerifyControlpath(trees[i])) {
			printf("Detected error in control path");
		}

	}

	// The tool for vars run
	AAVars varsObj;

	// Run vars check on each tree
	for (size_t i = 0; i < trees.size(); i++) {
		if (!varsObj.Vars(trees[i])) {
			return varsObj.GetError();
		}
	}

	// Update last static environment
	m_lastStaticEnv = senv;

	// Return no errors
	return NO_COMPILE_ERROR_MESSAGE;

}

bool AAStaticAnalysis::RunTypecheckAnalysis(AA_AST* pTree, AAStaticEnvironment& senv, AATypeChecker::Error& typeError) {

	// Run a complete type-check on the tree
	AATypeChecker checker = AATypeChecker(pTree, &senv, this->m_dynamicTypeEnvironment);

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

bool AAStaticAnalysis::FindEntryPoint(std::vector<AA_AST*> trees, AAStaticEnvironment senv, std::wstring overrideEntry, int& foundEntry) {

	// Look for class instance
	std::wstring namespaceDomain = L"";
	std::wstring classInstance = L"";

	// If no override entry is found
	if (overrideEntry.compare(L"") == 0) {
		overrideEntry = L"main";
	} else {
		if (aa::aastring::contains<std::wstring>(overrideEntry, L"::")) {
			// TODO: Look up
		}
	}

	// The function pool to look in
	aa::set<AAFuncSignature*> funcPool; // TODO: Find based on above information

	// TODO: Undo this, so it's done properly
	funcPool = senv.globalNamespace->functions;

	// Find all candidates
	aa::set<AAFuncSignature*> funcCandidates = funcPool.FindAll(
		[overrideEntry](AAFuncSignature*& sig) {
			return sig->GetName().compare(overrideEntry) == 0;
		}
	);

	// If there's only one candidate
	if (funcCandidates.Size() == 1) {

		// Set the proc ID
		foundEntry = funcCandidates.Apply(0)->procID;

		// Return true, we found the function we wanted
		return true;

	} else if (funcCandidates.Size() > 1) {

		// TODO: Implement

		return false;

	} else {

		// Set as global scope
		foundEntry = 0;

		// Return true
		return true;

	}

	// Default here is false
	return false;

}

AAStaticEnvironment AAStaticAnalysis::NewStaticEnvironment(AACNamespace*& globalDomain) {

	// Create the global domain
	globalDomain = new AACNamespace(L"", NULL);

	// Add preregistered namespaces to the global namespace's subspaces
	globalDomain->childspaces.UnionWith(m_preregisteredNamespaces);

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

		AAClassSignature* cc = this->m_preregisteredClasses.Apply(i);

		// Add class and class type to domain
		globalDomain->AddClass(cc);

		// Add type name
		env.availableTypes.Add(cc->type);

		// Add the class
		env.availableClasses.Add(cc);

		// The object
		if (cc->name.compare(L"object") == 0) {

			// Set object
			this->m_objectInheritFrom = cc;

		}

	}

	// For all preloaded functions
	for (size_t i = 0; i < this->m_preregisteredFunctions.Size(); i++) {

		// Add function to domain
		globalDomain->AddFunction(this->m_preregisteredFunctions.Apply(i));

		// Add function
		env.availableFunctions.Add(this->m_preregisteredFunctions.Apply(i));

	}

	// TODO: Add preloaded enums

	// Return static environment
	return env;

}

bool AAStaticAnalysis::ExtractGlobalScope(std::vector<AA_AST*>& trees) {

	// Global scope - which in turn is a compile unit for itself...
	AA_AST_NODE* pGlobalScope = new AA_AST_NODE(L"<GlobalScope>", AA_AST_NODE_TYPE::block, AACodePosition(0, 0));

	// Loop through all trees (files)
	for (size_t i = 0; i < trees.size(); i++) {

		// Expression index
		size_t j = 0;

		// While j < expressions in compile unit of tree i
		while (j < trees[i]->GetRoot()->expressions.size()) {

			// Get node type
			AA_AST_NODE_TYPE nodeType = trees[i]->GetRoot()->expressions[j]->type;

			// If not a declaration or otherwise important - non executiable statement
			if (nodeType != AA_AST_NODE_TYPE::fundecl && nodeType != AA_AST_NODE_TYPE::classdecl && nodeType != AA_AST_NODE_TYPE::enumdecleration && // Handle declerations for themselves
				nodeType != AA_AST_NODE_TYPE::usingspecificstatement && nodeType != AA_AST_NODE_TYPE::usingstatement && // Using statements need to preserve their position in code
				nodeType != AA_AST_NODE_TYPE::name_space) { // namespaces are per definition not part of the global namespace/scope

				pGlobalScope->expressions.push_back(trees[i]->GetRoot()->expressions[j]);
				trees[i]->GetRoot()->expressions.erase(trees[i]->GetRoot()->expressions.begin() + j);

			} else { // Else it is - leave it in
				j++;
			}

		}

	}

	// Add global scope
	trees.insert(trees.begin(), new AA_AST(pGlobalScope));

	// Return true if there were more than one element in the global scope
	return pGlobalScope->expressions.size() > 0;

}

AAC_CompileErrorMessage AAStaticAnalysis::FetchStaticDeclerationsFromTrees(AACNamespace* globalDomain, AAStaticEnvironment& senv) {

	// Error container
	AAC_CompileErrorMessage err;

	// Fetch static declerations from all tree roots, register (but do not verify) their types
	while (m_currentTreeIndex < m_workTrees->size()) {

		// Fetch static decleration from node and put it into the global domain. Return compile error if any
		if (COMPILE_ERROR(err = this->PreregisterTypes(m_workTrees->at(m_currentTreeIndex)->GetRoot(), globalDomain, senv))) {
			return err;
		}

		// Increment tree index
		m_currentTreeIndex++;

	}

	// Reset the tree node index
	m_currentTreeIndex = 0;

	// Fetch static declerations from all tree roots
	while (m_currentTreeIndex < m_workTrees->size()) {

		// Fetch static decleration from node and put it into the global domain. Return compile error if any
		if (COMPILE_ERROR(err = this->FetchStaticDeclerationsFromASTNode(m_workTrees->at(m_currentTreeIndex)->GetRoot(), globalDomain, senv))) {
			return err;
		}

		// Increment tree index
		m_currentTreeIndex++;

	}

	// Apply inheritance
	this->ApplyInheritance(globalDomain, senv);

	// Verify declarations
	if (COMPILE_ERROR(err = this->VerifyDeclarations(globalDomain, senv))) {
		return err;
	}

	// Set the global namespace
	senv.globalNamespace = globalDomain;

	// no compile error messages
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::VerifyDeclarations(AACNamespace* domain, AAStaticEnvironment senv) {

	// Error container
	AAC_CompileErrorMessage err;

	// For all domains
	if (!domain->childspaces.ForAll([&err, this, senv](AACNamespace*& dom) { return COMPILE_OK(err = this->VerifyDeclarations(dom, senv.Union(dom))); })) {
		return err;
	}

	// Make sure we have NO cicular dependencies
	if (!(domain->classes * domain->classes).ForAll([&err, this](aa::set_pair<AAClassSignature*, AAClassSignature*> pair) 
		{ return (COMPILE_OK(err = this->VerifyInheritanceCircularDependency(pair.first, pair.second))); })
		) {

		// Return error
		return err;
	}

	// Verify control paths
	if (!domain->functions.ForAll([&err, this, senv](AAFuncSignature*& fun) { return this->VerifyFunctionControlPath(fun, senv, err); })) {
		return err;
	}

	// no compile error messages
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::PreregisterTypes(AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv) {

	// Error container
	AAC_CompileErrorMessage err;

	// Get the node type
	AA_AST_NODE_TYPE rootType = pNode->type;

	if (rootType == AA_AST_NODE_TYPE::compile_unit) {

		// For all sub-elements of domain
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			// Fetch static decleration from node and put it into the global domain. Return compile error if any
			if (COMPILE_ERROR(err = this->PreregisterTypes(pNode->expressions[i], domain, senv))) {
				return err;
			}
		}

	} else if (rootType == AA_AST_NODE_TYPE::name_space) { // Is it a namespace

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

			// If in global space - add to available classes now
			if (domain->IsGlobalSpace()) {
				senv.availableClasses.Add(classSig);
				senv.availableTypes.Add(classSig->type);
			}

			// Assign constant ID
			classSig->type->constantID = ++m_typeIndex;

		} else { // Already exists in domain -> redefinition NOT allowed

			err.errorMsg = "Duplicate class definition found";
			err.errorSource = pNode->position;
			err.errorType = 1;
			return err;

		}

	} else if (rootType == AA_AST_NODE_TYPE::vardecl) {

		wprintf(L"Not implemented. AAStaticAnalysis.Cpp@%i", __LINE__);

	} else if (rootType == AA_AST_NODE_TYPE::enumdecleration) {

		// Create the new enum signature
		AACEnumSignature* enumSig = new AACEnumSignature(pNode->content);

		// Can we add the enum signature
		if (domain->AddEnum(enumSig)) {

			// If in global space - add to available classes now
			if (domain->IsGlobalSpace()) {
				senv.availableEnums.Add(enumSig);
				senv.availableTypes.Add(enumSig->type);
			}

			// Assign constant ID
			enumSig->type->constantID = ++m_typeIndex;

		} else {

			err.errorMsg = "Duplicate enum definition found";
			err.errorSource = pNode->position;
			err.errorType = 1;
			return err;

		}

	}

	// No compile errors
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::FetchStaticDeclerationsFromASTNode(AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv) {

	// Error container
	AAC_CompileErrorMessage err;

	// Get the node type
	AA_AST_NODE_TYPE rootType = pNode->type;

	// Is it a compile unit (top level)
	if (rootType == AA_AST_NODE_TYPE::compile_unit) {

		// For all sub-elements of domain
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			// Fetch static decleration from node and put it into the global domain. Return compile error if any
			if (COMPILE_ERROR(err = this->FetchStaticDeclerationsFromASTNode(pNode->expressions[i], domain, senv))) {
				return err;
			}
		}

	} else if (rootType == AA_AST_NODE_TYPE::name_space) { // Is it a namespace

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

	} else if (rootType == AA_AST_NODE_TYPE::enumdecleration) {

		// Get the enum
		AACEnumSignature* sig;

		if (COMPILE_OK(err = this->RegisterEnum(pNode, sig, domain, senv))) {

			// Add all methods to domain
			if (!sig->functions.ForAll([domain](AAFuncSignature*& sig) { return domain->AddFunction(sig); })) {
				err.errorMsg = "Duplicate method definition found";
				err.errorSource = pNode->position;
				err.errorType = 1;
				return err;
			}

			// If in the global space we'll add it directly to the static checks
			if (domain->IsGlobalSpace()) {

				// Register class methods into static environment as well
				for (size_t j = 0; j < sig->functions.Size(); j++) {

					// Register method
					if (!senv.availableFunctions.Add(sig->functions.Apply(j))) {

						err.errorMsg = "Duplicate method definition";
						err.errorSource = pNode->position;
						err.errorType = 1;
						return err;

					}

				}

			}

		} else {

			// Return error
			return err;

		}

	}

	// No compile errors
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::RegisterEnum(AA_AST_NODE* pNode, AACEnumSignature*& enumSig, AACNamespace* domain, AAStaticEnvironment& senv) {

	// Potential compiler error container
	AAC_CompileErrorMessage err;

	int enumID;
	if (!domain->enums.FindFirstIndex([pNode](AACEnumSignature*& sig) { return sig->name.compare(pNode->content) == 0; }, enumID)) {
		printf("fatal error");
	}

	// Fetch the enum from domain
	enumSig = domain->enums.Apply(enumID);

	// TODO: Set enum val type here

	// For all enum values
	for (size_t i = 0; i < pNode->expressions[0]->expressions.size(); i++) {

		// TODO: Evaluate constants here DIRECTLY (Everything must be known at compile time)

		// Create a default variable
		enumSig->values.Add(AACEnumValue(pNode->expressions[0]->expressions[i]->content, i));

	}

	// Inherit from object
	this->HandleObjectInheritance(enumSig, senv);

	// Keep track of function bodies
	std::vector<AA_AST_NODE*> funcBodyNodes;

	for (size_t i = 0; i < pNode->expressions[1]->expressions.size(); i++) {

		// If function
		if (pNode->expressions[1]->expressions[i]->type == AA_AST_NODE_TYPE::fundecl) {

			// We need to push a "this" into the method params list
			m_compilerPointer->GetClassCompilerInstance()->RedefineFunDecl(enumSig->name, pNode->expressions[1]->expressions[i]);

			// Register function
			AAFuncSignature* sig;
			if (COMPILE_OK(err = this->RegisterFunction(pNode->expressions[1]->expressions[i], sig, domain, senv))) {

				// Update signature data
				sig->isClassMethod = true;
				sig->node = pNode->expressions[1]->expressions[i];

				// Add method to class definition
				enumSig->functions.Add(sig);

				// make sure it's declared
				if (pNode->expressions[1]->expressions[i]->expressions.size() >= 3) { 
					funcBodyNodes.push_back(pNode->expressions[1]->expressions[i]->expressions[2]);
				}

			} else {
				return err;
			}

		}

	}

	// No error, return AOK
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::RegisterClass(AA_AST_NODE* pNode, AAClassSignature*& cc, AACNamespace* domain, AAStaticEnvironment& senv) {

	// Potential compiler error container
	AAC_CompileErrorMessage err;

	// Flag telling if the current class is abstract
	bool isAbstract = false;

	int classId;
	if (!domain->classes.FindFirstIndex([pNode](AAClassSignature*& sig) { return sig->name.compare(pNode->content) == 0; }, classId)) {
		printf("fatal error");
	}

	// Fetch the class from domain
	cc = domain->classes.Apply(classId);

	// Set class source node
	cc->pSourceNode = pNode;

	// Set default compiled class size
	cc->classByteSz = 0;

	// Do we have any modifiers?
	if (pNode->expressions[AA_NODE_CLASSNODE_MODIFIER]->expressions.size() > 0) {

		// Default storage modifier
		AAStorageModifier sMod = AAStorageModifier::NONE;

		// Go through all the storrage modifiers
		for (size_t i = 0; i < pNode->expressions[AA_NODE_CLASSNODE_MODIFIER]->expressions.size(); i++) {

			// Modifier
			AAStorageModifier mod;

			// Find the modifier (only check for class modifiers)
			if (pNode->expressions[AA_NODE_CLASSNODE_MODIFIER]->expressions[i]->content.compare(L"sealed") == 0) {
				mod = AAStorageModifier::SEALED;
			} else if (pNode->expressions[AA_NODE_CLASSNODE_MODIFIER]->expressions[i]->content.compare(L"abstract") == 0) {
				mod = AAStorageModifier::ABSTRACT;
				isAbstract = true;
			} else if (pNode->expressions[AA_NODE_CLASSNODE_MODIFIER]->expressions[i]->content.compare(L"static") == 0) {
				mod = AAStorageModifier::STATIC;
			} else if (pNode->expressions[AA_NODE_CLASSNODE_MODIFIER]->expressions[i]->content.compare(L"tagged") == 0) {
				mod = AAStorageModifier::TAGGED;
			} else {
				err.errorMsg = ("Invalid modifier '" + string_cast(pNode->expressions[AA_NODE_CLASSNODE_MODIFIER]->expressions[i]->content) + "' on class type").c_str();
				err.errorSource = pNode->expressions[AA_NODE_CLASSNODE_MODIFIER]->expressions[i]->position;
				err.errorType = 0;
				return err;
			}

			// If it's not none (we have to merge)
			if (sMod != AAStorageModifier::NONE) {

				// Merge them
				sMod = sMod | mod;

			} else {

				// Simply assign
				sMod = mod;
			
			}

		}

		// Set storage modifier
		cc->storageModifier = sMod;

	}

	// Do we inherit from something?
	if (pNode->expressions[AA_NODE_CLASSNODE_INHERITANCE]->expressions.size() > 0) {

		// For all inheritances
		for (size_t i = 0; i < pNode->expressions[AA_NODE_CLASSNODE_INHERITANCE]->expressions.size(); i++) {

			// Type to look for
			std::wstring lookfr = pNode->expressions[AA_NODE_CLASSNODE_INHERITANCE]->expressions[i]->content;

			// Type index
			int typeID;
			if (senv.availableTypes.FindFirstIndex([lookfr](AACType*& type) { return type->name.compare(lookfr) == 0 && type->isRefType; }, typeID)) {

				// Get type pointer
				AACType* type = senv.availableTypes.Apply(typeID);

				// Get Make sure type is a valid class!
				if (type->classSignature) {

					// Add to extension list
					cc->extends.Add(type->classSignature);
				
				} else { // invalid extension type

					// Tried to inherit from enum?
					if (type->isEnum) {
						err.errorMsg = ("Inheritance from enum type '" + string_cast(lookfr) + "' is not allowed").c_str();
						err.errorSource = pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]->position;
						err.errorType = 0;
						return err;
					} else {
						err.errorMsg = ("Inheritance from primitive type '" + string_cast(lookfr) + "' is not allowed.").c_str();
						err.errorSource = pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]->position;
						err.errorType = 0;
						return err;
					}

				}

			} else { // type not found

				err.errorMsg = ("Undefined base type '" + string_cast(lookfr) + "'").c_str();
				err.errorSource = pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]->position;
				err.errorType = 0;
				return err;

			}

		}

	} else { // Raw class --> Inherit from object automatically

		// Add extending from object
		cc->extends.Add(m_objectInheritFrom);

		// Assign base ptr
		cc->basePtr = cc->extends.FindIndexOf(m_objectInheritFrom);

	}

	// If it's a tagged class
	if (aa::modifiers::ContainsFlag(cc->storageModifier, AAStorageModifier::TAGGED)) {
		if (COMPILE_ERROR(err = GenerateTaggedClassMethods(cc, pNode))) {
			return err;
		}
	}

	// Function bodies to correct
	std::vector<AA_AST_NODE*> funcBodyNodes;

	// Make sure we have a class body to work with
	if (aa::parsing::Class_HasBody(pNode)) {

		// Track offset
		size_t fieldOffset = 0;

		// For all elements in class body
		for (size_t i = 0; i < pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions.size(); i++) {

			// If function
			if (pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]->type == AA_AST_NODE_TYPE::fundecl) {

				// We need to push a "this" into the method params list
				m_compilerPointer->GetClassCompilerInstance()->RedefineFunDecl(cc->name, pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]);

				// If we found a compile error
				if (COMPILE_ERROR(err = this->AddClassMethod(cc, pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i], domain, senv))) {
					return err;
				}

			} else if (pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]->type == AA_AST_NODE_TYPE::vardecl) {

				// Create field signature
				AAClassFieldSignature field;
				field.fieldID = (int)cc->fields.Size();
				field.fieldOffset = fieldOffset;
				field.fieldOwnerClass = cc;
				field.name = pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]->content;
				field.type = this->GetTypeFromName(pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]->expressions[0]->content, domain, senv);

				// Has field been tagged as a "tagged" field
				if (pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]->HasTag("ctor_field")) {
					field.tagged = true;
				}

				// Did we get an invalid field?
				if (field.type == AACType::ErrorType) {
					err.errorMsg = ("Undefined field type '" + string_cast(pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]->content) + "'").c_str();
					err.errorSource = pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions[i]->position;
					err.errorType = 0;
					return err;
				}

				// Add field to class fields
				cc->fields.Add(field);

				// Increment field offset
				fieldOffset += aa::runtime::size_of_type(aa::runtime::runtimetype_from_statictype(field.type));

			} else {

				printf("Unknown class member type found");

			}

		}

	}

	// Fetch the set of all constructors
	auto ctorSet = cc->GetConstructors();

	// Auto-generate constructor if none is found and not an abstract class
	if (!isAbstract && ctorSet.Size() == 0) {
		if (COMPILE_ERROR(err = this->GenerateDefaultConstructor(cc, pNode, domain, senv))) {
			return err;
		}
	} else if (isAbstract && ctorSet.Size() > 0) {
		err.errorMsg = ("Illegal constructor(s) found in abstract class '" + string_cast(cc->name) + "'").c_str();
		err.errorSource = pNode->position;
		err.errorType = 0;
		return err;
	}

	// Calculate the class size in memory
	cc->classByteSz = this->m_compilerPointer->GetClassCompilerInstance()->CalculateMemoryUse(cc);

	// Return no errors
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::AddClassMethod(AAClassSignature* pClassSig, AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv) {

	AAC_CompileErrorMessage err;

	// Register function
	AAFuncSignature* sig;
	if (COMPILE_OK(err = this->RegisterFunction(pNode, sig, domain, senv))) {

		// Update signature data
		sig->isClassMethod = true;
		sig->node = pNode;
		sig->isClassCtor = sig->name == (pClassSig->name + L"::" + pClassSig->name);

		// Update return count in case of a constructor
		if (sig->isClassCtor) {
			pNode->tags["returncount"] = 1;
		}

		// Add method to class definition
		pClassSig->methods.Add(sig);

		return NO_COMPILE_ERROR_MESSAGE;

	} else {
		return err;
	}

}

AAC_CompileErrorMessage AAStaticAnalysis::GenerateDefaultConstructor(AAClassSignature* pClassSig, AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv) {

	AAC_CompileErrorMessage err;
	AA_AST_NODE* ctorNode = 0;

	if (!this->m_compilerPointer->GetClassCompilerInstance()->AutoConstructor(pClassSig, ctorNode)) {
		printf("");
	}

	if (!ctorNode) {
		printf("");
	}

	if (COMPILE_ERROR(err = this->AddClassMethod(pClassSig, ctorNode, domain, senv))) {
		return err;
	}

	pNode->expressions[AA_NODE_CLASSNODE_BODY]->expressions.push_back(ctorNode);

	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::GenerateTaggedClassMethods(AAClassSignature* pClassSig, AA_AST_NODE* pNode) {

	AAC_CompileErrorMessage err;

	// The tagged auto function constructor
	AA_AST_NODE* pTaggedAutoCtor = 0;

	// Auto-generate some class methods
	if (!m_compilerPointer->GetClassCompilerInstance()->AutoTaggedClass(pClassSig, pTaggedAutoCtor)) {
		err.errorMsg = "Failed to generate tagged class elements";
		err.errorSource = pNode->position;
		err.errorType = aa::compiler_err::C_Compiler_TaggedClass_Generator_Error;
		return err;
	}

	// If assigned, register it
	if (pTaggedAutoCtor) {

		// Insert into next tree (and then let it be handled from there)
		m_workTrees->insert(m_workTrees->begin() + m_currentTreeIndex + 1, new AA_AST(pTaggedAutoCtor));

	} else {

		// Error out => Compiler should always be able to generate this
		err.errorMsg = "Failed to generate tagged class elements";
		err.errorSource = pNode->position;
		err.errorType = aa::compiler_err::C_Compiler_TaggedClass_Generator_Error;
		return err;

	}

	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::RegisterFunction(AA_AST_NODE* pNode, AAFuncSignature*& sig, AACNamespace* domain, AAStaticEnvironment& senv) {

	// Potential compiler error container
	AAC_CompileErrorMessage err;

	// Create new function signature
	sig = new AAFuncSignature;

	// Set basic function data
	sig->name = pNode->content;
	sig->returnType = this->GetTypeFromName(this->TypeIdentifierToString(pNode->expressions[AA_NODE_FUNNODE_RETURNTYPE], domain, senv), domain, senv);
	sig->node = pNode;
	sig->isCompilerGenerated = pNode->HasTag("__ctorgen");

	// Did the function return the error type?
	if (sig->returnType == AACType::ErrorType) {
		err.errorMsg = ("Undefined return type '" + string_cast(pNode->expressions[AA_NODE_FUNNODE_RETURNTYPE]->content) + "'").c_str();
		err.errorSource = pNode->expressions[AA_NODE_FUNNODE_RETURNTYPE]->position;
		err.errorType = 0;
		return err;
	}

	// Setup parameters
	for (AA_AST_NODE* arg : pNode->expressions[AA_NODE_FUNNODE_ARGLIST]->expressions) {

		AAFuncParam param;
		param.identifier = arg->content;
		param.type = this->GetTypeFromName(this->TypeIdentifierToString(arg->expressions[0], domain, senv), domain, senv);

		// Did the function return the error type?
		if (param.type == AACType::ErrorType) {
			err.errorMsg = ("Undefined argument type '" + string_cast(arg->expressions[0]->content) + "'").c_str();
			err.errorSource = pNode->expressions[AA_NODE_FUNNODE_ARGLIST]->position;
			err.errorType = 0;
			return err;
		}

		sig->parameters.push_back(param);

	}

	// Set storage mode
	sig->storageModifier = AAStorageModifier::NONE;

	// For all modifiers
	for (AA_AST_NODE* mod : pNode->expressions[AA_NODE_FUNNODE_MODIFIER]->expressions) {
		if (mod->content.compare(L"virtual") == 0) {
			if (sig->storageModifier == AAStorageModifier::NONE) {
				sig->storageModifier = AAStorageModifier::VIRTUAL;
			} else {
				err.errorMsg = ("Invalid modifier combination 'virtual' and '" + string_cast(aa::NameofStorageModifier(sig->storageModifier)) + "'").c_str();
				err.errorSource = mod->position;
				err.errorType = 0;
				return err;
			}
		} else if (mod->content.compare(L"override") == 0) {
			if (sig->storageModifier == AAStorageModifier::NONE) {
				sig->storageModifier = AAStorageModifier::OVERRIDE;
			} else {
				err.errorMsg = ("Invalid modifier combination 'override' and '" + string_cast(aa::NameofStorageModifier(sig->storageModifier)) + "'").c_str();
				err.errorSource = mod->position;
				err.errorType = 0;
				return err;
			}
		} else if (mod->content.compare(L"abstract") == 0) {
			if (sig->storageModifier == AAStorageModifier::NONE) {
				sig->storageModifier = AAStorageModifier::VIRTUAL | AAStorageModifier::ABSTRACT; // Also adding virtual here
			} else {
				err.errorMsg = ("Invalid modifier combination 'abstract' and '" + string_cast(aa::NameofStorageModifier(sig->storageModifier)) + "'").c_str();
				err.errorSource = mod->position;
				err.errorType = 0;
				return err;
			}
		} else {
			err.errorMsg = ("Invalid method modifier '" + string_cast(mod->content) + "'").c_str();
			err.errorSource = mod->position;
			err.errorType = 0;
			return err;
		}
	}

	// Set return count
	pNode->tags["returncount"] = this->GetReturnCount(sig);

	// Make sure it's not marked abstract (abstract ==> no body ==> nothing to execute)
	//if (!aa::modifiers::ContainsFlag(sig->storageModifier, AAStorageModifier::ABSTRACT)) {

		// Get the next procedure ID
		sig->procID = this->m_compilerPointer->GetNextProcID();

/*	} else {

		// Tell the compiler to ignore it
		pNode->tags["do_not_compile_as_procedure"] = 1;

	}*/

	// Verify function control structure
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::HandleObjectInheritance(AACEnumSignature* sig, AAStaticEnvironment& senv) {
	return this->HandleInheritanceFrom(sig, m_objectInheritFrom, senv);
}

AAC_CompileErrorMessage AAStaticAnalysis::HandleInheritanceFrom(AACEnumSignature* child, AAClassSignature* super, AAStaticEnvironment& senv) {

	// Merge functions
	child->functions.Merge(super->methods,
		[child](AAFuncSignature* sig) { return !child->functions.Contains(sig); },
		[child, &senv](AAFuncSignature* sig) {
			AAFuncSignature* nSig = new AAFuncSignature(*sig);
			nSig->name = child->name + L"::" + nSig->GetName();
			child->functions.Add(nSig);
			return nSig;
		}
	);

	return NO_COMPILE_ERROR_MESSAGE;

}

int AAStaticAnalysis::GetReturnCount(AAFuncSignature* funcSig) {
	return (funcSig->returnType == AACType::Void) ? 0 : 1;
}

bool AAStaticAnalysis::VerifyFunctionControlPath(AAFuncSignature* sig, AAStaticEnvironment environment, AAC_CompileErrorMessage& err) {

	// If it's a VM function, this will be handled by the C++ compiler
	if (sig->isVMFunc) {
		return true;
	}

	// Compiler generated this function (The assumption is then that it's correct)
	if (sig->isCompilerGenerated) {
		return true;
	}

	// No node to run test on
	if (sig->node == 0) {
		return false;
	}

	// If the function is marked abstract, we dont check it
	if (aa::modifiers::ContainsFlag(sig->storageModifier, AAStorageModifier::ABSTRACT)) {
		return true;
	}

	// Get the return count the function expects
	int expectedReturncount = this->GetReturnCount(sig);

	// Get the highest return count detected
	int highestReturncount = this->VerifyFunctionControlPath(sig->node->expressions[AA_NODE_FUNNODE_BODY], environment, err);

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

int AAStaticAnalysis::VerifyFunctionControlPath(AA_AST_NODE* pNode, AAStaticEnvironment environment, AAC_CompileErrorMessage& err) {

	// Set error message to none
	err = NO_COMPILE_ERROR_MESSAGE;

	switch (pNode->type) {
	case AA_AST_NODE_TYPE::block:
	case AA_AST_NODE_TYPE::funcbody: {
		int j = 0;
		for (size_t i = 0; i < pNode->expressions.size(); i++) {
			int k = 0;
			if ((k = VerifyFunctionControlPath(pNode->expressions[i], environment, err)) > j && i == pNode->expressions.size() - 1) {
				j = k;
			}
		}
		return j;
	}
	case AA_AST_NODE_TYPE::binop:
	case AA_AST_NODE_TYPE::unop_pre:
	case AA_AST_NODE_TYPE::unop_post:
	case AA_AST_NODE_TYPE::intliteral:
	case AA_AST_NODE_TYPE::floatliteral:
	case AA_AST_NODE_TYPE::stringliteral:
	case AA_AST_NODE_TYPE::charliteral:
	case AA_AST_NODE_TYPE::boolliteral:
	case AA_AST_NODE_TYPE::variable:
	case AA_AST_NODE_TYPE::fieldaccess: // Will return one
		return 1;
	case AA_AST_NODE_TYPE::callaccess: // NEEDS PRROPER CHECKING
		return 1;
	case AA_AST_NODE_TYPE::funcall: {
		int funcID;
		if (environment.availableFunctions.FindFirstIndex([pNode](AAFuncSignature*& fsig) { return fsig->name.compare(pNode->content) == 0; }, funcID)) {
			return this->GetReturnCount(environment.availableFunctions.Apply(funcID));
		}
		return -1; // TODO: Execute this whole check AFTER typecheck ==> Then we know precisely what function is called
	}
	case AA_AST_NODE_TYPE::elsestatement:
		return this->VerifyFunctionControlPath(pNode->expressions[0], environment, err);
	case AA_AST_NODE_TYPE::elseifstatement:
		return this->VerifyFunctionControlPath(pNode->expressions[1], environment, err);
	case AA_AST_NODE_TYPE::ifstatement: {
		printf(""); // This is where we'd potentially determine if a control block leads nowhere
		int ret = pNode->expressions.size() - 2;
		int c = this->VerifyFunctionControlPath(pNode->expressions[1], environment, err);
		for (size_t i = 2; i < pNode->expressions.size(); i++) {
			int j = this->VerifyFunctionControlPath(pNode->expressions[i], environment, err);
			if (c != j) {
				printf("err AAStaticAnalysis.Cpp@%i", __LINE__);
			}
		}
		return c;
	}
	case AA_AST_NODE_TYPE::matchstatement: // NEEDS PROPER CHECKING
		return 1;
	default:
		break;
	}

	return 0;

}

std::wstring AAStaticAnalysis::TypeIdentifierToString(AA_AST_NODE* pNode, AACNamespace* domain, AAStaticEnvironment& senv) {

	if (pNode->type == AA_AST_NODE_TYPE::typeidentifier) {
		return pNode->content;
	} else if (pNode->type == AA_AST_NODE_TYPE::tupletypeidentifier) {
		auto typeMapper = [this, domain, &senv](std::wstring name) { return this->GetTypeFromName(name, domain, senv); };
		std::wstring formalTupleTypeName = aa::type::FormalizeTuple(pNode, typeMapper);
		this->m_dynamicTypeEnvironment->AddTypeIfNotFound(formalTupleTypeName, typeMapper);
		return formalTupleTypeName;
	}

	return L"";

}

AACType* AAStaticAnalysis::GetTypeFromName(std::wstring tName, AACNamespace* domain, AAStaticEnvironment& senv) {

	// If the typename is not given, we definately can't find it
	if (tName.compare(L"") == 0) {
		return AACType::ErrorType;
	}

	// If the type is void, simply return that
	if (tName.compare(L"void") == 0) {
		return AACType::Void;
	}

	// Lookupp in dynamic type environment
	AACType* dynamicType = this->m_dynamicTypeEnvironment->FindType(tName, [this, domain, &senv](std::wstring ws) { return this->GetTypeFromName(ws, domain, senv); });
	if (dynamicType != AACType::ErrorType) {
		return dynamicType;
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

AAC_CompileErrorMessage AAStaticAnalysis::VerifyInheritanceCircularDependency(AAClassSignature* signatureA, AAClassSignature* signatureB) {

	// Inheritance flags 
	bool a = false, b = false;

	// Does signature A derive from signature B ?
	if (signatureA->DerivesFrom(signatureB)) {
		a = true;
	}

	// Does signature B derive from signature A ?
	if (signatureB->DerivesFrom(signatureA)) {
		b = true;
	}

	// Do they depend on each other?
	if (a && b) {

		// Error message
		AAC_CompileErrorMessage err;
		err.errorMsg = ("Cyclic dependency detected in types '" + string_cast(signatureA->GetFullname()) + "' and '" + string_cast(signatureB->GetFullname()) + "'").c_str();
		err.errorSource = AACodePosition::Undetermined;
		err.errorType = 0;

		// Return error
		return err;

	}

	// Return no compile error
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::ApplyInheritance(AACNamespace* domain, AAStaticEnvironment& senv) {

	// Possible error message
	AAC_CompileErrorMessage err;

	// Call recursively through subspaces
	if (!domain->childspaces.ForAll([&err, &senv, this](AACNamespace* dom) { return COMPILE_OK(err = this->ApplyInheritance(dom, senv)); })) {
		return err;
	}

	// Call on all classes
	if (!domain->classes.ForAll([&err, &senv, this](AAClassSignature*& sig) { return COMPILE_OK(err = this->ApplyInheritance(sig, senv)); })) {
		return err;
	}

	// Return no compile error
	return NO_COMPILE_ERROR_MESSAGE;

}

AAC_CompileErrorMessage AAStaticAnalysis::ApplyInheritance(AAClassSignature* classSig, AAStaticEnvironment& senv) {

	// Inheritance tree
	AAInheritanceTree tree = AAInheritanceTree(classSig);

	// Set auxilary
	tree.SetAuxilary(this->m_compilerPointer->GetClassCompilerInstance(), &senv);

	// Sort the tree
	tree.Sort();

	// Apply the tree
	tree.Apply();

	// Possible error message
	AAC_CompileErrorMessage err;

	// Function that's the source of a potential error
	AAFuncSignature* errSrc;

	// Verify we have no dangling override functions
	if (!classSig->methods.ForAll([&errSrc](AAFuncSignature*& sig) { errSrc = sig; return sig->storageModifier != AAStorageModifier::OVERRIDE || sig->overrides != 0; })) {
		err.errorMsg = ("No matching function '" + string_cast(errSrc->GetName()) + "' to override found in inherited classes").c_str();
		err.errorSource = errSrc->node->position;
		err.errorType = 0;
		return err;
	}

	// Return no compile error
	return NO_COMPILE_ERROR_MESSAGE;

}
