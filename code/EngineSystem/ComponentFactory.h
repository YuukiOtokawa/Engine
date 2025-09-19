#pragma once
#include "Component.h"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

using ComponentCreator = std::function<Component* ()>;

class ComponentFactory {

    ComponentFactory() = default;
    std::map<std::string, ComponentCreator> m_Creators;

public:
    static ComponentFactory* GetInstance() {
        static ComponentFactory instance;
        return &instance;
    }

    void RegisterComponent(const std::string& name, ComponentCreator creator) {
        m_Creators[name] = creator;
    }

    Component* CreateComponent(const std::string& name) {
        if (m_Creators.find(name) != m_Creators.end()) {
            return m_Creators[name]();
        }
        return nullptr;
    }

    std::vector<std::string> GetRegisteredComponentNames() {
        std::vector<std::string> names;
        for (const auto& pair : m_Creators) {
            names.push_back(pair.first);
        }
        return names;
    }
};

#define DECLARE_COMPONENT(ClassName) \
public: \
    static const char* GetStaticName() { return #ClassName; } \
    const char* GetComponentName() const override { return GetStaticName(); }

#define REGISTER_COMPONENT(ClassName) \
    namespace { \
        struct Registrar_##ClassName { \
            Registrar_##ClassName() { \
                ComponentFactory::GetInstance()->RegisterComponent(#ClassName, []() -> Component* { return new ClassName(); }); \
            } \
        }; \
        static Registrar_##ClassName registrar_##ClassName; \
    }

