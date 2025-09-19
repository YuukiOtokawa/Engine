#pragma once

#include "Script.h"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

using ScriptCreator = std::function<Script* ()>;

class ScriptFactory {
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

private:
    // privateなメンバーは下にまとめるのが一般的
    ScriptFactory() = default;
    std::map<std::string, ScriptCreator> m_Creators;
};

// 修正後のマクロ
#define REGISTER_SCRIPT(ClassName) \
    namespace { \
        struct Registrar_##ClassName { \
            Registrar_##ClassName() { \
                ScriptFactory::GetInstance().RegisterScript(#ClassName, []() -> Script* { return new ClassName(); }); \
            } \
        }; \
        static Registrar_##ClassName registrar_##ClassName; \
    } \
    /* GetName()を実装する行を追加 */ \
    const char* ClassName::GetScriptName() const { return #ClassName; }
