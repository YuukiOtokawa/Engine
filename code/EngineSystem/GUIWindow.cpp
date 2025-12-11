#include "GUIWindow.h"

#include "GUI.h"

void GUIWindow::StartWindow(std::string windowName, ImGuiWindowFlags flag)
{
	if (!m_IsOpen) return;

	m_IsOpen = GUI::BeginWindow(windowName.c_str(), flag);

	if (!m_IsOpen) {
		ImGui::End();
		return;
	}
}
