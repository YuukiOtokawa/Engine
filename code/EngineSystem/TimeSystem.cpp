#include "TimeSystem.h"

std::chrono::duration<double> Time::targetFrameDuration;

std::chrono::duration<double, std::micro> Time::deltaTime; // 前フレームからの経過時間（ミリ秒）
std::chrono::duration<double, std::micro> Time::elapsedTime; // 起動からの累積時間（ミリ秒）
std::chrono::high_resolution_clock::time_point Time::lastFrameTime; // 前フレーム実行時のの時間（ミリ秒）
std::chrono::high_resolution_clock::time_point Time::lastSystemTime; // 前システム情報取得時のシステム時間（ミリ秒）
std::chrono::high_resolution_clock::time_point Time::currentTime; // 今フレーム実行時の時間（ミリ秒）
float Time::m_FramePerSecond = 0;
float Time::m_TimeScale = 1.0f;

std::vector<float> Time::m_FPSHistory; // 過去のFPS履歴を保存するベクター


DWORD Time::m_FrameCount = 0; // フレームカウント

using Clock = std::chrono::high_resolution_clock;

Time::Time()
{
	currentTime = std::chrono::high_resolution_clock::now();
	targetFrameDuration = std::chrono::duration<double>(1.0 / FRAME_PER_SECOND);
}

void Time::Update()
{
	currentTime = std::chrono::high_resolution_clock::now();
}

void Time::FrameUpdate()
{
	deltaTime = currentTime - lastFrameTime;
	elapsedTime += deltaTime;
	lastFrameTime = currentTime;
	m_FramePerSecond = 1000.0f / (float)(deltaTime.count() / 1000.0);

	m_FPSHistory.push_back(m_FramePerSecond);
	if (m_FPSHistory.size() > FPS_HISTORY_SIZE) {
		m_FPSHistory.erase(m_FPSHistory.begin()); // 古いFPS履歴を削除
	}
	//m_FrameCount++;
}

void Time::SystemTimeUpdate()
{
	lastSystemTime = currentTime;
}

void Time::Reset()
{
	currentTime = lastFrameTime = std::chrono::high_resolution_clock::now(); // システム時刻をミリ秒単位で取得
	m_FrameCount = 0;
}

#include <imgui.h>
#include <string>
void Time::DrawFPSGraph()
{
	char fps_text[64];
	sprintf_s(fps_text, "FPS: %10.f (%10.f ms)", (float)Time::FramePerSecond(), (float)(Time::deltaTime.count() / 1000.0));
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
		FLT_MAX,                        // グラフの最大値（5ms = 200FPS相当）
		ImVec2(0, 80)                 // グラフのサイズ (横幅0で自動、高さ80ピクセル)
	);
}
