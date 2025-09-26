#pragma once
#include "Component.h"
class Animation :
    public Component
{
private:
	int m_FrameCount = 0;
	int m_SpriteCount = 1;
	int m_FramePerSecond = 30;
	bool m_Loop = true;
	bool m_Playing = true;
public:
    DECLARE_COMPONENT(Animation)
    Animation() {
        m_ClassID = CID_Animation;
    }

	void Update() override;

    void ExportComponent() override;

	void SetFramePerSecond(int fps) { m_FramePerSecond = fps; }
	void SetLoop(bool loop) { m_Loop = loop; }
	void SetPlaying(bool playing) { m_Playing = playing; }

	void DrawGUI() override;

	void InitializeTag() override {}
};

