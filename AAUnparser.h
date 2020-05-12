#pragma once
#include "AA_AST.h"
#include <string>
#include <fstream>

class AAUnparser {

public:

	/// <summary>
	/// Open the file to write unparsed data to
	/// </summary>
	/// <param name="targetFile">The file to write unparsed data to</param>
	void Open(std::wstring targetFile);

	/// <summary>
	/// Closes and saves the unparser file
	/// </summary>
	void Close();

	/// <summary>
	/// Append unparsed string for AA_AST
	/// </summary>
	/// <param name="pTree">The tree to unparse</param>
	void Unparse(AA_AST* pTree);

private:

	std::wstring Unparse(AA_AST_NODE* pNode);

	inline std::wstring WriteToString(std::wstring ln, ...);

	inline std::wstring WriteIndent();

	inline void IncreaseIndent();
	inline void DecreaseIndent();

private:

	bool m_isOpen;
	std::wofstream m_stream;
	int m_indent;

};
