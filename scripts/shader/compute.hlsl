#include "common.hlsli"

StructuredBuffer<PARTICLE> particle : register(t0);
RWStructuredBuffer<PARTICLE> BufOut : register(u0);

[numthreads(512,1,1)]

void com(in uint3 dispatch : SV_DispatchThreadID) {
    int index = dispatch.x;
    BufOut[index].Position = particle[index].Position + particle[index].Velocity;
    BufOut[index].Velocity = particle[index].Velocity;
    BufOut[index].Acceleration = particle[index].Acceleration;
    BufOut[index].Parameter = particle[index].Parameter - 1.0f;
    
    return;
}