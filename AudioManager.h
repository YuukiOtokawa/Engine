#pragma once

#include <xaudio2.h>

class AudioManager
{
private:
    static IXAudio2* m_pXAudio2;
    static IXAudio2MasteringVoice* m_pMasteringVoice;

    static AudioManager* m_Instance;
    AudioManager();
public:
    static AudioManager* GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new AudioManager();
        }
        return m_Instance;
    }

    ~AudioManager();

    static IXAudio2* GetXAudio2()
    {
        return m_pXAudio2;
    }
};

