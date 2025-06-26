#pragma once
#include "Component.h"
class MonoBehavior :
    public Component
{
public:
    MonoBehavior() {
        m_ClassID = CID_MonoBehaviour;
    }
    virtual void Start() = 0;
    virtual void Update() = 0;

    void UpdateComponent() override
    {
        Update();
	}

    void InitializeTag() override
    {
        
	}
};

