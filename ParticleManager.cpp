#include "ParticleManager.h"

#include "Component_Transform.h"
#include "Billboard.h"
#include "Component_MeshRenderer.h"

std::map<Particle*, std::list<ParticleData>> ParticleManager::m_Particles;
ParticleManager* ParticleManager::m_pInstance = nullptr;

ParticleManager::~ParticleManager()
{
    for (auto& pair : m_Particles)
    {
        delete pair.first; // Clean up dynamically allocated particles
    }
    m_Particles.clear();
    m_pInstance = nullptr; // Reset singleton instance
}

void ParticleManager::AddParticle(Particle* particle)
{
    m_Particles[particle] = std::list<ParticleData>(); // Initialize with empty data list
}

void ParticleManager::AddParticleData(Particle* particle, const ParticleData& data)
{
    auto it = m_Particles.find(particle);
    if (it != m_Particles.end())
    {
        for (auto& list : it->second) {
            if (list.enable == false) {
                list = data;
                list.enable = true;
                return;
            }
        }

        it->second.push_back(data); // Add data to the particle's data list
        return;
    }

	AddParticle(particle); // If particle not found, add it
	AddParticleData(particle, data); // Then add the data
}

void ParticleManager::RemoveParticle(Particle* particle)
{
    auto it = m_Particles.find(particle);
    if (it != m_Particles.end())
    {
        delete it->first; // Clean up the particle
        m_Particles.erase(it); // Remove from the map
    }
}

void ParticleManager::UpdateParticles()
{
    for (auto& pair : m_Particles)
    {
        Particle* particle = pair.first;
        std::list<ParticleData>& dataList = pair.second;
		std::vector<ParticleData> toRemove; // List to hold particles to be removed
        // Update each particle's data
        for (auto& data : dataList)
        {
            // Update logic for each particle's data
            data.frameCount++; // Example update, adjust as needed
            if (data.frameCount >= data.lifeTime)
            {
				data.enable = false; // Disable particle if frame count exceeds lifetime
				toRemove.push_back(data); // Mark for removal
				continue; // Skip further updates for this particle
            }
			data.velocity += data.acceleration; // Update velocity with acceleration
			data.position += data.velocity; // Update position with velocity
			particle->UpdateParticle(&data); // Call particle's update method
        }

		// Remove particles that are marked for removal
        for (const auto& removeData : toRemove) {
            dataList.remove_if([&removeData](const ParticleData& d) {
                return &d == &removeData;
                });
        }
    }
}

void ParticleManager::DrawParticles()
{
	if (m_Particles.empty()) return;
    Object* object = new Object(false);
    object->AddComponent<Transform>();
	object->AddComponent<Billboard>();
	object->AddComponent<MeshRenderer>();
	object->GetComponent<Transform>()->SetScale(Vector4O(1.0f, 1.0f, 1.0f));
    object->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
    object->GetComponent<MeshRenderer>()->SetPixelShader("unlit");
	LIGHT light;
	light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
	light.Ambient = Vector4O(0.1f, 0.1f, 0.1f, 1.0f);
    light.Direction = Vector3O(0.2f, -1.0f, -1.0f);

    for (auto& pair : m_Particles)
    {
		object->GetComponent<Transform>()->SetScale(pair.second.begin()->size); // Set scale based on the first particle's size
		object->GetComponent<MeshRenderer>()->SetTexture(pair.second.begin()->texture);
        for (auto& data : pair.second)
        {
            if (data.enable) // Only draw enabled particles
            {
				object->GetComponent<Transform>()->SetPosition(data.position);
                
                object->Draw();
            }
		}
    }
    object->Destroy();
}
