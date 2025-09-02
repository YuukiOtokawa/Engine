#pragma once
#include "Component_MeshFilter.h"
class SpriteMesh :
    public MeshFilter
{
private:
	static constexpr int SPRITE_VERTICES = 4;
	Vector4O m_uvRect = (1,1);
	int m_spriteIndex = 0;
    int m_newIndex = -1;
public:
	SpriteMesh();

	void UpdateComponent() override;

    void DrawGUI() override;
    void ExportComponent() override;

	void SetUVRect(Vector4O uvRect) {
        m_uvRect = uvRect;
        SetSpriteByIndex(0); // Reset to first sprite
    }

	bool SetSpriteByIndex(int index);

    void InitializeTag() override
    {
        owner->SetTag(GameObjectTagLayer::SpriteTag);
    }

    void Draw() override;
};

