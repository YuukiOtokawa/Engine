//
// Copyright (c) 2013 Márton Tamás
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

// Minor modifications by Doug Binks
// This implementation for FileWatcherWin32 uses FindFirstChangeNotification rather
// than ReadDirectoryChanges

#pragma once

#include <iostream>
#include <vector>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace FW
{
    const static int FW_TYPES[] = { FILE_NOTIFY_CHANGE_LAST_WRITE,FILE_NOTIFY_CHANGE_CREATION, FILE_NOTIFY_CHANGE_SIZE,FILE_NOTIFY_CHANGE_FILE_NAME };
    const static size_t NUMTYPES = sizeof(FW_TYPES) / sizeof(int);

    class FileWatcher_Alt
    {
        struct FileData
        {
            std::string fileName;
            LARGE_INTEGER fileSize;
            FILETIME fileTime;
            FILETIME fileWriteTime;
            FileData(std::string n = std::string(), LARGE_INTEGER s = LARGE_INTEGER(), FILETIME t = FILETIME(), FILETIME tw = FILETIME())
            {
                fileName = n;
                fileSize = s;
                fileTime = t;
                fileWriteTime = tw;
            }
        };

        std::vector<HANDLE> handles[NUMTYPES];
        std::vector<std::vector<FileData>> dirContents;
    };
}
