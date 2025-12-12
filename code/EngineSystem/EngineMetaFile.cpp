#include "EngineMetaFile.h"

int EngineMetaFile::m_FileIDCounter = 0;

std::string EngineMetaFile::GetDropPath(std::string fileType)
{
    std::string result = "";
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(fileType.c_str())) {
            if (payload->IsDelivery()) {
                result = std::string(static_cast<const char*>(payload->Data), payload->DataSize);
            }
        }
        ImGui::EndDragDropTarget();
    }

    return result;
}