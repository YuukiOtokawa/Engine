#include "EngineConsole.h"

#include "TimeSystem.h"
#include <chrono>

#include "imgui.h"

std::vector<LogEntry> EngineConsole::m_Logs;
char EngineConsole::m_Buff[CONSOLE_LOG_BUFFER_MAX];

void EngineConsole::LogInternal(const char* message, va_list args, LogLevel level)
{
	int result = vsnprintf(m_Buff, CONSOLE_LOG_BUFFER_MAX - 1, message, args);
	// Make sure there's a limit to the amount of rubbish we can output
	m_Buff[CONSOLE_LOG_BUFFER_MAX - 1] = '\0';

	// 現在の時刻を取得（Windowsシステム時刻）
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	std::tm now_tm;
	localtime_s(&now_tm, &now_c);
	// 時刻をフォーマット
	char timeBuffer[20];
	std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &now_tm);
	// ログエントリを作成して保存
	m_Logs.push_back({ m_Buff, timeBuffer, level });

}
void EngineConsole::Log(const char* message, ...)
{
	va_list args;
	va_start(args, message);
	LogInternal(message, args, LogLevel::Info);
}
void EngineConsole::LogWarning(const char* message, ...)
{
	va_list args;
	va_start(args, message);
	LogInternal(message, args, LogLevel::Warning);
}
void EngineConsole::LogError(const char* message, ...)
{
	va_list args;
	va_start(args, message);
	LogInternal(message, args, LogLevel::Error);
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
