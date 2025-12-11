#include "ConsoleWindow.h"

#include "imgui.h"
#include "EngineConsole.h"

void ConsoleWindow::Render()
{
	ImGui::Begin("Console");
	// EngineConsoleのDrawメソッドを呼び出す
	EngineConsole::Draw();
	ImGui::End();
}
