#pragma once
#include "Component.h"
#include "Script.h"
#include "../ComponentFactory.h"

#include <memory>

class ScriptComponent :
    public Component
{
    std::unique_ptr<Script> m_ScriptInstance;
public:
    DECLARE_COMPONENT(ScriptComponent)

    ~ScriptComponent() override = default;

    void InitializeTag() override {
    }

    void UpdateComponent() override {
        if (m_ScriptInstance) {
            m_ScriptInstance->Update();
        }
    }

    void SetScript(std::unique_ptr<Script> script) {
        m_ScriptInstance = std::move(script);
        m_ScriptInstance->gameobject = this->owner;
    }
};

