#pragma once
#include "Component.h"

#include "AudioManager.h"

class Audio :
    public Component
{
private:

    AudioDataFileID m_FileID;

    int m_iLength{};
    int m_iPosition{};
    bool m_Loop = true;
    int m_LoopCount = 0;
    float m_Volume = 1.0f;

public:
    DECLARE_COMPONENT(Audio)
    Audio() = default;
    ~Audio() = default;

    void InitializeTag() override {
        owner->SetDrawable(false); // Disable drawing for audio components
    }

    void Update() override {}

    void ExportComponent(YAML::Emitter& out) override {}

    void LoadAudio(const char* filename);
    void Release();
    void Play();
    void PlayOneShot();
    void Stop();
    void Pause();
    void Resume();
    void SetVolume(float volume) {
        if (volume < 0.0f)
            m_Volume = 0.0f;
        else if (volume > 1.0f)
            m_Volume = 1.0f;
        else
            m_Volume = volume;
    }

    void DrawGUI() override;
};

