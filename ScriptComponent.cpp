#include "ScriptComponent.h"

REGISTER_COMPONENT(ScriptComponent)

void ScriptComponent::DrawGUI()
{
	for (const auto& prop : m_ScriptInstance->GetProperties()) {
		ImGui::Text("%s:", prop.Name.c_str());
		switch (prop.Type) {
		case PropertyType::INT:
			ImGui::InputInt(("##" + prop.Name).c_str(), static_cast<int*>(prop.Data));
			break;
		case PropertyType::FLOAT:
			ImGui::InputFloat(("##" + prop.Name).c_str(), static_cast<float*>(prop.Data));
			break;
		case PropertyType::STRING:
			ImGui::InputText(("##" + prop.Name).c_str(), static_cast<std::string*>(prop.Data)->data(), 256);
			break;
		case PropertyType::BOOL:
			ImGui::Checkbox(("##" + prop.Name).c_str(), static_cast<bool*>(prop.Data));
			break;
		case PropertyType::VECTOR2:
			ImGui::InputFloat2(("##" + prop.Name).c_str(), reinterpret_cast<float*>(prop.Data));
			break;
		case PropertyType::VECTOR3:
			ImGui::InputFloat3(("##" + prop.Name).c_str(), reinterpret_cast<float*>(prop.Data));
			break;
		case PropertyType::VECTOR4:
			ImGui::InputFloat4(("##" + prop.Name).c_str(), reinterpret_cast<float*>(prop.Data));
			break;
		case PropertyType::METAFILE:
			// MetaFile の場合の処理をここに追加
			ImGui::Text("MetaFile property editing not implemented.");
			break;
		default:
			ImGui::Text("Unknown property type.");
			break;
		}
	}
}
