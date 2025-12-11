#pragma once
#include "SpriteMesh.h"
class Billboard :
    public SpriteMesh
{
public:
    Billboard() : SpriteMesh() {
        m_ClassID = CID_Billboard;
    }

    virtual void SetOwner(Object* obj) override {
        SpriteMesh::SetOwner(obj);
        InitializeTag();
    }

    void InitializeTag() override {
        owner->SetTag(GameObjectTagLayer::BillBoardTag);
    }

    void Update() override;
};

