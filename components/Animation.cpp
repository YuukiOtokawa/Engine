#include "Animation.h"
#include "SpriteMesh.h"

#include "MainEngine.h"

#include "imgui.h"

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

void Animation::DrawGUI() {
	ImGui::Separator();
	ImGui::Text("Animation Component");
	ImGui::Indent();
	ImGui::Checkbox("Loop", &m_Loop);
	ImGui::InputInt("Frame Per Second", &m_FramePerSecond);
	ImGui::BeginDisabled();
	ImGui::InputInt("Sprite Count", &m_SpriteCount,ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();
	ImGui::Unindent();

}