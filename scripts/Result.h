#pragma once
#include "MonoBehavior.h"

#include "Component_InputSystem.h"
#include "MainEngine.h"

class Result :
    public MonoBehaviour
{
public:
    void Start() override {}

    void OnUpdate() override {
        if (InputSystem::GetKeyboard()->GetInstance()->GetKeyDown(KK_ENTER)) {
            Editor::GetInstance()->ChangeScene("Title");
        }
    }
};
