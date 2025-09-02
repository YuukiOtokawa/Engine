#pragma once
#include "Component.h"
class MonoBehaviour :
    public Component
{
private:
    bool isStarted = false; // Flag to check if Start has been called
public:
    MonoBehaviour() {
        m_ClassID = CID_MonoBehaviour;
    }
    virtual void Start() = 0;
    virtual void Update() = 0;

    void UpdateComponent() override
    {
        if (!isStarted) {
            Start();
            isStarted = true; // Set the flag to true after Start is called
        }
        Update();
	}

    void InitializeTag() override
    {
        
	}
};

