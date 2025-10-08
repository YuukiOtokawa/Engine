#pragma once

// RuntimeCompiledC++を参考にしたコンパイルシステム
// https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus.git

/**
    @author James Wynn
    @date 4/15/2009

    Copyright (c) 2009 James Wynn (james@jameswynn.com)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <string>
#include <stdexcept>

#include <map>

#include "CompileFileSystem.h"
#include "FileWatcher_Alt.h"


namespace FW
{
    
    class FileWatchListener;

    namespace Actions
    {
        enum Action
        {
            Add = 1,        // created or renamed
            Delete = 2,     // deleted or renamed
            Modified = 4    // modified
        };
    }
    typedef Actions::Action Action;
    typedef CompileFileSystem::Path String;
    typedef unsigned long WatchID;

    struct WatchStruct;

    class FileWatcher
    {
    public:
        typedef std::map<WatchID, WatchStruct*> WatchMap;

    public:
        FileWatcher();
        virtual ~FileWatcher();
        WatchID AddWatch(const String& directory, FileWatchListener* watcher);
        WatchID AddWatch(const String& directory, FileWatchListener* watcher, bool recursive);
        void RemoveWatch(unsigned long watchid);
        void RemoveWatch(const String& directory);
        void Update();

        void HandleAction(WatchStruct* watch, const String& fileName, unsigned long action);
    private:
        WatchMap m_Watches;
        WatchID m_LastWatchID;

        FileWatcher_Alt* m_pAltWatcher;
    };



    class FileWatchListener
    {
    public:
        FileWatchListener() {}
        virtual ~FileWatchListener() {}


        virtual void handleFileAction(unsigned long watchid, const String& dir, const String& filename, FW::Action action) = 0;
    };
}

