#pragma once
#include "Component.h"

class Object;
class Sky :
    public Component
{
    float m_Size = 100.0f;
    Object* m_pCamera = nullptr;
public:
    DECLARE_COMPONENT(Sky)

    void Update() override;
};

