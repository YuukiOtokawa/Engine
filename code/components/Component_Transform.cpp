// ========================================================
//
// タイトル[XXX]
// 
//									Date:YYYYMMDD
//									Author:Yuuki Otokawa
// ========================================================
#include "Component_Transform.h"

#include "CSVImporter.h"
#include "SceneImporter.h"

REGISTER_COMPONENT(Transform)

void Transform::Update() {

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

void Transform::ImportFile(YAML::Node& node)
{
	if (node["tag"]) {
		tag = static_cast<Tag>(node["tag"].as<int>());
	}
	if (node["position"]) {
		auto pos = node["position"];
		SetPosition(Vector4O(pos[0].as<float>(), pos[1].as<float>(), pos[2].as<float>(), pos[3].as<float>()));
	}
	if (node["rotation"]) {
		auto rot = node["rotation"];
		SetRotation(Vector4O(rot[0].as<float>(), rot[1].as<float>(), rot[2].as<float>(), rot[3].as<float>()));
	}
	if (node["scale"]) {
		auto scale = node["scale"];
		SetScale(Vector4O(scale[0].as<float>(), scale[1].as<float>(), scale[2].as<float>(), scale[3].as<float>()));
	}
}

