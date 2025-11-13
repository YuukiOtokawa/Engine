#include "Particle.h"

#include "ParticleManager.h"
#include "Component_Transform.h"

#include "MainEngine.h"

REGISTER_COMPONENT(Particle)

void Particle::Update()
{
	m_FrameCount--;
	if (m_FrameCount <= 0) {
		ParticleData data;
		
		data.startPosition = owner->GetComponent<Transform>()->GetPosition();
		data.size = m_Size;
		data.velocity = m_Velocity;
		data.acceleration = m_Acceleration;
		data.texture = m_Texture;
		data.lifeTime = m_LifeTime;
		
		ParticleManager::AddParticleData(this, data);
		m_FrameCount = m_SpawnSpan;
	}
}

void Particle::DrawGUI()
{
	ImGui::Separator();
	ImGui::DragFloat3("Velocity", &m_Velocity.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Acceleration", &m_Acceleration.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragInt("Spawn Span", &m_SpawnSpan, 1.0f, 0.0f, 100.0f);
	ImGui::DragInt("Life Time", &m_LifeTime, 1.0f, 0.0f, 100.0f);
	ImGui::InputInt("Frame Count", &m_FrameCount);
	ImGui::Image((ImTextureID)MainEngine::GetInstance()->GetRenderCore()->GetTexture(m_Texture), ImVec2(300, 300));
	ImGui::Unindent();
}

void Particle::ExportComponent(YAML::Emitter& out)
{
	out << YAML::Key << "velocity" << YAML::Value << YAML::Flow << YAML::BeginSeq
		<< m_Velocity.x << m_Velocity.y << m_Velocity.z << m_Velocity.w << YAML::EndSeq;

	out << YAML::Key << "acceleration" << YAML::Value << YAML::Flow << YAML::BeginSeq
		<< m_Acceleration.x << m_Acceleration.y << m_Acceleration.z << m_Acceleration.w << YAML::EndSeq;

	out << YAML::Key << "spawnSpan" << YAML::Value << m_SpawnSpan;
	out << YAML::Key << "lifeTime" << YAML::Value << m_LifeTime;
	out << YAML::Key << "texture" << YAML::Value << m_Texture;
}

void Particle::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::ParticleTag);
	owner->SetLayer(GameObjectLayer::SystemLayer);
}