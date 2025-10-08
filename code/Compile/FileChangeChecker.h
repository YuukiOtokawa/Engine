#pragma once

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

#include <vector>

struct FileChangeListener;

struct FileChangeChecker
{
    virtual bool IsMonitoringActive() const = 0;
    virtual void SetMonitoringActive(bool bActive) = 0;

    virtual float GetMinTimeBetweenNotifications() const = 0;
    virtual void SetMinTimeBetweenNotifications(float fMinTime) = 0;

    virtual float GetChangeNotifyDelay() const = 0;
    virtual void SetChangeNotifyDelay(float fDelay) = 0;

    virtual void Update(float fTimeDelta) = 0;
    virtual void Watch(const char* filename, FileChangeListener* pListener) = 0;

    virtual void RemoveListener(FileChangeListener* pListener) = 0;
    virtual ~FileChangeChecker() {}
};

struct FileChangeListener
{
    FileChangeListener() : _registeredChecker(0) {}
    virtual ~FileChangeListener()
    {
        if (_registeredChecker)
        {
            _registeredChecker->RemoveListener(this);
        }
    }

    virtual void OnFileChange(const std::vector<const char*>& filelist) = 0;

    void OnRegisteredWithChecker(FileChangeChecker* pChecker)
    {
        _registeredChecker = pChecker;
    }

private:
    FileChangeChecker* _registeredChecker;
};
