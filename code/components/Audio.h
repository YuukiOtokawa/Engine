#pragma once
#include "Component.h"

#include <xaudio2.h>

class Audio :
    public Component
{
private:

    IXAudio2SourceVoice* m_pSourceVoice{};
    BYTE* m_pAudioData{};

    int m_iLength{};
    int m_iPosition{};
    bool m_Loop = true;
    int m_LoopCount = 0;

public:
    DECLARE_COMPONENT(Audio)
    Audio() = default;
    ~Audio() = default;

    void InitializeTag() override {
        owner->SetDrawable(false); // Disable drawing for audio components
    }

    void Update() override {}

    void ExportComponent() override {}

    void LoadAudio(const char* filename);
    void Release();
    void Play(bool loop = false);
    void Stop();
    void Pause();
    void Resume();
    void SetVolume(float volume) {
        m_pSourceVoice->SetVolume(volume);
    }
};

