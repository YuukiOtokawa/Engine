#include "EngineConsole.h"

#include "TimeSystem.h"
#include <chrono>

#include "imgui.h"

std::vector<LogEntry> EngineConsole::m_Logs;

void EngineConsole::Log(const std::string& message, LogLevel level)
{
	// 現在の時刻を取得
	auto now = Time::currentTime;
	std::time_t now_c = static_cast<std::time_t>(now / 1000); // ミリ秒を秒に変換
	
	std::tm now_tm;
	localtime_s(&now_tm, &now_c);
	// 時刻をフォーマット
	char timeBuffer[20];
	std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &now_tm);
	// ログエントリを作成して保存
	m_Logs.push_back({ message, timeBuffer, level });
}

void EngineConsole::Clear()
{
	m_Logs.clear();
}

void EngineConsole::Draw()
{
	// Draw()の先頭付近にstatic変数を追加
	static bool autoScroll = true;

	// ここにImGuiを使った描画コードを追加
	if (ImGui::Button("Clear")) {
		Clear();
		autoScroll = true; // クリア時は自動スクロールを再開
	}
	ImGui::Separator();

	// ユーザーが手動でスクロールしたか判定
	if (autoScroll) {
		// スクロールバーが一番下以外なら自動スクロールをやめる
		if (ImGui::GetScrollY() < ImGui::GetScrollMaxY()) {
			if (ImGui::IsWindowHovered() && (ImGui::GetIO().MouseWheel != 0 || ImGui::IsMouseDown(ImGuiMouseButton_Left))) {
				autoScroll = false;
			}
		}
	}

	for (const auto& log : m_Logs) {
		ImVec4 color;
		switch (log.level) {
		case LogLevel::Info:
			color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case LogLevel::Warning:
			color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		case LogLevel::Error:
			color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		}
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::Text("[%s] %s", log.timestamp.c_str(), log.message.c_str());
		ImGui::PopStyleColor();
	}

	// 自動スクロールが有効なら一番下へ
	if (autoScroll && !m_Logs.empty()) {
		ImGui::SetScrollHereY(1.0f);
	}

}
