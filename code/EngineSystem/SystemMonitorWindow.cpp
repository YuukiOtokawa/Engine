#include "SystemMonitorWindow.h"

#include "TimeSystem.h"
#include "SystemMonitor.h"

#include "imgui.h"

void SystemMonitorWindow::Render()
{
    ImGui::Begin("System Monitor");

    Time::DrawFPSGraph();
	SystemMonitor::Draw();

    ImGui::End();
}
