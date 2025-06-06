#pragma once
#include "Component.h"
class MonoBehavior :
    public Component
{
public:
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

