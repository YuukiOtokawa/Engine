#pragma once
#include "MonoBehavior.h"

#include "Component_InputSystem.h"
#include "MainEngine.h"

class Title :
    public MonoBehavior
{
public:
    void Start() override {}

    void Update() override {
        if (InputSystem::GetKeyboard()->GetInstance()->GetKeyDown(KK_ENTER)) {
            Editor::GetInstance()->ChangeScene("Game");
        }
    }
};
