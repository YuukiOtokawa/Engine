// RuntimeCompiledC++を参考にしたコンパイルシステム
// https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus.git

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

//
// Notes:
//   - We use a single intermediate directory for compiled .obj files, which means
//     we don't support compiling multiple files with the same name. Could fix this
//     with either mangling names to include paths,  or recreating folder structure
//
//

#include "Compiler.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include "CompileFileSystem.h"

#include "assert.h"
#include <process.h>

using namespace std;
using namespace CompileFileSystem;

struct VSVersionInfo
{
	std::string Path;
};

const std::string CompiletionToken("_COMPILETION_TOKEN_");

void GetPathsOfVisualStudioInstalls(std::vector<VSVersionInfo>* pVersions, CompilerLogger* pLogger);

void ReadAndHandleOutputThread(LPVOID arg);

struct CmdProcess
{
	CmdProcess();
	~CmdProcess();

	void InitializeProcess();
	void WriteInput(std::string& input);
	void CleanupProcessAndPipes();

	PROCESS_INFORMATION m_CmdProcessInfo;
	HANDLE m_CmdProcessOutputRead;
	HANDLE m_CmdProcessInputWrite;
	
	volatile bool m_isComplete;
	CompilerLogger* m_pLogger;
	bool m_isStoreCmdOutput;
	std::string m_CmdOutput;
	CompileFileSystem::Path m_PathTempCLCommandFile;
};

class PlatformCompilerData
{
public:
	PlatformCompilerData();
	~PlatformCompilerData();

	std::string m_VSPath;
	bool m_isFoundVS;
	CmdProcess m_CmdProcess;
	CompilerLogger* m_pLogger;
};

Compiler::Compiler()
	: m_pData(0)
	, m_isFastCompile(false)
{ }

Compiler::~Compiler()
{
	delete m_pData;
}

std::string Compiler::GetObjectFileExtension() const
{
	return ".obj";
}

bool Compiler::GetIsComplete() const
{
	bool isComplete = m_pData->m_CmdProcess.m_isComplete;
	if (isComplete && !m_isFastCompile)
	{
		m_pData->m_CmdProcess.CleanupProcessAndPipes();
	}
	return isComplete;
}

void Compiler::Initialize(CompilerLogger* pLogger)
{
	m_pData = new PlatformCompilerData;
	m_pData->m_pLogger = pLogger;
	m_pData->m_CmdProcess.m_pLogger = pLogger;
}

static inline std::string ExpandEnvVars(const std::string& string)
{
	std::wstring temp = CompileFileSystem::_Win32Utf8ToUtf16(string);
	uint32_t numChars = ExpandEnvironmentStringsW(temp.c_str(), nullptr, 0);
	std::wstring tempExpanded;
	tempExpanded.resize(numChars++); // documentation is a little unclear if null character is included
	uint32_t numCharsExpanded = ExpandEnvironmentStringsW(temp.c_str(), &tempExpanded[0], numChars);
	return CompileFileSystem::_Win32Utf16ToUtf8(tempExpanded);
}

void Compiler::RunCompile(const std::vector<CompileFileSystem::Path>& filesToCompile,
						  const CompilerOptions& compilerOptions,
						  const std::vector<CompileFileSystem::Path>& linkLibraryList,
						  const CompileFileSystem::Path& moduleName)
{
	if (!m_pData->m_isFoundVS)
	{
		std::vector<VSVersionInfo> vsVersions;
		GetPathsOfVisualStudioInstalls(&vsVersions, m_pData->m_pLogger);
		if (vsVersions.size())
		{
			m_pData->m_VSPath = vsVersions[0].Path; // just use first one for now
			m_pData->m_isFoundVS = true;
			if (m_pData->m_pLogger) m_pData->m_pLogger->LogInfo("[RuntimeCompiler] Found Visual Studio at %s\n", m_pData->m_VSPath.c_str());
		}
		else
		{
			if (m_pData->m_pLogger) m_pData->m_pLogger->LogError("[RuntimeCompiler] Could not find a Visual Studio install\n");
			return;
		}
	}
	std::string compilerLocation = ExpandEnvVars(compilerOptions.compilerLocation.m_String);
	if (compilerLocation.empty())
	{
		if (m_pData->m_pLogger) m_pData->m_pLogger->LogError("[RuntimeCompiler] No compiler location set\n");
		return;
	}
	if (!CompileFileSystem::Path(compilerLocation).Exists())
	{
		if (m_pData->m_pLogger) m_pData->m_pLogger->LogError("[RuntimeCompiler] Compiler location does not exist: %s\n", compilerLocation.c_str());
		return;
	}
	std::string flags;
	switch (compilerOptions.optimizationLevel)
	{
	case O0: flags = " /Od /RTC1"; break;
	case O1: flags = " /O1"; break;
	case O2: flags = " /O2"; break;
	case O3: flags = " /Ox"; break;
	case Os: flags = " /O1"; break; // no size optimization in MSVC
	case Oz: flags = " /O1"; break; // no size optimization in MSVC
	default:
		break;
	}
	std::string pCharTypeFlags = " /Zc:wchar_t"; // always use wchar_t
}