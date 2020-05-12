#include "AAFileStream.h"


void AAFileStream::OpenWrite(std::wstring path) {

	m_stream = std::wfstream(path, /*std::fstream::in*/ std::fstream::out);

	if (!m_stream.is_open()) {
		throw std::exception("Exception");
	}

}

void AAFileStream::WriteString(std::wstring ws) {

	if (m_stream.is_open()) {

		// Write to stream
		m_stream.write(ws.c_str(), ws.size());

	}

}

void AAFileStream::WriteWchar(wchar_t w) {

	if (m_stream.is_open()) {
		m_stream << w;
	}

}

int AAFileStream::CloseStream() {

	// Close the stream
	m_stream.close();

	return 0;

}
