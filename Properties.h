#pragma once

#include "../VectorO.h"
#include "../EngineMetaFile.h"

#define REGISTER_PROPERTY(type, name) \
    if constexpr (std::is_same_v<decltype(type), int>) { \
        AddProperty(name, PropertyType::INT, &type); \
    } else if constexpr (std::is_same_v<decltype(type), float>) { \
        AddProperty(name, PropertyType::FLOAT, &type); \
    } else if constexpr (std::is_same_v<decltype(type), std::string>) { \
        AddProperty(name, PropertyType::STRING, &type); \
    } else if constexpr (std::is_same_v<decltype(type), bool>) { \
        AddProperty(name, PropertyType::BOOL, &type); \
    } else if constexpr (std::is_same_v<decltype(type), Vector2O>) { \
        AddProperty(name, PropertyType::VECTOR2, &type); \
    } else if constexpr (std::is_same_v<decltype(type), Vector3O>) { \
        AddProperty(name, PropertyType::VECTOR3, &type); \
    } else if constexpr (std::is_same_v<decltype(type), Vector4O>) { \
        AddProperty(name, PropertyType::VECTOR4, &type); \
    } else if constexpr (std::is_same_v<decltype(type), EngineMetaFile>) { \
        AddProperty(name, PropertyType::METAFILE, &type); \
    }
