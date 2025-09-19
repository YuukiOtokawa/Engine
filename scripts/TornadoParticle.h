#pragma once
#include "Particle.h"
struct TornadoParticle :
    public Particle
{
public:
    void UpdateParticle(ParticleData* data) override {
        data->position = data->startPosition + Vector4O(
            sinf(data->frameCount * 0.1f) * 1.75f * (1.0f + data->frameCount * 0.01f),
            data->frameCount * 0.02f,
            cosf(data->frameCount * 0.1f) * 1.75f * (1.0f + data->frameCount * 0.01f),
            0.0f);
    }
};
