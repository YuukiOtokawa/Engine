#include "TimeSystem.h"

DWORD Time::deltaTime = 0; // 前フレームからの経過時間（ミリ秒）
DWORD Time::elapsedTime = 0; // 起動からの累積時間（ミリ秒）
DWORD Time::lastTime = 0; // 前フレーム実行時のの時間（ミリ秒）
DWORD Time::currentTime = 0; // 今フレーム実行時の時間（ミリ秒）
int Time::m_FramePerSecond = 0;
float Time::m_TimeScale = 1.0f;

std::vector<float> Time::m_FPSHistory; // 過去のFPS履歴を保存するベクター


DWORD Time::m_FrameCount = 0; // フレームカウント

void Time::Update()
{
	currentTime = timeGetTime();
}

void Time::FrameUpdate()
{
	deltaTime = currentTime - lastTime;
	elapsedTime += deltaTime;
	lastTime = currentTime;
	m_FramePerSecond = 1000 / deltaTime;

	m_FPSHistory.push_back(m_FramePerSecond);
	if (m_FPSHistory.size() > FPS_HISTORY_SIZE) {
		m_FPSHistory.erase(m_FPSHistory.begin()); // 古いFPS履歴を削除
	}
	//m_FrameCount++;
}

void Time::Reset()
{
	lastTime = timeGetTime(); // システム時刻をミリ秒単位で取得
	currentTime = m_FrameCount = 0;
}

#include <imgui.h>
#include <string>
void Time::DrawFPSGraph()
{
	char fps_text[64];
	sprintf_s(fps_text, "FPS: %.1f (%.3f ms)", (float)Time::FramePerSecond(), (float)Time::deltaTime);
	ImGui::Text(fps_text);

	// グラフのタイトルを作成
	// データがまだ溜まっていない場合は計算しない
	std::string plot_title = "";
	auto fps_history = Time::GetFPSHistory();
	if (!fps_history.empty()) {
		float max_flame_time = (*std::max_element(fps_history.begin(), fps_history.end()));
		plot_title = std::to_string(max_flame_time);
	}

	// PlotLines関数でグラフを描画
	ImGui::PlotLines(
		plot_title.c_str(),           // グラフのラベル
		fps_history.data(),           // データ配列のポインタ
		static_cast<int>(fps_history.size()), // データ数
		0,                            // データのオフセット（通常は0）
		nullptr,                      // オーバーレイテキスト（今回は使わない）
		0.0f,                         // グラフの最小値（オートスケール）
		10.0f,                        // グラフの最大値（50ms = 20FPS相当）
		ImVec2(0, 80)                 // グラフのサイズ (横幅0で自動、高さ80ピクセル)
	);
}
