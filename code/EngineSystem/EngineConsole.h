#pragma once

#include <string>
#include <vector>

#include "EngineAPI.h"

enum class LogLevel {
    Info,
    Warning,
    Error
};

struct LogEntry {
    std::string message;
    std::string timestamp;
    LogLevel level;
};

const size_t CONSOLE_LOG_BUFFER_MAX = 4096;

class OTOKAWA_API EngineConsole
{
private:
    static std::vector<LogEntry> m_Logs;

    static char m_Buff[CONSOLE_LOG_BUFFER_MAX];

    static void LogInternal(const char* message, va_list args, LogLevel level);
public:
    static void Log(const char* message, ...);
    static void LogWarning(const char* message, ...);
    static void LogError(const char* message, ...);
    static void Clear();
    static void Draw();
};

