#include "AAInheritanceTree.h"
#include "AAPrimitiveType.h"
#include "AAClassCompiler.h"

AAInheritanceTree::AAInheritanceTree(AAClassSignature* root) {
	this->root = root;
	this->classCompiler = 0;
	this->senv = 0;
}

AAInheritanceTree::~AAInheritanceTree() {}

void AAInheritanceTree::SetAuxilary(AAClassCompiler* pClassCompiler, AAStaticEnvironment* pSenv) {
	this->classCompiler = pClassCompiler;
	this->senv = pSenv;
}

void AAInheritanceTree::Sort() {
	this->rootNode = this->Sort(this->root);
}

AAInheritanceTree::Node AAInheritanceTree::Sort(AAClassSignature* pBase) {
	
	// The node we're working on
	Node n = Node();
	n.pClass = pBase;
	
	// Determine type
	if (pBase == this->root) {
		n.type = NodeType::SELF;
	} else {
		if (aa::modifiers::ContainsFlag(pBase->storageModifier, AAStorageModifier::ABSTRACT)) {
			n.type = NodeType::ABSTRACTBASE;
		} /* else if interface ... */ else {
			n.type = NodeType::BASE;
		}
	}

	// For each extension
	pBase->extends.ForEach(
		[&n, this](AAClassSignature*& sig) {
			n.extends.Add(this->Sort(sig));
		}
	);

	return n;

}

void AAInheritanceTree::Apply() {
	this->Apply(this->rootNode);
}

void AAInheritanceTree::Apply(Node& node) {

	// Are we extending from something?
	if (node.extends.Size() > 0) {

		// First we apply to parents --> Such that they're correct before we inherit from them
		node.extends.ForEach(
			[this, &node](Node& n) {

				// Apply on node
				this->Apply(n);

			}
		);

		// Update priority of node
		node.UpdatePriority();

		// Apply field inheritance
		this->InheritFields(node);

		// Apply property inheritance
		this->InheritProperties(node);

		// Apply function inheritance (Will always have to happen)
		this->InheritMethods(node);

		// Update methods with 'this' and 'base' references
		this->UpdateMethods(node);

		// Recalculate the class size in memory
		node.pClass->classByteSz = classCompiler->CalculateMemoryUse(node.pClass);

	}

}

void AAInheritanceTree::InheritFields(Node node) {

	// Get existing fields
	aa::set<FieldInheritance> fields;

	// Add all fields
	node.extends.ForEach(
		[&fields](Node& n) {
			n.pClass->fields.ForEach(
				[&fields, n](AAClassFieldSignature& f) {
					FieldInheritance fi;
					fi.field = f;
					fi.priority = n.priority;
					fields.Add(fi); 
				}
			);
		}
	);

	// If we have fields to inhert
	if (fields.Size() > 0) {

		// Fields to inherit (+ own fields)
		aa::set<AAClassFieldSignature> inheritFields;
		
		// Order by priority
		auto ls = fields.ToList().Filter(
			[](FieldInheritance& f) -> bool {
				return f.field.accessModifier != AAAccessModifier::PRIVATE;
			}
		).SortBy<int>(
			[](const FieldInheritance& f) -> int {
				return f.priority;
			}
		).Map<AAClassFieldSignature>(
			[](FieldInheritance& f) -> AAClassFieldSignature {
				return AAClassFieldSignature(f.field);
			}
		);

		ls.Add(node.pClass->fields.ToList());
		inheritFields.FromList(ls);

		size_t offset = 0;
		int index = 0;

		// Go through all fields and sort out offsets
		inheritFields.ForEach(
			[&offset, &index](AAClassFieldSignature& sig) {
				sig.fieldID = index++;
				sig.fieldOffset = offset;
				offset += aa::runtime::size_of_type(aa::runtime::runtimetype_from_statictype(sig.type));
			}
		);

		// Set inheritted fields
		node.pClass->fields = inheritFields;

	}

}

