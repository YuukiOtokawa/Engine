#include "Animation.h"
#include "SpriteMesh.h"

#include "MainEngine.h"

#include "imgui.h"

REGISTER_COMPONENT(Animation)

void Animation::UpdateComponent() {
	if (m_FrameCount < 60.0f / m_FramePerSecond) {
		m_FrameCount++;
		return;
	}
	if (owner->GetComponent<SpriteMesh>() == nullptr) {
		MainEngine::ErrorMessage("SpriteMesh Not Found", owner->GetName());
	}
	bool stable = owner->GetComponent<SpriteMesh>()->SetSpriteByIndex(m_SpriteCount);
	m_SpriteCount++;
	if (!stable) {
		m_SpriteCount = 0;
		if (m_Loop == false)
			owner->Destroy();
	}
	m_FrameCount = 0;
}

void Animation::ExportComponent()
{
	CSVExporter::ExportInt(m_SpriteCount);
	CSVExporter::ExportInt(m_FramePerSecond);
	CSVExporter::ExportInt(m_Loop);
	CSVExporter::ExportInt(m_Playing);
}

void Animation::DrawGUI() {
	ImGui::Separator();
	ImGui::Text("Animation Component");
	ImGui::Indent();
	ImGui::Checkbox("Loop", &m_Loop);
	if (ImGui::Button("Play")) {
		m_Playing = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop")) {
		m_Playing = false;
	}
	ImGui::InputInt("Frame Per Second", &m_FramePerSecond);
	ImGui::BeginDisabled();
	ImGui::InputInt("Sprite Count", &m_SpriteCount,ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();
	ImGui::Unindent();

}