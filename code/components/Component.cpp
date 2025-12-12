#include "Component.h"
#include "Editor.h"

Component::Component() : EngineMetaFile() {
}

void Component::RightClickMenu()
{
	if (ImGui::BeginPopupContextItem(std::to_string(m_FileID).c_str())) {
		if (ImGui::Selectable("Delete Object")) {
			Destroy();
		}
		ImGui::EndPopup();
	}
}

