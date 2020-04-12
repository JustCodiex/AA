#pragma once

/// <summary>
/// Represents a position in source code
/// </summary>
struct AACodePosition {
	
	/// <summary>
	/// The line where the term can be found
	/// </summary>
	unsigned int line;

	/// <summary>
	/// The column where the term starts
	/// </summary>
	unsigned int column;
	
	AACodePosition() {
		this->line = 0;
		this->column = 0;
	}

	AACodePosition(unsigned int ln, unsigned int column) {
		this->line = ln;
		this->column = column;
	}

	/// <summary>
	/// Is this code position considered valid?
	/// </summary>
	/// <returns>True if line = 2147483647 and column = 2147483647</returns>
	bool isInvalid() { return this->line == 2147483647 && this->column == 2147483647; }

	bool operator==(const AACodePosition other) {
		return this->line == other.line && this->column == other.column;
	}

	/// <summary>
	/// Undetermined position in code (Defined in AAP.Cpp)
	/// </summary>
	static AACodePosition Undetermined;

};
