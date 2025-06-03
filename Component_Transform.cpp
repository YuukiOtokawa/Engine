#include "Component_Transform.h"

void Transform::Update() {

}

void Transform::Draw() {

}

void Transform::DrawGUI() {
	ImGui::Text("Transform");
	ImGui::InputFloat3("Position", &m_Position.x, "%.2f");
	ImGui::InputFloat3("Rotation", &m_Rotation.x, "%.2f");
	ImGui::InputFloat3("Scale", &m_Scale.x, "%.2f");
}

void Transform::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::SystemTag);
}