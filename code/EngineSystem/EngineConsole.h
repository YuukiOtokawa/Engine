#pragma once

#include <string>
#include <vector>

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

class EngineConsole
{
private:
    static std::vector<LogEntry> m_Logs;

public:
    static void Log(const std::string& message, LogLevel level = LogLevel::Info);
    static void Clear();
    static void Draw();
};

