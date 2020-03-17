#include "astring.h"
#include <vector>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#else
#include <Windows.h>
#endif

std::string string_cast(const wchar_t* pSource) {
    if (!pSource) {
        return std::string();
    }
    const size_t sourceLength = std::wcslen(pSource);
    if (sourceLength == 0) {
        return std::string();
    }
    int length = ::WideCharToMultiByte(CP_UTF8, 0, pSource, sourceLength, NULL, 0, NULL, NULL);
    std::vector<char> buffer(length);
    ::WideCharToMultiByte(CP_UTF8, 0, pSource, sourceLength, &buffer[0], length, NULL, NULL);
    return std::string(buffer.begin(), buffer.end());
}

std::string string_cast(std::wstring sSource) {
    const wchar_t* pSource = sSource.c_str();
    const size_t sourceLength = std::wcslen(pSource);
    if (sourceLength == 0) {
        return std::string();
    }
    int length = ::WideCharToMultiByte(CP_UTF8, 0, pSource, sourceLength, NULL, 0, NULL, NULL);
    std::vector<char> buffer(length);
    ::WideCharToMultiByte(CP_UTF8, 0, pSource, sourceLength, &buffer[0], length, NULL, NULL);
    return std::string(buffer.begin(), buffer.end());
}
