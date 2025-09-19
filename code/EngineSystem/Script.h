#pragma once

#include <string>
#include <vector>
#include <variant> // void* に代わる型安全な代替手段として std::variant を使用することも検討できます。

#include "Properties.h"

class Object;

enum class PropertyType {
    INT,
    FLOAT,
    STRING,
    BOOL,
    VECTOR2,
    VECTOR3,
    VECTOR4,
    METAFILE,
    // 他の型も必要に応じて追加
};

struct Property {
    std::string Name;
    PropertyType Type;
    void* Data;
};

class Script 
{

    friend class ScriptComponent;

    std::vector<Property> m_Properties;

protected:
    Object* gameobject = nullptr;
    void AddProperty(const std::string& name, PropertyType type, void* data) {
        m_Properties.push_back({ name, type, data });
    }

public:
    virtual ~Script() = default;

    virtual void Start() {}

    virtual void Update() {}

    virtual void Import(std::vector<std::string> tokens) {}
    virtual void Export() {}

    virtual const char* GetScriptName() const = 0;

    Object* GetGameObject() const { return gameobject; }

    const std::vector<Property>& GetProperties() const { return m_Properties; }
};

