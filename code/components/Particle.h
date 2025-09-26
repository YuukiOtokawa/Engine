#pragma once
#include "Component.h"

struct ParticleData {
    Vector4O position;
    Vector4O startPosition; // Start position for the particle
    Vector4O size;
    Vector4O velocity;
    Vector4O acceleration;
    int texture;
    int lifeTime;
    int frameCount = 0;
    bool enable;
};

class Particle :
    public Component
{
private:
    int m_SpawnSpan = 0;
    int m_FrameCount = 0;
    int m_LifeTime = 0;
    Vector4O m_Size = Vector4O(1.0f, 1.0f, 1.0f); // Size of the particle
    Vector4O m_Velocity;
    Vector4O m_Acceleration;
    int m_Texture = -1;

    bool m_isStarted;
public:
    DECLARE_COMPONENT(Particle)
    Particle() {
        m_ClassID = CID_Particle;
    }

    void Update() override;

    void DrawGUI() override;

    void ExportComponent() override;

    void SetSize(Vector4O size) {
        m_Size = size;
    }
    void SetSpawnSpan(int span) {
        m_SpawnSpan = span;
    }
    void SetVelocity(const Vector4O& velocity) {
        m_Velocity = velocity;
    }
    void SetAcceleration(const Vector4O& acceleration) {
        m_Acceleration = acceleration;
    }
    void SetLifeTime(int lifeTime) {
        m_LifeTime = lifeTime;
    }
    void SetTexture(int texture) {
        m_Texture = texture;
    }

    void InitializeTag();

    virtual void UpdateParticle(ParticleData* data) {}

};

