#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

class Script;

using ScriptCreator = std::function<Script* ()>;

class ScriptComponent;

#include "IObjectFactorySystem.h"

class ScriptFactory : public IObjectFactoryListener {
public:
    // シングルトンインスタンスを取得
    static ScriptFactory& GetInstance() {
        static ScriptFactory instance;
        return instance;
    }

    // スクリプト生成関数を登録
    void RegisterScript(const std::string& name, ScriptCreator creator) {
        m_Creators[name] = creator;
    }

    // 名前を指定してスクリプトを生成
    Script* CreateScript(const std::string& name) {
        auto it = m_Creators.find(name);
        if (it != m_Creators.end()) {
            // findで見つけたイテレータを直接使う方が効率的
            return it->second();
        }
        return nullptr;
    }

    // 登録されている全スクリプトの名前リストを取得
    std::vector<std::string> GetRegisteredScriptNames() {
        std::vector<std::string> names;
        for (const auto& pair : m_Creators) {
            names.push_back(pair.first);
        }
        return names;
    }

    // 遅延登録用の関数を追加
    static void RegisterAllPendingScripts() {
        auto& functions = GetPendingRegistrations();
        for (auto func : functions) {
            func();
        }
        functions.clear();
    }


    void RegisterComponent(ScriptComponent* component) {
        m_RegisteredComponents.push_back(component);
    }
    void UnregisterComponent(ScriptComponent* component) {
        auto it = std::find(m_RegisteredComponents.begin(), m_RegisteredComponents.end(), component);
        if (it != m_RegisteredComponents.end()) {
            m_RegisteredComponents.erase(it);
        }
    }

    // IObjectFactoryListener implementation
    void OnConstructorsAdded() override;

private:
    ScriptFactory() = default;
    std::map<std::string, ScriptCreator> m_Creators;

    // 保留中の登録関数のリストを取得
    static std::vector<void(*)()>& GetPendingRegistrations() {
        static std::vector<void(*)()> pending;
        return pending;
    }

    friend struct RegisterScriptHelper;

    std::vector<ScriptComponent*> m_RegisteredComponents;
    std::atomic<bool> m_IsCompiling{ false };
};

// ヘルパー構造体
struct RegisterScriptHelper {
    static void AddPendingRegistration(void(*func)()) {
        ScriptFactory::GetPendingRegistrations().push_back(func);
    }
};

#include "MainEngine.h"
#include "SystemTable.h"
#include "RuntimeObjectSystem.h"

// REGISTER_SCRIPTマクロはこのヘッダーの最後に定義されています（循環依存回避のため）

// 安全性チェック付きの修正後のマクロ
#define REGISTER_SCRIPT(ClassName) \
    namespace { \
        struct Registrar_##ClassName { \
            static void Register() { \
                ScriptFactory::GetInstance().RegisterScript(#ClassName, []() -> Script* { \
                    return dynamic_cast<Script*>(g_pSystemTable->pRuntimeObjectSystem->GetObjectFactorySystem()->GetConstructor(#ClassName)->Construct()); \
                }); \
            } \
            Registrar_##ClassName() { \
                RegisterScriptHelper::AddPendingRegistration(&Register); \
            } \
        }; \
        static Registrar_##ClassName registrar_##ClassName; \
    } \
    const char* ClassName::GetScriptName() const { return #ClassName; }

#define DECLARE_SCRIPT(ClassName) \
    const char* GetScriptName() const override;

// OnConstructorsAddedの実装を見えるようにするため、ScriptComponent.hをインクルード
// Script.hからScriptFactory.hのインクルードを削除したので、循環依存は発生しない
#include "ScriptComponent.h"
