#pragma once
#include "Component.h"

struct ParticleData {
    Vector4O position;
    Vector4O velocity;
    float lifetime;
    float age;
};

class Particle :
    public Component
{
private:
    enum ParticleStatus {
        PARTICLE_WAIT,
        PARTICLE_MOVE,
        PARTICLE_LAUNCH,
        PARTICLE_DESTROY
    };

    Vector4O m_Velocity;
    Vector4O m_Acceleration;

public:
    Particle() {
        m_ClassID = CID_Particle;
    }

    void UpdateComponent() override {

    }
};

