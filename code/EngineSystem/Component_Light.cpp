#include "Component_Light.h"

#include "MainEngine.h"

#include "Component_Transform.h"

Vector4O Light::m_Ambient = Vector4O(0.2f, 0.2f, 0.2f, 1.0f); // 環境光の色
DIRECTIONAL_LIGHT Light::m_DirectionalLight = {};
POINT_LIGHT Light::m_PointLight = {};
SPOT_LIGHT Light::m_SpotLight = {};

REGISTER_COMPONENT(Light)

void Light::InitializeTag()
{
	owner->SetTag(GameObjectTagLayer::LightTag);
	owner->SetLayer(GameObjectLayer::LightLayer);
	owner->SetDrawable(false);
}

void Light::Draw()
{
	auto renderer = MainEngine::GetInstance()->GetRenderCore();

	switch (m_Type)
	{
	case LIGHTTYPE_DIRECTIONAL:
		m_DirectionalLight = {
			m_isEnable,
			{},
			m_Diffuse,
			m_Direction
		};
		break;
	case LIGHTTYPE_POINT:
		m_PointLight = {
			m_isEnable,
			{},
			m_Range,
			m_Diffuse,
			owner->GetComponent<Transform>()->GetPosition()
		};
		break;
	case LIGHTTYPE_SPOT:
		m_SpotLight = {
			m_isEnable,
			m_Range,
			m_InnerAngle,
			m_OuterAngle,
			m_Diffuse,
			owner->GetComponent<Transform>()->GetPosition(),
			m_Direction
		};
		break;
	default:
		break;
	}
}

void Light::DrawGeneralLight()
{
	auto renderer = MainEngine::GetInstance()->GetRenderCore();
	LIGHT_BUFFER lightBuffer = {};
	lightBuffer.ambient = m_Ambient;
	lightBuffer.directionalLight = m_DirectionalLight;
	lightBuffer.pointLight = m_PointLight;
	lightBuffer.spotLight = m_SpotLight;
	renderer->SetLightBuffer(lightBuffer);
	m_DirectionalLight = {};
	m_PointLight = {};
	m_SpotLight = {};
}

void Light::DrawGUI()
{
	ImGui::Indent();
	ImGui::Combo("Type", reinterpret_cast<int*>(&m_Type), "Directional\0Point\0Spot\0\0");
	ImGui::Checkbox("Enable", &m_isEnable);
	ImGui::ColorEdit4("Ambient", &m_Ambient.x);

	switch (m_Type)
	{
	case LIGHTTYPE_DIRECTIONAL:
		ImGui::ColorEdit4("Diffuse", &m_Diffuse.x);
		ImGui::DragFloat3("Direction", &m_Direction.x, 0.1f, -1.0f, 1.0f);
		break;
	case LIGHTTYPE_POINT:
		ImGui::ColorEdit4("Diffuse", &m_Diffuse.x);
		ImGui::DragFloat("Range", &m_Range, 0.1f, 0.0f, 10000.0f);
		break;
	case LIGHTTYPE_SPOT:
		ImGui::ColorEdit4("Diffuse", &m_Diffuse.x);
		ImGui::DragFloat("Range", &m_Range, 0.1f, 0.0f, 10000.0f);
		ImGui::DragFloat3("Direction", &m_Direction.x, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat("Inner Angle", &m_InnerAngle, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Outer Angle", &m_OuterAngle, 0.1f, 0.0f, 100.0f);
		break;
	default:
		break;
	}

	ImGui::Unindent();
}

void Light::ExportComponent()
{
	CSVExporter::ExportInt(static_cast<int>(m_Type));
	switch (m_Type)
	{
	case LIGHTTYPE_DIRECTIONAL:
		CSVExporter::ExportVector4O(m_Direction);
		CSVExporter::ExportVector4O(m_Diffuse);
		break;
	case LIGHTTYPE_POINT:
		CSVExporter::ExportFloat(m_Range);
		CSVExporter::ExportVector4O(m_Diffuse);
		break;
	case LIGHTTYPE_SPOT:
		CSVExporter::ExportFloat(m_Range);
		CSVExporter::ExportFloat(m_InnerAngle);
		CSVExporter::ExportFloat(m_OuterAngle);
		CSVExporter::ExportVector4O(m_Diffuse);
		CSVExporter::ExportVector4O(m_Direction);
		break;
	default:
		break;
	}
}
