#pragma once

#include "MonoBehavior.h"

#include "Component_InputSystem.h"
#include "MainEngine.h"

class Game :
    public MonoBehaviour
{
public:
    void Start() override {}

    void Update() override {
        if (InputSystem::GetKeyboard()->GetInstance()->GetKeyDown(KK_ESCAPE)) {
            Editor::GetInstance()->ChangeScene("Result");
        }
    }
};
