#include "AACompileProject.h"
#include "AAUnparser.h"
#include "astring.h"
#include "AAVM.h"
#include "AAB2F.h"

// STL imports
#include <fstream>
#include <sstream>

constexpr int _PROJECT_FILEPATH = 0;
constexpr int _PROJECT_ASSMBLYPATH = 1;
constexpr int _PROJECT_UNPARSEPATH = 2;
constexpr int _PROJECT_RELATIVEPATH = 3;
constexpr int _PROJECT_OUTFILE = 4;
constexpr int _PROJECT_NAME = 5;

AACompileProject::AACompileProject(AAVM* pAAVM) {
	this->m_pVMTarget = pAAVM;
	this->m_importSTL = false;
	this->m_entrypointname = L"";
}

void AACompileProject::SetUnparsePath(std::wstring unparseToPath) {
	this->m_paths[_PROJECT_UNPARSEPATH] = unparseToPath;
}

void AACompileProject::SetGeneratedAssemblishPath(std::wstring assemblishToPath) {
	this->m_paths[_PROJECT_ASSMBLYPATH] = assemblishToPath;
}

void AACompileProject::LoadProjectFile(std::wstring projectFilePath) {
	
	// Set path
	this->m_paths[_PROJECT_FILEPATH] = projectFilePath;
	this->m_paths[_PROJECT_RELATIVEPATH] = projectFilePath.substr(0, projectFilePath.find_last_of(L"\\") + 1);
	this->m_paths[_PROJECT_NAME] = projectFilePath.substr(
		this->m_paths[_PROJECT_RELATIVEPATH].length()
	);
	this->m_paths[_PROJECT_NAME] = this->m_paths[_PROJECT_NAME].substr(0, this->m_paths[_PROJECT_NAME].find_first_of(L"."));

	// Open the project file
	std::wifstream wfs = std::wifstream(this->m_paths[_PROJECT_FILEPATH]);

	// Make sure it's open
	if (!wfs.is_open()) {
		// ERROR
	} else {

		std::wstring ln;
		int state = 1;
		
		// As long as there's a line - read it
		while (std::getline(wfs, ln)) {

			if (aa::aastring::begins_with<std::wstring>(ln, L"compile")) { // path input

				std::wstring targetfile = this->GetPathArgument(ln);
				if (!m_compilefiles.Add(targetfile)) {
					wprintf(L"Duplicate compile file %ws", targetfile.c_str());
				}

			} else if (aa::aastring::begins_with<std::wstring>(ln, L"resource")) { // path input
				
				std::wstring targetfile = this->GetPathArgument(ln);
				if (!m_resourcefiles.Add(targetfile)) {
					wprintf(L"Duplicate resource file %ws", targetfile.c_str());
				}

			} else if (aa::aastring::begins_with<std::wstring>(ln, L"library")) { // path input

				std::wstring targetfile = this->GetPathArgument(ln);
				if (!m_libraryfiles.Add(targetfile)) {
					wprintf(L"Duplicate library file %ws", targetfile.c_str());
				}

			} else if (aa::aastring::begins_with<std::wstring>(ln, L"stl")) { // yes/no input

				if (aa::aastring::contains<std::wstring>(ln, L"yes") && !aa::aastring::contains<std::wstring>(ln, L"no")) {
					m_importSTL = true;
				} else if (!aa::aastring::contains<std::wstring>(ln, L"yes") && aa::aastring::contains<std::wstring>(ln, L"no")) {
					m_importSTL = false;
				} else {
					printf("STL parameter is invalid (yes|no) --> Setting to false");
				}

			} else if (aa::aastring::begins_with<std::wstring>(ln, L"entrypoint")) { // string input

				std::wstring targetfile = this->GetPathArgument(ln);
				if (m_entrypointname.compare(L"") != 0) {
					wprintf(L"Duplicate entrypoint definition '%ws'", targetfile.c_str());
				} else {
					m_entrypointname = targetfile;
				}

			} else if (aa::aastring::begins_with<std::wstring>(ln, L"output")) { // path input

				std::wstring targetfile = this->GetPathArgument(ln);
				if (m_paths[_PROJECT_OUTFILE].compare(L"") != 0) {
					wprintf(L"Duplicate output entry '%ws'", targetfile.c_str());
				} else {
					m_paths[_PROJECT_OUTFILE] = targetfile;
				}

			}

		}

		// Always close files...
		wfs.close();

	}

}

std::wstring AACompileProject::GetPathArgument(std::wstring ln) {

	size_t start, end;
	start = ln.find_first_of(L'\"');
	end = ln.find_last_of(L'\"');

	return ln.substr(start + 1, end - start - 1);

}

bool AACompileProject::ExecuteProjectFile() {

	// Get the compiler instance
	AAC* pCompilerInstance = this->m_pVMTarget->GetCompiler();

	// 1st: Push SDL

	// TODO:

	// 2nd: Push library

	// TODO:

	// 3rd: Compile project:

	auto parser = this->m_pVMTarget->GetParser();
	aa::list<AAP_ParseResult> parseResults;

	// Parse all files
	if (!this->m_compilefiles.ForAll(
		[parser, &parseResults, this](std::wstring& file) {
			AAP_ParseResult r = parser->Parse(std::wifstream(this->m_paths[_PROJECT_RELATIVEPATH] + file));
			parseResults.Add(r);
			return r.success;
		})) {
		return false;
	}

	// Merge into one giant tree
	AAP_ParseResult parseResult;
	parseResults.ForEach(
		[&parseResult](AAP_ParseResult& p) {
			parseResult.result.Add(p.result);
		}
	);

	// The paths for additional output
	std::wstring additionalPaths[2] = {
		this->m_paths[_PROJECT_ASSMBLYPATH] + this->m_paths[_PROJECT_NAME] + L".txt",
		this->m_paths[_PROJECT_UNPARSEPATH] + this->m_paths[_PROJECT_NAME] + L".aa"
	};

	// Set custom entry point (if any)
	pCompilerInstance->SetEntrypointFunction(this->m_entrypointname);

	// Compile the whole project
	auto result = pCompilerInstance->CompileFromAbstractSyntaxTrees(parseResult.result);

	// Did we fail?
	if (!result.success) {
		return false;
	}

	// Unparse if we have a file
	if (additionalPaths[1].compare(L"") != 0) {

		// The unparser
		AAUnparser unparser;
		unparser.Open(additionalPaths[1]);

		// Unparse everything
		parseResults.ForEach(
			[additionalPaths, &unparser](AAP_ParseResult& result) {
				unparser.Unparse(result.result.First());
			}
		);

		// Close the unparser
		unparser.Close();

	}

	// Write bytecode
	aa::dump_bytecode(this->m_paths[_PROJECT_RELATIVEPATH] + this->m_paths[_PROJECT_OUTFILE], result.result);
	
	// Write operations out in a readable format
	if (additionalPaths[0] != L"") {
		aa::dump_instructions(additionalPaths[0], result.compileResults.Vector(), pCompilerInstance->GetTypedata().Vector(), this->m_pVMTarget);
	}

	// Return true
	return true;

}
