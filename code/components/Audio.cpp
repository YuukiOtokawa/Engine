#include "Audio.h"

#include "AudioManager.h"

REGISTER_COMPONENT(Audio)

void Audio::LoadAudio(const char *filename)
{
	int index = -1;

	WAVEFORMATEX waveFormat = {};

	{
		HMMIO hmmio = NULL;
		MMIOINFO mmioinfo = { 0 };
		MMCKINFO riffchunkinfo = { 0 };
		MMCKINFO datachunkinfo = { 0 };
		MMCKINFO mmckinfo = { 0 };
		UINT32 buflen;
		LONG readlen;


		hmmio = mmioOpen((LPSTR)filename, &mmioinfo, MMIO_READ);
		assert(hmmio);//falseなら止まる

		riffchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		mmioDescend(hmmio, &riffchunkinfo, NULL, MMIO_FINDRIFF);

		mmckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescend(hmmio, &mmckinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		if (mmckinfo.cksize >= sizeof(WAVEFORMATEX))
		{
			mmioRead(hmmio, (HPSTR)&waveFormat, sizeof(waveFormat));
		}
		else
		{
			PCMWAVEFORMAT pcmwf = { 0 };
			mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));
			memset(&waveFormat, 0x00, sizeof(waveFormat));
			memcpy(&waveFormat, &pcmwf, sizeof(pcmwf));
			waveFormat.cbSize = 0;
		}
		mmioAscend(hmmio, &mmckinfo, 0);

		datachunkinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioDescend(hmmio, &datachunkinfo, &riffchunkinfo, MMIO_FINDCHUNK);



		buflen = datachunkinfo.cksize;
		m_pAudioData = new unsigned char[buflen];
		readlen = mmioRead(hmmio, (HPSTR)m_pAudioData, buflen);


		m_iLength = readlen;
		m_iPosition = readlen / waveFormat.nBlockAlign;


		mmioClose(hmmio, 0);
	}

	AudioManager::GetInstance()->GetXAudio2()->CreateSourceVoice(&m_pSourceVoice, &waveFormat);
	assert(m_pSourceVoice); // falseなら止まる
}

void Audio::Release()
{
	m_pSourceVoice->Stop();
	m_pSourceVoice->DestroyVoice();

	delete[] m_pAudioData;
	m_pAudioData = nullptr;
}

void Audio::Play(bool loop)
{
	m_pSourceVoice->Stop();
	m_pSourceVoice->FlushSourceBuffers();

	XAUDIO2_BUFFER buffer = {};

	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_iLength;
	buffer.pAudioData = m_pAudioData;
	buffer.PlayBegin = 0;
	buffer.PlayLength = m_iPosition;

	if (loop) {
		buffer.LoopBegin = 0;
		buffer.LoopLength = m_iPosition;
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	m_pSourceVoice->SubmitSourceBuffer(&buffer);
	m_pSourceVoice->Start();
}

void Audio::Stop()
{
	if (m_pSourceVoice) {
		m_pSourceVoice->Stop();
		m_pSourceVoice->FlushSourceBuffers();
	}
}

void Audio::Pause()
{
	if (m_pSourceVoice) {
		m_pSourceVoice->Stop();
	}
}

void Audio::Resume()
{
	if (m_pSourceVoice) {
		m_pSourceVoice->Start();
	}
}
