#pragma once
#include "Component.h"

#include "ComponentFactory.h"

class MonoBehaviour :
    public Component
{
private:
    bool isStarted = false; // Flag to check if Start has been called
public:

    MonoBehaviour() {
        m_ClassID = CID_Script;
    }
    virtual void Start() = 0;
    virtual void OnUpdate() = 0;

    void Update() override;

    void InitializeTag() override
    {
        
	}
};

