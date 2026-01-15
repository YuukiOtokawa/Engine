#include "TitlePool.h"

#include "ScriptFactory.h"
#include "imgui.h"

#include "MeshField.h"

REGISTER_SCRIPT(TitlePool)

void TitlePool::Start()
{
	cfg = kiss_fft_alloc(FFT_SIZE, 0, nullptr, nullptr);
	timeData.resize(FFT_SIZE);
	freqData.resize(FFT_SIZE);
	window.resize(FFT_SIZE);

	for (int i = 0; i < FFT_SIZE; ++i) {
		window[i] = 0.5f * (1.0f - cosf((2.0f * 3.14159265f * i) / (FFT_SIZE - 1))); // �n�j���O��
	}

	meshFieldComp = gameobject->GetComponent<MeshField>();
	audioComp = gameobject->GetComponent<Audio>();
	audioComp->PlayOneShot();
}

void TitlePool::Update()
{
	auto voice = audioComp->GetSourceVoice();

	XAUDIO2_VOICE_STATE state;
	voice->pSourceVoice->GetState(&state);

	auto totalAudioSamples = voice->length / (voice->pFormat->nBlockAlign / sizeof(short));

	UINT64 currPos = state.SamplesPlayed % totalAudioSamples;

	for (int i = 0; i < FFT_SIZE; ++i) {
		int index = (currPos + i) % totalAudioSamples;
		short* sampleData = reinterpret_cast<short*>(voice->data);
		timeData[i].r = static_cast<float>(sampleData[index * 2]) * window[i]; // ���`�����l��
		timeData[i].i = 0.0f;
	}

	kiss_fft(cfg, timeData.data(), freqData.data());

	for (int i = 0; i < FFT_SIZE / 2; ++i) {
		float magnitude = sqrtf(freqData[i].r * freqData[i].r + freqData[i].i * freqData[i].i);

		
		magnitude = log(magnitude + 1.0f) * logScaleValue; 

		magnitudes[i] = magnitude;
		meshFieldComp->SetHeight(i, magnitudes[i] * 0.1f);
	}

}

void TitlePool::DrawCustomGUI()
{
	ImGui::Separator();
	ImGui::Text("FFT Magnitudes");

	// 折れ線グラフで表示
	ImGui::PlotLines(
		"##FFT",
		magnitudes.data(),
		static_cast<int>(magnitudes.size()),
		0,
		nullptr,
		0.0f,
		FLT_MAX,
		ImVec2(0, 200)  // グラフの高さを200ピクセルに設定
	);

	// 追加情報の表示
	ImGui::Text("Samples: %d", static_cast<int>(magnitudes.size()));
}

REGISTERCLASS(TitlePool);
