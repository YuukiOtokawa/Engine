#pragma once
#include "Component.h"

#include "ComponentFactory.h"

#include "FBXImporter.h"
#include <unordered_map>

class ModelAnimator :
    public Component
{
public:
    DECLARE_COMPONENT(ModelAnimator)

    void UpdateComponent() override;
    void DrawGUI() override;
    void InitializeTag() override;

    //void SetAnimation(std::unordered_map<std::string,const aiScene*> animation) {
    //    m_Animation = animation;
    //}
};

