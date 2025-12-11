#include "AudioManager.h"

#include <string>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include "FrameWork.h"
#include "StringConverter.h"

#include <vector>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

IXAudio2* AudioManager::m_pXAudio2 = nullptr;
IXAudio2MasteringVoice* AudioManager::m_pMasteringVoice = nullptr;
AudioManager* AudioManager::m_Instance = nullptr;

std::list<AudioData*> AudioManager::m_Datas;

AudioManager::AudioManager()
{
	MFStartup(MF_VERSION, MFSTARTUP_FULL);
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

AudioData* AudioManager::GetAudioData(int fileID)
{
	for (auto data : m_Datas) {
		if (data->GetFileID() == fileID) {
			return data;
		}
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
	MFShutdown();
}

AudioDataFileID AudioManager::Load(std::string fileName)
{

	HRESULT hr = S_OK;

	IMFSourceReader* pReader = nullptr;
	IMFMediaType* pAudioType = nullptr;

	UINT32 size = 0;
	WAVEFORMATEX* pWaveFormat = nullptr;

	std::vector<BYTE> buf;

	AudioData* data = nullptr;
	for (auto exist : m_Datas) {
		if (exist->fileName == fileName)
			data = exist;
	}

	if (data != nullptr) {
		return AudioDataFileID(
			fileName,
			data->GetFileID()
		);
	}

	// ファイル名からSourceReaderを作成
	auto wFileName = ToWString(fileName);
	hr = MFCreateSourceReaderFromURL(wFileName.c_str(), NULL, &pReader);
	if (FAILED(hr)) goto FAILED_LOADING;

	// 読み込み形式の設定
	// PCM形式に変換する(非圧縮)
	hr = MFCreateMediaType(&pAudioType);
	if (FAILED(hr)) goto FAILED_LOADING;

	hr = pAudioType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	hr = pAudioType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);


	// SourceReaderに設定を反映
	hr = pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, pAudioType);
	if (FAILED(hr)) goto FAILED_LOADING;

	
	// 最終的なフォーマット情報(WAVEFORMATEX)を取得
	hr = pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pAudioType);
	if (FAILED(hr)) goto FAILED_LOADING;

	// WAVEFORMATEX構造体を取得
	hr = MFCreateWaveFormatExFromMFMediaType(
		pAudioType,
		&pWaveFormat,
		&size
	);
	if (FAILED(hr)) goto FAILED_LOADING;


	while (true) {
		IMFSample* pSample = nullptr;
		DWORD flags = 0;

		hr = pReader->ReadSample(
			MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			0,
			NULL,
			&flags,
			NULL,
			&pSample
		);
		if (FAILED(hr)) goto FAILED_LOADING;

		if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
			SAFE_RELEASE(pSample);
			break;
		}

		if (pSample) {
			IMFMediaBuffer* pBuf = nullptr;
			hr = pSample->ConvertToContiguousBuffer(&pBuf);

			BYTE* pData = nullptr;
			DWORD currentLen = 0;

			hr = pBuf->Lock(&pData, NULL, &currentLen);

			size_t oldSize = buf.size();
			buf.resize(oldSize + currentLen);
			memcpy(buf.data() + oldSize, pData, currentLen);

			pBuf->Unlock();
			SAFE_RELEASE(pBuf);
			SAFE_RELEASE(pSample);
		}
	}

	// メンバーに保存するやつ
	{
		auto len = static_cast<UINT32>(buf.size());
		auto audioData = new UCHAR[len];
		memcpy(audioData, buf.data(), len);

		auto pos = len / pWaveFormat->nBlockAlign;

		IXAudio2SourceVoice* pSourceVoice = nullptr;
		m_pXAudio2->CreateSourceVoice(&pSourceVoice, pWaveFormat);
		if (!pSourceVoice) goto FAILED_LOADING;

		data = new AudioData();
		data->length = len;
		data->data = audioData;
		data->position = pos;
		data->pSourceVoice = pSourceVoice;
		data->fileName = fileName;
		data->pFormat = pWaveFormat;
		m_Datas.push_back(data);
	}


	return AudioDataFileID(data->fileName,data->GetFileID());

FAILED_LOADING:
	// TODO [otokawa]:エラーログを流す
	SAFE_RELEASE(pReader);
	SAFE_RELEASE(pAudioType);
	return AudioDataFileID("error", -1);
}

void AudioManager::Play(int fileID, float volume, bool loop)
{
	auto data = GetAudioData(fileID);
	if (data) {
		XAUDIO2_BUFFER buf = {};

		buf.pAudioData = data->data;
		buf.AudioBytes = data->length;
		buf.Flags = XAUDIO2_END_OF_STREAM;

		buf.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 1;
		buf.LoopBegin = 0;

		buf.LoopLength = data->length / data->pFormat->nBlockAlign;

		data->pSourceVoice->SubmitSourceBuffer(&buf);

		data->pSourceVoice->SetVolume(volume);
		data->pSourceVoice->Start(0);
	}
}

void AudioManager::Stop(int fileID)
{
	auto data = GetAudioData(fileID);
	if (data) {
		data->pSourceVoice->Stop();
		data->pSourceVoice->FlushSourceBuffers();
		
	}
}

void AudioManager::Pause(int fileID)
{
	auto data = GetAudioData(fileID);
	if (data) {
		data->pSourceVoice->Stop();
	}
}

void AudioManager::Resume(int fileID)
{
	auto data = GetAudioData(fileID);
	if (data) {
		data->pSourceVoice->Start(0);
	}
}
