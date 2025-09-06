#pragma once
#include "Script.h"
class PlayerControl :
    public Script
{
    float m_Speed = 5.0f;
public:
    void Start() override;
    void Update() override;


};