void AAInheritanceTree::InheritMethods(Node node) {

	// First, handle virtual members
	this->HandleVirtualMembers(node);

	// Secondly, handle our inheritted methods
	aa::set<MethodInheritance> inheritMethods;

	// Go though all our nodes we're an extension of and collect their methods
	node.extends.ForEach(
		[&inheritMethods, node](Node& n) {
			n.pClass->methods.ForEach(
				[&inheritMethods, node, n](AAFuncSignature*& sig) {
					if (!sig->isClassCtor && sig->accessModifier != AAAccessModifier::PRIVATE) {
						std::wstring s = sig->GetFunctionalSignature(true);
						if (node.pClass->FindMethodFromFunctionalSignature(s) == 0) { // No override was found
							MethodInheritance m;
							m.classOwner = n.pClass;
							m.function = sig;
							m.priority = n.priority;
							m.type = MethodInheritanceType::INHERIT;
							inheritMethods.Add(m);
						} else { // An override was found
							if (aa::modifiers::ContainsFlag(sig->storageModifier, AAStorageModifier::VIRTUAL)) { // only add if marked virtual
								MethodInheritance m;
								m.classOwner = n.pClass;
								m.function = sig;
								m.priority = n.priority;
								m.type = MethodInheritanceType::OVERRIDE;
								inheritMethods.Add(m);
							}
						}
					}
				}
			);
		}
	);

	// Do we have any methods to work with?
	if (inheritMethods.Size() > 0) {

		// Methods to inherit
		aa::set<AAFuncSignature*> inherittedMethods;

		// Sort by priority and map inherrited methods to a function signature
		auto ls = inheritMethods.ToList()
			.SortBy<int>(
				[](const MethodInheritance& m) -> int {
					return m.priority;
				}
			).Map<AAFuncSignature*>(
				[node, this](MethodInheritance& m) -> AAFuncSignature* {
					if (m.type == MethodInheritanceType::INHERIT) {
						AAFuncSignature* sig = new AAFuncSignature(*m.function);
						sig->name = node.pClass->name + L"::" + sig->GetName();
						node.pClass->domain->AddFunction(sig); // Add function to domain
						if (node.pClass->domain->IsGlobalSpace()) { // If global, we also have to add it to the auxilary static environment
							this->senv->availableFunctions.Add(sig);
						}
						return sig;
					} else {
						int i = -1;
						if (node.pClass->methods.FindFirstIndex([m](AAFuncSignature*& s) { return s->GetFunctionalSignature(true) == m.function->GetFunctionalSignature(true); }, i)) {
							AAFuncSignature* sig = node.pClass->methods.Apply(i);
							sig->overrides = m.function; // Set override
							if (aa::modifiers::ContainsFlag(m.function->storageModifier, AAStorageModifier::VIRTUAL)) {
								m.classOwner->classVTable->RegisterFunction(node.pClass, sig); // Register in the VTable
							}
							return sig; // We merely update it ==> The set functionality will ensure it's not added as an duplicate
						} else {
							return NULL;
						}
					}
				}
			);

		// Convert to set
		inherittedMethods.FromList(ls);

		// Add new methods
		node.pClass->methods.UnionWith(inherittedMethods);

	}

}

void AAInheritanceTree::UpdateMethods(Node node) {

	// For each constructor in class
	node.pClass->GetConstructors().ForEach(
		[this, node](AAFuncSignature*& ctorSig) {
			if (!ctorSig->isVMFunc) {
				this->classCompiler->AddInheritanceCall(node.pClass, ctorSig->node);
			}
		}
	);

	// Correct incorrect references (eg. field access)
	classCompiler->CorrectReferences(node.pClass->fields.ToList().Map<std::wstring>([](AAClassFieldSignature& f) { return f.name; }), node.pClass->methods);

}

void AAInheritanceTree::HandleVirtualMembers(Node node) {

	// Does the class already have a vtable?
	if (node.pClass->classVTable != 0) {
		return; // This has already been done
	}

	// Find all virtual methods
	aa::set<AAFuncSignature*> virtuals = node.pClass->methods.FindAll(
		[](AAFuncSignature*& s) {
			return aa::modifiers::ContainsFlag(s->storageModifier, AAStorageModifier::VIRTUAL);
		}
	);

	// If any virtual functions found
	if (virtuals.Size() > 0) {

		// Create the virtual table
		node.pClass->CreateVTable();

		// Add virtual
		virtuals.ForEach(
			[node](AAFuncSignature*& v) {
				node.pClass->classVTable->AddVirtualFunction(v);
			}
		);

	}

}

void AAInheritanceTree::InheritProperties(Node node) {



}
