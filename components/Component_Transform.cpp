// ========================================================
//
// タイトル[XXX]
// 
//									Date:YYYYMMDD
//									Author:Yuuki Otokawa
// ========================================================
#include "Component_Transform.h"

#include "CSVImporter.h"



REGISTER_COMPONENT(Transform)

void Transform::UpdateComponent() {

}

void Transform::Draw() {

}

void Transform::DrawGUI() {
	ImGui::Indent();
	ImGui::InputFloat3("Position", &m_Position.x, "%.2f");
	ImGui::InputFloat3("Rotation", &m_Rotation.x, "%.2f");
	ImGui::InputFloat3("Scale", &m_Scale.x, "%.2f");
	ImGui::Unindent();
}

void Transform::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::SystemTag);
}

void Transform::ImportFile(std::vector<std::string>& tokens)
{
	SetPosition(CSVImporter::ImportVector4O(tokens[4], tokens[5], tokens[6], tokens[7]));
	SetRotation(CSVImporter::ImportVector4O(tokens[8], tokens[9], tokens[10], tokens[11]));
	SetScale(CSVImporter::ImportVector4O(tokens[12], tokens[13], tokens[14], tokens[15]));
}
