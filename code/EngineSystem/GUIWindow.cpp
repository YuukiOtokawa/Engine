#include "GUIWindow.h"

#include "GUI.h"

#include "Object.h"

void GUIWindow::StartWindow(std::string windowName, ImGuiWindowFlags flag)
{
	if (!m_IsOpen) return;

	m_IsOpen = GUI::BeginWindow(windowName.c_str(), flag);

	if (!m_IsOpen) {
		ImGui::End();
		return;
	}
}

std::string GUIWindow::GetDropPath(std::string fileType)
{
    std::string result = "";
    ImVec2 pos = ImGui::GetCursorPos();
    ImGui::Dummy(ImGui::GetContentRegionAvail());
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(fileType.c_str())) {
            if (payload->IsDelivery()) {
                result = std::string(static_cast<const char*>(payload->Data), payload->DataSize);
                // null終端文字が含まれている場合は削除
                size_t nullPos = result.find('\0');
                if (nullPos != std::string::npos) {
                    result = result.substr(0, nullPos);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::SetCursorPos(pos);
    return result;
}

Object* GUIWindow::GetDropIterator(std::string fileType)
{
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(fileType.c_str())) {
            if (payload->IsDelivery()) {
                // payload->DataはObject**なのでデリファレンスしてObject*を取得
                auto result = *static_cast<Object**>(payload->Data);
                return result;
            }
        }
        ImGui::EndDragDropTarget();
    }

    return nullptr;
}
