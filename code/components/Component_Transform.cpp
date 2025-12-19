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

#include "ObjectNode.h"

REGISTER_COMPONENT(Transform)

void Transform::Update() {

}

void Transform::DrawGUI() {
    ImGui::Indent();
    if (ImGui::DragFloat3("Position", &m_Position.x, 0.1f) && owner->GetObjectNode() != nullptr) {
        owner->GetObjectNode()->SetPosition(m_Position);
    }
    
    // Rotationの変更を検出してQuaternionに適用
    if (ImGui::DragFloat3("Rotation", &m_Rotation.x, 0.1f)) {
        SetRotation(m_Rotation); // SetRotationで自動的にQuaternionに同期
    }
    
    // Quaternionの変更を検出してRotationに適用
    if (ImGui::DragFloat4("Quaternion", &m_Quaternion.x, 0.01f)) {
        SetQuaternion(m_Quaternion); // SetQuaternionで自動的にEuler角に同期
    }
    
    ImGui::DragFloat3("Scale", &m_Scale.x, 0.1f);
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

