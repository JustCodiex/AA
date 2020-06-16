#pragma once
#include "AAStaticEnvironment.h"
#include "list.h"

class AAClassCompiler;

/// <summary>
/// Inheritance tree 
/// </summary>
class AAInheritanceTree {

private:

	enum class NodeType {
		SELF,
		BASE,
		ABSTRACTBASE,
		INTERFACE,
	};

	struct Node {
		NodeType type;
		AAClassSignature* pClass;
		aa::list<Node> extends;
		int priority;
		Node() {
			type = NodeType::SELF;
			pClass = 0;
			priority = 0;
		}
		void UpdatePriority() {
			int m = extends.Max([](Node n) { return n.priority; });
			if (m == INT32_MIN) {
				m = -1;
			}
			this->priority = m + 1;
		}
	};


	struct FieldInheritance {
		AAClassFieldSignature field;
		int priority;
		FieldInheritance() { priority = 0; }
		bool operator==(FieldInheritance other) {
			return field.name.compare(other.field.name) == 0 && priority == other.priority;
		}
	};

	enum class MethodInheritanceType {
		INHERIT,
		OVERRIDE,
	};

	struct MethodInheritance {
		AAFuncSignature* function;
		AAClassSignature* classOwner;
		MethodInheritanceType type;
		int priority;
		MethodInheritance() {
			function = 0;
			priority = 0;
			classOwner = 0;
			type = MethodInheritanceType::INHERIT;
		}
		bool operator==(MethodInheritance other) {
			return function->GetFunctionalSignature(true).compare(other.function->GetFunctionalSignature(true)) == 0;
		}
	};

public:

	AAInheritanceTree(AAClassSignature* root);
	~AAInheritanceTree();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="pClassCompiler"></param>
	void SetAuxilary(AAClassCompiler* pClassCompiler, AAStaticEnvironment* pSenv);

	/// <summary>
	/// Sort the tree
	/// </summary>
	void Sort();

	/// <summary>
	/// Apply inheritance
	/// </summary>
	void Apply();

private:

	Node Sort(AAClassSignature* pBase);

	void Apply(Node& node);

	void InheritFields(Node node);

	void InheritMethods(Node node);

	void InheritProperties(Node node);

	void UpdateMethods(Node node);

	void HandleVirtualMembers(Node node);

private:

	AAClassSignature* root;
	Node rootNode;

	AAClassCompiler* classCompiler;
	AAStaticEnvironment* senv;

};
