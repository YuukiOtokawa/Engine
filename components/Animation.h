#pragma once
#include "Component.h"
class Animation :
    public Component
{
private:
	int m_FrameCount = 0;
    Vector4O m_uvRect = (1, 1);
	int m_SpriteCount = 1;
	int m_FramePerSecond = 30;
	bool m_Loop = true;
public:
    Animation() {
        m_ClassID = CID_Animation;
    }

	void UpdateComponent() override;

	void SetUVRect(const Vector4O& uvRect) { m_uvRect = uvRect; }
	void SetFramePerSecond(int fps) { m_FramePerSecond = fps; }
	void SetLoop(bool loop) { m_Loop = loop; }

	void DrawGUI() override;

	void InitializeTag() override {}
};

