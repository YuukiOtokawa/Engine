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

void Light::Draw(bool castShadow)
{
	auto renderer = MainEngine::GetInstance()->GetRenderCore();
	
	auto pos = owner->GetComponent<Transform>()->GetPosition();
	switch (m_Type)
	{
	case LIGHTTYPE_DIRECTIONAL:
	{
		if (m_Direction.Length() == 0.0f) break;
		// DirectionalLightの場合、ライトの向きを使ってView行列を作成
		XMVECTOR lightPos = XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
		XMVECTOR lightDir = XMVectorSet(m_Direction.x, m_Direction.y, m_Direction.z, 0.0f);
		XMVECTOR lightTarget = XMVectorAdd(lightPos, lightDir); // ライト位置 + 向き = 注視点
		XMVECTOR upVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		

		m_DirectionalLight = {
			m_isEnable,
			{},
			m_Diffuse,
			m_Direction,
			XMMatrixLookAtLH(lightPos, lightTarget, upVec),
			// DirectionalLightは平行光源なので正射影を使用
			// サイズを大きくしてシーン全体をカバー
			XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), renderer->GetClientSize().x/renderer->GetClientSize().y, 1.0f, 1000.0f)
		};
	}
	break;
	case LIGHTTYPE_POINT:
		m_PointLight = {
			m_isEnable,
			{},
			m_Range,
			m_Diffuse,
			owner->GetComponent<Transform>()->GetPosition(),
			XMMatrixLookAtLH(XMVectorSet(pos.x,pos.y,pos.z,0.0f),XMVectorSet(0.0f,pos.y - 1.0f,0.0f,0.0f),XMVectorSet(0.0f,0.1f,0.0f,0.0f)),
			XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)renderer->GetClientSize().x / renderer->GetClientSize().y, 1.0f, 1000.0f)
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
			m_Direction,
			XMMatrixLookAtLH(XMVectorSet(pos.x,pos.y,pos.z,0.0f),XMVectorSet(0.0f,pos.y - 1.0f,0.0f,0.0f),XMVectorSet(0.0f,0.1f,0.0f,0.0f)),
			XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)renderer->GetClientSize().x / renderer->GetClientSize().y, 1.0f, 1000.0f)
		};
		break;
	default:
		break;
	}
}

void Light::DrawGeneralLight(bool castShadow)
{
	auto renderer = MainEngine::GetInstance()->GetRenderCore();
	LIGHT_BUFFER lightBuffer = {};
	lightBuffer.ambient = m_Ambient;
	lightBuffer.directionalLight = m_DirectionalLight;
	lightBuffer.pointLight = m_PointLight;
	lightBuffer.spotLight = m_SpotLight;

	if (castShadow) {
		renderer->SetViewMatrix(lightBuffer.directionalLight.view);
		renderer->SetProjectionMatrix(lightBuffer.directionalLight.projection);
	}
	lightBuffer.directionalLight.view = XMMatrixTranspose(lightBuffer.directionalLight.view);
	lightBuffer.directionalLight.projection = XMMatrixTranspose(lightBuffer.directionalLight.projection);
	lightBuffer.pointLight.view = XMMatrixTranspose(lightBuffer.pointLight.view);
	lightBuffer.pointLight.projection = XMMatrixTranspose(lightBuffer.pointLight.projection);
	lightBuffer.spotLight.view = XMMatrixTranspose(lightBuffer.spotLight.view);
	lightBuffer.spotLight.projection = XMMatrixTranspose(lightBuffer.spotLight.projection);

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

void Light::ExportComponent(YAML::Emitter& out)
{
	out << YAML::Key << "type" << YAML::Value << static_cast<int>(m_Type);
	switch (m_Type)
	{
	case LIGHTTYPE_DIRECTIONAL:
	{
		out << YAML::Key << "direction" << YAML::Value << YAML::Flow << YAML::BeginSeq
			<< m_Direction.x << m_Direction.y << m_Direction.z << m_Direction.w << YAML::EndSeq;

		out << YAML::Key << "diffuse" << YAML::Value << YAML::Flow << YAML::BeginSeq
			<< m_Diffuse.x << m_Diffuse.y << m_Diffuse.z << m_Diffuse.w << YAML::EndSeq;
		break;
	}
	case LIGHTTYPE_POINT:
	{
		out << YAML::Key << "range" << YAML::Value << m_Range;

		out << YAML::Key << "diffuse" << YAML::Value << YAML::Flow << YAML::BeginSeq
			<< m_Diffuse.x << m_Diffuse.y << m_Diffuse.z << m_Diffuse.w << YAML::EndSeq;
		break;
	}
	case LIGHTTYPE_SPOT:
	{
		out << YAML::Key << "range" << YAML::Value << m_Range;
		out << YAML::Key << "innerAngle" << YAML::Value << m_InnerAngle;
		out << YAML::Key << "outerAngle" << YAML::Value << m_OuterAngle;

		out << YAML::Key << "diffuse" << YAML::Value << YAML::Flow << YAML::BeginSeq
			<< m_Diffuse.x << m_Diffuse.y << m_Diffuse.z << m_Diffuse.w << YAML::EndSeq;

		out << YAML::Key << "direction" << YAML::Value << YAML::Flow << YAML::BeginSeq
			<< m_Direction.x << m_Direction.y << m_Direction.z << m_Direction.w << YAML::EndSeq;
		break;
	}
	default:
		break;
	}
}
