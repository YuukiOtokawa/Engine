#pragma once

#include <map>
#include <list>
#include "Particle.h"

class ParticleManager
{
private:
    static std::map<Particle*, std::list<ParticleData>> m_Particles; // Map of particles and their data

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

    ~ParticleManager();

    static void AddParticle(Particle* particle);

    static void AddParticleData(Particle* particle, const ParticleData& data);

    static void RemoveParticle(Particle* particle);

    void UpdateParticles();

    void DrawParticles();
};

