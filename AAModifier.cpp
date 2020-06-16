#include "AAModifier.h"

namespace aa {

	std::wstring NameofAccessModifier(AAAccessModifier mod) {
		switch (mod) {
		case AAAccessModifier::PUBLIC:
			return L"public";
		case AAAccessModifier::PRIVATE:
			return L"private";
		case AAAccessModifier::PROTECTED:
			return L"protected";
		case AAAccessModifier::INTERNAL:
			return L"internal";
		case AAAccessModifier::EXTERNAL:
			return L"external";
		default:
			return NameofAccessModifier(AAAccessModifier::PUBLIC);
		}
	}

	std::wstring NameofStorageModifier(AAStorageModifier mod) {
		switch (mod) {
		case AAStorageModifier::NONE:
			return L"";
		case AAStorageModifier::VIRTUAL:
			return L"virtual";
		case AAStorageModifier::OVERRIDE:
			return L"override";
		case AAStorageModifier::TAGGED:
			return L"tagged";
		case AAStorageModifier::STATIC:
			return L"static";
		case AAStorageModifier::ABSTRACT:
			return L"abstract";
		case AAStorageModifier::SEALED:
			return L"sealed";
		case AAStorageModifier::CONSTANT:
			return L"const";
		default:
			return NameofStorageModifier(AAStorageModifier::NONE);
		}
	}

	namespace modifiers {

		bool IsLegalModifierCombination(AAAccessModifier aMod, AAStorageModifier sMod) {
			return true;
		}

		bool IsLegalStorageModifierCombination(AAStorageModifier mod) {
			return true;
		}

		bool IsLegalAccessModifierCombination(AAAccessModifier mod) {
			return true;
		}

		AAStorageModifier operator|(AAStorageModifier a, AAStorageModifier b) {
			return static_cast<AAStorageModifier>(static_cast<int>(a) | static_cast<int>(b));
		}

		AAStorageModifier operator&(AAStorageModifier a, AAStorageModifier b) {
			return static_cast<AAStorageModifier>(static_cast<int>(a) & static_cast<int>(b));
		}

		bool ContainsFlag(AAStorageModifier modifier, AAStorageModifier flag) {
			return (static_cast<int>(modifier & flag)) != 0;
		}

	}

}
