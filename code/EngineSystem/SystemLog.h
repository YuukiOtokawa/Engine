#pragma once

#include <string>
class SystemLog
{
public:
    static void Log(int line, std::string log);
    static void ResetLog();
};

