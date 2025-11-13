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


#pragma once

#include <vector>
#include <stdlib.h>

struct RuntimeObject;
struct RuntimeObjectSystem;

const size_t InvalidId = (size_t)-1;    // 無効なIDを表す定数
typedef size_t PerTypeObjectId;         // 型ごとのオブジェクトIDを表す型
typedef size_t ConstructorId;           // コンストラクタIDを表す型

struct ObjectId {
    ObjectId() : m_PerTypeId(InvalidId), m_ConstructorId(InvalidId) {}

    PerTypeObjectId m_PerTypeId;        // 型ごとのオブジェクトID
    ConstructorId   m_ConstructorId;    // コンストラクタID

    // 比較演算子のオーバーロード
    bool operator<(ObjectId lhs) const {
        // コンストラクタIDを比較
        if (m_ConstructorId < lhs.m_ConstructorId) {
            return true;
        }
        if (m_ConstructorId == lhs.m_ConstructorId) {
            return m_PerTypeId < lhs.m_PerTypeId;
        }
        return false;
    }

    bool operator==(const ObjectId& rhs) const {
        return (m_ConstructorId == rhs.m_ConstructorId && m_PerTypeId == rhs.m_PerTypeId);
    }

    bool IsValid() const {
        return (m_ConstructorId != InvalidId && m_PerTypeId != InvalidId);
    }
    void SetInvalid() {
        m_ConstructorId = InvalidId;
        m_PerTypeId = InvalidId;
    }
};

struct RuntimeTrackingInfo;

struct ObjectConstructor {
    RuntimeObject* Construct();
    void ConstructNull();
    const char* GetName();
    const char* GetFileName();
    const char* GetCompiledPath();
    size_t GetMaxNumTrackingInfo() const;
    RuntimeTrackingInfo GetTrackingInfo(size_t num) const;
    void SetProjectId(unsigned int id);
    unsigned short GetProjectId() const;

    bool GetIsSingleton() const;
    bool GetIsAutoConstructSingleton() const;
    RuntimeObject* GetSingleton() { return Construct(); }

    RuntimeObject* GetConstructedObject(PerTypeObjectId num) const;
    size_t GetNumberConstructedObjects() const;
    ConstructorId GetConstructorId() const;
    void SetConstructorId(ConstructorId id);
    void ClearIfAllDeleted();

    ~ObjectConstructor() {}
};


#ifdef _WIN32
typedef PerModuleInterface* (__cdecl* GETPerModuleInterface_PROC)(void);
#else
typedef PerModuleInterface* (*GETPerModuleInterface_PROC)(void);
#endif
