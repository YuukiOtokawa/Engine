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

////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// IObject header file.
//
// The RuntimeCompiler library does not declare an IObject interface, only forward declares it.
// Hence each project can define their own base interface for objects they want to runtime compile
// and construct by using their own declaration of IObject in their own header file.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////



#pragma once

#include "ObjectInterface.h"

struct SimpleSerializer;
class ObjectFactorySystem;

enum InterfaceIDEnum {
    IID_IOBJECT,
    IID_ENDInterfaceID

};

typedef unsigned int InterfaceID;

template<InterfaceID Tiid, typename TSuper> struct TInterface : public TSuper {
    static const InterfaceID m_InterfaceID = Tiid;
    void GetInterface(InterfaceID iid, void** pReturn) {
        switch (iid) {
        case Tiid:
            *pReturn = this;
            break;
        default:
            TSuper::GetInterface(iid, pReturn);
        }
    }
};

struct RuntimeObject {
    static const InterfaceID m_InterfaceID = IID_IOBJECT;

    void GetInterface(InterfaceID iid, void** pReturn) {
        switch (iid) {
        case IID_IOBJECT:
            *pReturn = this;
            break;
        default:
            *pReturn = nullptr;
        }
    }

    template<typename T> void GetInterface(T** pReturn) {
        GetInterface(T::m_InterfaceID, (void**)pReturn);
    }

    RuntimeObject() : m_IsRuntimeDelete(false) {}
    virtual ~RuntimeObject() {}

    void Init(bool isFirstInit) {
        (void)isFirstInit;
    }

    PerTypeObjectId GetPerTypeId() const;

    void GetObjectId(ObjectId& id) const {
        id.m_ConstructorId = GetConstructor()->GetConstructorId();
        id.m_PerTypeId = GetPerTypeId();
    }
    ObjectId GetObjectId() const {
        ObjectId id;
        GetObjectId(id);
        return id;
    }

    ObjectConstructor* GetConstructor() const;

    void Serialize(SimpleSerializer* pSerializer) { (void)pSerializer; }

    const char* GetTypeName() const;

    bool IsRuntimeDelete() { return m_IsRuntimeDelete; }

private:
    friend class ObjectFactorySystem;

    bool m_IsRuntimeDelete;
};
