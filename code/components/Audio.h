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
    Audio() {
        m_ClassID = CID_AudioData;
    }
    ~Audio() = default;

    void InitializeTag() override {
        owner->SetDrawable(false); // Disable drawing for audio components
    }

    void Update() override {}

    void ExportComponent(YAML::Emitter& out) override {
        out << YAML::Key << "fileName" << YAML::Value << m_FileID.fileName;
        out << YAML::Key << "volume" << YAML::Value << m_Volume;
    }
    void ImportFile(YAML::Node& node) override {
        if (node["fileID"]) {
            m_FileID.fileName = node["fileName"].as<std::string>();
        }
        if (node["volume"]) {
            m_Volume = node["volume"].as<float>();
        }
    }

    void End() override { Stop(); }

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

