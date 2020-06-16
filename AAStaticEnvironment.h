#pragma once
#include "AAClassCompiler.h"

/// <summary>
/// A static environment for static data
/// </summary>
struct AAStaticEnvironment {

	AACNamespace* globalNamespace;
	aa::set<AAFuncSignature*> availableFunctions;
	aa::set<AAClassSignature*> availableClasses;
	aa::set<AACType*> availableTypes;
	aa::set<AACEnumSignature*> availableEnums;

	AAStaticEnvironment() {
		globalNamespace = 0;
	}

	AAStaticEnvironment Union(AACNamespace*& space) const {
		AAStaticEnvironment senv = AAStaticEnvironment(*this);
		senv.availableClasses.UnionWith(space->classes);
		senv.availableFunctions.UnionWith(space->functions);
		senv.availableTypes.UnionWith(space->types);
		senv.availableEnums.UnionWith(space->enums);
		return senv;
	}

};
