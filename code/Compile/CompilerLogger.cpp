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

#include "CompilerLogger.h"

#include <stdarg.h>
#include <assert.h>
#include <iostream>

#ifdef _WIN32
#include "CompileFileSystem.h"
#include "Windows.h"
#pragma warning( disable : 4996 4800 )
#endif // _WIN32

void CompilerLogger::LogError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogInternal(format, args);
}

void CompilerLogger::LogWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogInternal(format, args);
}

void CompilerLogger::LogInfo(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogInternal(format, args);
}

void CompilerLogger::LogInternal(const char* format, va_list args)
{
	int result = vsnprintf(m_Buff, LOGSYSTEM_MAX_BUFFER - 1, format, args);
	m_Buff[LOGSYSTEM_MAX_BUFFER - 1] = '\0';

	std::cout << m_Buff;
#ifdef _WIN32
	std::string temp = m_Buff;
	// ワイド文字に変換
	std::wstring tempW = CompileFileSystem::_Win32Utf8ToUtf16(temp);
	OutputDebugStringW(tempW.c_str());
#endif // _WIN32
}