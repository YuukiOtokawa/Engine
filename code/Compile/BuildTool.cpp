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

#include "BuildTool.h"
#include "Compiler.h"
#include <fstream>
#include <algorithm>
#include "CompilerLogger.h"

using namespace std;
using namespace CompileFileSystem;

BuildTool::BuildTool() {}
BuildTool::~BuildTool() {}

void BuildTool::Clean(const Path& tempraryPath) const {
	PathIterator pathIt(tempraryPath);
	std::string objExtention = m_Compiler.GetObjectFileExtension();

	while (++pathIt) {
		if (pathIt.GetPath().Extension() == objExtention) {
			if (m_pLogger){
				m_pLogger->LogInfo("Deleting temp RCC+ obj file: %s\n", pathIt.GetPath().c_str());
			}
			pathIt.GetPath().Remove();
		}
	}

}

void BuildTool::Initialize(CompilerLogger* pLogger) {
	m_pLogger = pLogger;
	m_Compiler.Initialize(pLogger);
}

void BuildTool::BuildModule(const std::vector<FileToBuild>& buildFileList,
							const CompilerOptions& compilerOptions,
							const std::vector<CompileFileSystem::Path>& linkLibraryList,
							const CompileFileSystem::Path& moduleName) {
	Path objectFileExtension = m_Compiler.GetObjectFileExtension();
	vector<Path> compileFileList;
	compileFileList.reserve(buildFileList.size());
	vector<Path> forcedCompileFileList;
	vector<Path> nonForcedCompileFileList;

	for (size_t i = 0; i < buildFileList.size(); i++) {
		Path buildFile = buildFileList[i].m_FilePath;
		if (buildFileList[i].m_ForceCompile) {
			if (find(forcedCompileFileList.begin(), forcedCompileFileList.end(), buildFile) == forcedCompileFileList.end()) {
				forcedCompileFileList.push_back(buildFile);
			}
		}
		else {
			if (find(nonForcedCompileFileList.begin(), nonForcedCompileFileList.end(), buildFile) == nonForcedCompileFileList.end()) {
				nonForcedCompileFileList.push_back(buildFile);
			}
		}
	}

	for (size_t i = 0; i < forcedCompileFileList.size(); i++) {
		compileFileList.push_back(forcedCompileFileList[i]);
	}

	for (size_t i = 0; i < nonForcedCompileFileList.size(); i++) {
		Path buildFile = nonForcedCompileFileList[i];
		if (find(forcedCompileFileList.begin(), forcedCompileFileList.end(), buildFile) == forcedCompileFileList.end()) {
			Path objectFileName = compilerOptions.intermediatePath / buildFile.FileName();
			objectFileName.ReplaceExtension(objectFileExtension.c_str());

			if (objectFileName.Exists() && buildFile.Exists()) {
				CompileFileSystem::filetime_t objTime = objectFileName.GetLastWriteTime();
				if (objTime > buildFile.GetLastWriteTime()) {
					buildFile = objectFileName;
				}
			}
			compileFileList.push_back(buildFile);
		}
	}

	std::vector<CompileFileSystem::Path> uniqueLinkLibraryList;
	uniqueLinkLibraryList.reserve(linkLibraryList.size());
	for (const auto& library : linkLibraryList) {
		if (find(uniqueLinkLibraryList.begin(), uniqueLinkLibraryList.end(), library) == uniqueLinkLibraryList.end()) {
			uniqueLinkLibraryList.push_back(library);
		}
	}

	m_Compiler.RunCompile(compileFileList, compilerOptions, uniqueLinkLibraryList, moduleName);

}