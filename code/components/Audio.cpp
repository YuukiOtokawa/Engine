#include "Audio.h"


#include "MainEngine.h"

REGISTER_COMPONENT(Audio)

void Audio::LoadAudio(const char *filename)
{
	auto data = AudioManager::GetInstance()->Load(filename);
	m_FileID = data;
}

void Audio::Release()
{
	AudioManager::GetInstance()->Stop(m_FileID.FileID);
}

void Audio::Play()
{
	AudioManager::GetInstance()->Play(m_FileID.FileID, m_Volume, m_Loop);
}

void Audio::PlayOneShot()
{
	AudioManager::GetInstance()->Play(m_FileID.FileID, m_Volume);
}

void Audio::Stop()
{
	AudioManager::GetInstance()->Stop(m_FileID.FileID);
}

void Audio::Pause()
{
	AudioManager::GetInstance()->Pause(m_FileID.FileID);
}

void Audio::Resume()
{
	AudioManager::GetInstance()->Resume(m_FileID.FileID);
}

void Audio::DrawGUI()
{
	ImGui::Text("Audio Component");
	ImGui::Separator();
	ImGui::Text("File Name: %s", m_FileID.fileName.c_str());
	if (ImGui::Button("Load Audio")) {
		std::string filePath = OpenImportFileDialog();
		if (!filePath.empty()) {
			LoadAudio(filePath.c_str());
		}
	}
	float volume = m_Volume;
	if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f)) {
		SetVolume(volume);
	}
	ImGui::Checkbox("Loop", &m_Loop);

}

