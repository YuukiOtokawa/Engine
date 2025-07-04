#pragma once
#include "Component.h"

struct ParticleData {
    Vector4O position;
    Vector4O size;
    Vector4O velocity;
    Vector4O acceleration;
    ID3D11ShaderResourceView* texture;
    int lifeTime;
    int frameCount = 0;
    bool enable;
};

class Particle :
    public Component
{
private:
    int m_SpawnSpan;
    int m_FrameCount = 0;
    int m_LifeTime;
    Vector4O m_Size = Vector4O(1.0f, 1.0f, 1.0f); // Size of the particle
    Vector4O m_Velocity;
    Vector4O m_Acceleration;
    ID3D11ShaderResourceView* m_Texture;

public:
    Particle() {
        m_ClassID = CID_Particle;
    }

    void UpdateComponent() override;

    void DrawGUI() override;

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
    void SetTexture(ID3D11ShaderResourceView* texture) {
        m_Texture = texture;
    }

    void InitializeTag() {
        owner->SetTag(GameObjectTagLayer::ParticleTag);
    }
};

