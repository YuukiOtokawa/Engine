#pragma once
#include "Component.h"
#include "Script.h"
#include "ComponentFactory.h"

#include <memory>
#include "ScriptFactory.h"

class ScriptComponent :
    public Component
{
    std::unique_ptr<Script> m_ScriptInstance;
public:
    DECLARE_COMPONENT(ScriptComponent)
    ScriptComponent() {
        tag = ScriptTag;
        m_ClassID = CID_Script;
    }

    ~ScriptComponent() override = default;

    void InitializeTag() override {
    }

    void Update() override {
        if (m_ScriptInstance) {
            m_ScriptInstance->Update();
        }
    }

    void DrawGUI() override;

    void ImportFile(std::vector<std::string>& tokens) override {
        std::string scriptName = tokens[4];
        m_ScriptInstance = std::unique_ptr<Script>(ScriptFactory::GetInstance().CreateScript(scriptName));
        if (m_ScriptInstance) {
            m_ScriptInstance->gameobject = this->owner;
            m_ScriptInstance->Import(tokens);
        }
    }

    void ExportComponent() {
        if (m_ScriptInstance) {
            CSVExporter::ExportString(m_ScriptInstance->GetScriptName());
            m_ScriptInstance->Export();
        }
    }

    void SetScript(std::unique_ptr<Script> script) {
        m_ScriptInstance = std::move(script);
        m_ScriptInstance->gameobject = this->owner;
    }
};

