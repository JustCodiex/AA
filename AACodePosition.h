#pragma once

struct AACodePosition {
	unsigned int line; // The line where the term can be found
	unsigned int column; // The column where the term starts
	AACodePosition() {
		this->line = 0;
		this->column = 0;
	}
	AACodePosition(unsigned int ln, unsigned int column) {
		this->line = ln;
		this->column = column;
	}
};
