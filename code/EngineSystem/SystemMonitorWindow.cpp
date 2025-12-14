#include "SystemMonitorWindow.h"

#include "TimeSystem.h"
#include "SystemMonitor.h"

#include "imgui.h"

void SystemMonitorWindow::Render()
{
    if (!StartWindow("System Monitor")) return;

    Time::DrawFPSGraph();
	SystemMonitor::Draw();

    ImGui::End();
}
