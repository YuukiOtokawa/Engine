#pragma once

#include <map>
#include <list>
#include "Particle.h"

class ParticleManager
{
private:
    std::map<Particle*, std::list<ParticleData>> m_Particles; // Map of particles and their data

    ParticleManager() = default; // Private constructor for singleton pattern
    ParticleManager(const ParticleManager&) = delete; // Disable copy constructor
    ParticleManager& operator=(const ParticleManager&) = delete; // Disable copy assignment

    static ParticleManager* m_pInstance; // Singleton instance

public:
    static ParticleManager* GetInstance()
    {
        if (m_pInstance == nullptr)
        {
            m_pInstance = new ParticleManager();
        }
        return m_pInstance;
    }

    ~ParticleManager()
    {
        for (auto& pair : m_Particles)
        {
            delete pair.first; // Clean up dynamically allocated particles
        }
        m_Particles.clear();
        m_pInstance = nullptr; // Reset singleton instance
    }

    void AddParticle(Particle* particle)
    {
        m_Particles[particle] = std::list<ParticleData>(); // Initialize with empty data list
    }

    void AddParticleData(Particle* particle, const ParticleData& data)
    {
        auto it = m_Particles.find(particle);
        if (it != m_Particles.end())
        {
            it->second.push_back(data); // Add data to the particle's data list
        }
    }

    void RemoveParticle(Particle* particle)
    {
        auto it = m_Particles.find(particle);
        if (it != m_Particles.end())
        {
            delete it->first; // Clean up the particle
            m_Particles.erase(it); // Remove from the map
        }
    }

    void UpdateParticles()
    {
        for (auto& pair : m_Particles)
        {
            Particle* particle = pair.first;
            std::list<ParticleData>& dataList = pair.second;
            // Update each particle's data
            for (auto& data : dataList)
            {
                // Update logic for each particle's data
                data.age += 0.1f; // Example update, adjust as needed
                if (data.age >= data.lifetime)
                {
                    // Handle particle destruction logic
                    //dataList.remove(data); // Remove expired particles
                }
            }
            particle->UpdateComponent(); // Call the particle's update method
        }
    }

    void DrawParticles()
    {
        for (auto& pair : m_Particles)
        {
            Particle* particle = pair.first;
            particle->Draw(); // Call the particle's draw method
        }
    }
};

