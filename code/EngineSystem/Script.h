#pragma once

#include <string>
#include <vector>
#include <variant> // void* に代わる型安全な代替手段として std::variant を使用することも検討できます。

#include "Properties.h"
#include "PrefabAsset.h"

#include "ObjectInterfacePerModule.h"

struct ISimpleSerializer;
class Object;

// 前方宣言
namespace YAML {
    class Node;
}

enum class PropertyType {
    INT,
    FLOAT,
    STRING,
    BOOL,
    VECTOR2,
    VECTOR3,
    VECTOR4,
    METAFILE,
    DOUBLE,
    PREFAB,
    TEXTURE,
    // 他の型も必要に応じて追加
};

struct Property {
    std::string Name;
    PropertyType Type;
    void* Data;
};

class ScriptI :public IObject { };

class Script : public TInterface<IID_ENDInterfaceID, ScriptI>
{
    

    friend class ScriptComponent;

    std::vector<Property> m_Properties;


protected:
    void AddProperty(const std::string& name, PropertyType type, void* data) {
        m_Properties.push_back({ name, type, data });
    }

public:
    Object* gameobject = nullptr;
    virtual ~Script() = default;

    virtual void Start() {}

    virtual void Update() {}

    virtual void Import(YAML::Node& node);
    virtual void Export(YAML::Emitter& out);

    virtual const char* GetScriptName() const = 0;

    Object* GetGameObject() const { return gameobject; }

    const std::vector<Property>& GetProperties() const { return m_Properties; }
};

