#pragma once

#include <xaudio2.h>

#include <list>

#include "EngineMetaFile.h"

class AudioData : public EngineMetaFile {
public:
    UINT32 length = 0;
    UCHAR* data;
    UINT position = 0;
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    std::string fileName;
    WAVEFORMATEX* pFormat;
};

struct AudioDataFileID {
    std::string fileName;
    int FileID;
};

class AudioManager
{
private:
    static IXAudio2* m_pXAudio2;
    static IXAudio2MasteringVoice* m_pMasteringVoice;

    static AudioManager* m_Instance;

    static std::list<AudioData*> m_Datas;
    AudioManager();

public:
    static AudioData* GetAudioData(int fileID);
    static AudioManager* GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new AudioManager();
        }
        return m_Instance;
    }

    ~AudioManager();

    // Media Foundationを使用した音声ファイルの読み込み
    static AudioDataFileID Load(std::string fileName);

    static IXAudio2* GetXAudio2()
    {
        return m_pXAudio2;
    }

    static void Play(int fileID, float volume, bool loop = false);
    static void Stop(int fileID);
    static void Pause(int fileID);
    static void Resume(int fileID);
};

