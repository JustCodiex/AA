#pragma once
#include "AAVM.h"
#include <string>
#include <iostream>

class AAFileStream {

public:

	void OpenWrite(std::wstring path);

	void WriteString(std::wstring ws);

	int CloseStream();

private:

	std::wfstream m_stream;

};
