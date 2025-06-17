// ========================================================
//
// ƒ^ƒCƒgƒ‹[XXX]
// 
//									Date:YYYYMMDD
//									Author:Yuuki Otokawa
// ========================================================
#include "Component_Transform.h"

void Transform::UpdateComponent() {

}

void Transform::Draw() {

}

void Transform::DrawGUI() {
	ImGui::Separator();
	ImGui::Text("Transform");
	ImGui::Indent();
	ImGui::InputFloat3("Position", &m_Position.x, "%.2f");
	ImGui::InputFloat3("Rotation", &m_Rotation.x, "%.2f");
	ImGui::InputFloat3("Scale", &m_Scale.x, "%.2f");
	ImGui::Unindent();
}

void Transform::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::SystemTag);
}