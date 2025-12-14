#include "ConsoleWindow.h"

#include "imgui.h"
#include "EngineConsole.h"

void ConsoleWindow::Render()
{
	if (!StartWindow("Console")) return;
	// EngineConsoleのDrawメソッドを呼び出す
	EngineConsole::Draw();
	ImGui::End();
}
