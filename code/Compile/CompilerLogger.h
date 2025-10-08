#pragma once

//
// Copyright (c) 2010-2011 Matthew Jack and Doug Binks
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include <string>
#include <stdio.h>

const size_t LOGSYSTEM_MAX_BUFFER = 4096;

class CompilerLogger
{
protected:
    /// @brief 指定されたフォーマット文字列と可変引数リストを使用してログを出力します。
    /// @param format 書式指定されたログメッセージのフォーマット文字列。
    /// @param args ログメッセージの値を含む可変引数リスト。
    void LogInternal(const char* format, va_list args);
    char m_Buff[LOGSYSTEM_MAX_BUFFER];

public:
    virtual void LogError(const char* format, ...);
    virtual void LogWarning(const char* format, ...);
    virtual void LogInfo(const char* format, ...);
};

