#pragma once
#include "Component.h"
#include "Script.h"
#include "ComponentFactory.h"

#include <memory>
#include "ScriptFactory.h"

#include "IObject.h"
#include "AUArray.h"
#include "IObjectFactorySystem.h"

class ScriptComponent :
    public Component
{
    Script* m_ScriptInstance;
    std::string m_ScriptName;
public:
    DECLARE_COMPONENT(ScriptComponent)
    ScriptComponent() {
        tag = ScriptTag;
        m_ClassID = CID_Script;
        ScriptFactory::GetInstance().RegisterComponent(this);
    }

    ~ScriptComponent() override {
        ScriptFactory::GetInstance().UnregisterComponent(this);
        delete m_ScriptInstance;
    }

    void InitializeTag() override {
    }

    void Start() override {
        if (m_ScriptInstance) {
            m_ScriptInstance->Start();
        }
    }

    void Update() override {
        if (m_ScriptInstance) {
            m_ScriptInstance->Update();
        }
    }

    void DrawGUI() override;

    void ImportFile(YAML::Node& node) override {
        if (node["tag"]) {
            tag = static_cast<Tag>(node["tag"].as<int>());
        }
        if (node["scriptName"]) {
            m_ScriptName = node["scriptName"].as<std::string>();
            m_ScriptInstance = ScriptFactory::GetInstance().CreateScript(m_ScriptName);
            if (m_ScriptInstance) {
                m_ScriptInstance->gameobject = this->owner;
                m_ScriptInstance->Import(node);
            }
        }
    }

    void ExportComponent(YAML::Emitter& out) override {
        if (m_ScriptInstance) {
            out << YAML::Key << "scriptName" << YAML::Value << m_ScriptInstance->GetScriptName();
            m_ScriptInstance->Export(out);
        }
    }

    void SetScript(Script* script) {
        m_ScriptInstance = script;
        if (m_ScriptInstance) {
            m_ScriptInstance->gameobject = this->owner;
            m_ScriptName = m_ScriptInstance->GetScriptName();
        }
    }

    void RecreateScript() {
        // 古いスクリプトインスタンスを安全に解放
        // Runtime Compiled C++システムが古いDLLのオブジェクトを管理するため、
        // 明示的にdeleteせずにrelease()で所有権を放棄
        if (m_ScriptInstance) {
            m_ScriptInstance = nullptr;
        }

        if (!m_ScriptName.empty()) {
            auto newScript = ScriptFactory::GetInstance().CreateScript(m_ScriptName);
            if (newScript) {
                newScript->gameobject = this->owner;
                m_ScriptInstance = newScript;
            }
        }
    }
};

// ScriptFactory::OnConstructorsAddedの実装
// ScriptComponentが完全に定義された後に実装する必要があるため、ここに配置
inline void ScriptFactory::OnConstructorsAdded() {
    // ホットリロード時にスクリプトを再登録
    m_Creators.clear();

    // RuntimeObjectSystemから全てのコンストラクタを取得して、
    // Scriptクラスを継承しているものを再登録
    if (g_pSystemTable && g_pSystemTable->pRuntimeObjectSystem) {
        auto* pObjectFactorySystem = g_pSystemTable->pRuntimeObjectSystem->GetObjectFactorySystem();
        if (pObjectFactorySystem) {
            // 全コンストラクタを取得
            AUDynArray<IObjectConstructor*> constructors;
            pObjectFactorySystem->GetAll(constructors);

            // 各コンストラクタをチェックしてScriptを継承しているものを登録
            for (size_t i = 0; i < constructors.Size(); ++i) {
                IObjectConstructor* pConstructor = constructors[i];
                if (pConstructor) {
                    // 一時的にインスタンスを作成してScriptかどうかチェック
                    IObject* pObj = pConstructor->Construct();
                    if (pObj) {
                        Script* pScript = dynamic_cast<Script*>(pObj);
                        if (pScript) {
                            // Scriptクラスを継承している場合、ScriptFactoryに登録
                            const char* className = pScript->GetScriptName();
                            RegisterScript(className, [pConstructor]() -> Script* {
                                return dynamic_cast<Script*>(pConstructor->Construct());
                            });
                        }
                        // 一時オブジェクトを削除（RuntimeObjectSystemが管理）
                        delete pObj;
                    }
                }
            }
        }
    }

    // 登録されている全てのScriptComponentのスクリプトを即座に再作成
    for (auto* component : m_RegisteredComponents) {
        if (component) {
            component->RecreateScript();
        }
    }
}
