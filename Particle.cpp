#include "Particle.h"

#include "ParticleManager.h"
#include "Component_Transform.h"

void Particle::UpdateComponent()
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
	ImGui::Indent();
	ImGui::Text("Particle Component");
	ImGui::Separator();
	ImGui::DragFloat3("Velocity", &m_Velocity.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Acceleration", &m_Acceleration.x, 0.1f, -100.0f, 100.0f);
	ImGui::DragInt("Spawn Span", &m_SpawnSpan, 1.0f, 0.0f, 100.0f);
	ImGui::DragInt("Life Time", &m_LifeTime, 1.0f, 0.0f, 100.0f);
	ImGui::InputInt("Frame Count", &m_FrameCount);
	ImGui::Image((ImTextureID)m_Texture, ImVec2(300, 300));
	ImGui::Unindent();
}
