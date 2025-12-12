#include "EngineMetaFile.h"

int EngineMetaFile::m_FileIDCounter = 0;

std::string EngineMetaFile::GetDropPath(std::string fileType)
{
    std::string result = "";
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

    return result;
}