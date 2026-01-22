#pragma once


#include <string>

inline std::wstring ToWString(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
inline std::wstring ToWString(const char* str) {
    if (str == nullptr || str[0] == '\0') {
        return std::wstring();
    }
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, (int)strlen(str), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, (int)strlen(str), &wstrTo[0], size_needed);
    return wstrTo;
}
inline std::string GetFileNameFromFilePath(const std::string& filePath) {
    size_t lastSlashPos = filePath.find_last_of("\\/");
    size_t startPos = (lastSlashPos == std::string::npos) ? 0 : lastSlashPos + 1;
    size_t dotPos = filePath.find_last_of('.');
    size_t endPos = (dotPos == std::string::npos || dotPos < startPos) ? filePath.length() : dotPos;
    return filePath.substr(startPos, endPos - startPos);
}
inline std::string GetFileNameFromFilePath(const char* filePath) {
    std::string pathStr(filePath);
    size_t lastSlashPos = pathStr.find_last_of("\\/");
    size_t startPos = (lastSlashPos == std::string::npos) ? 0 : lastSlashPos + 1;
    size_t dotPos = pathStr.find_last_of('.');
    size_t endPos = (dotPos == std::string::npos || dotPos < startPos) ? pathStr.length() : dotPos;
    return pathStr.substr(startPos, endPos - startPos);
}
