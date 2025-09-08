#pragma once
#include "Component.h"

#include "ComponentFactory.h"

#include "FBXImporter.h"
#include <unordered_map>

class ModelAnimator :
    public Component
{
    std::unordered_map<std::string, const aiScene*> m_Animation;
    std::unordered_map<std::string, BONE> m_Bone;//ボーンデータ（名前で参照）
public:
    DECLARE_COMPONENT(ModelAnimator)

    void SetAnimation(std::unordered_map<std::string,const aiScene*> animation) {
        m_Animation = animation;
    }
};

