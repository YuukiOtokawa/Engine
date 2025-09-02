#include "AudioManager.h"

IXAudio2* AudioManager::m_pXAudio2 = nullptr;
IXAudio2MasteringVoice* AudioManager::m_pMasteringVoice = nullptr;
AudioManager* AudioManager::m_Instance = nullptr;

AudioManager::AudioManager()
{
	// Initialize XAudio2
	HRESULT hr = XAudio2Create(&m_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		// Handle error
	}
	// Create mastering voice
	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if (FAILED(hr)) {
		// Handle error
	}
}

AudioManager::~AudioManager()
{
	// Release resources
	if (m_pMasteringVoice) {
		m_pMasteringVoice->DestroyVoice();
		m_pMasteringVoice = nullptr;
	}
	if (m_pXAudio2) {
		m_pXAudio2->Release();
		m_pXAudio2 = nullptr;
	}
}
