#pragma once
#include "set.h"
#include <string>

class AAVM; // Forward declaration of VM

/// <summary>
/// Represents a project that's being compiled
/// </summary>
class AACompileProject {

public:

	AACompileProject(AAVM* pAAVM);

	void LoadProjectFile(std::wstring projectFilePath);

	void SetUnparsePath(std::wstring unparseToPath);
	void SetGeneratedAssemblishPath(std::wstring assemblishToPath);

	bool ExecuteProjectFile();

private:

	std::wstring GetPathArgument(std::wstring ln);

private:

	AAVM* m_pVMTarget;

	std::wstring m_paths[16];
	
	aa::set<std::wstring> m_compilefiles;
	aa::set<std::wstring> m_resourcefiles;
	aa::set<std::wstring> m_libraryfiles;

	std::wstring m_entrypointname;
	bool m_importSTL;

};
