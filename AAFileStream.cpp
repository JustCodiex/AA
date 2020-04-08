#include "AAFileStream.h"


void AAFileStream::OpenWrite(std::wstring path) {

	m_stream = std::wfstream(path, /*std::fstream::in*/ std::fstream::out);

	if (!m_stream.is_open()) {
		throw std::exception("Exception");
	}

}

void AAFileStream::WriteString(std::wstring ws) {

	// Write to stream
	m_stream.write(ws.c_str(), ws.size());

}

int AAFileStream::CloseStream() {

	// Close the stream
	m_stream.close();

	return 0;

}
