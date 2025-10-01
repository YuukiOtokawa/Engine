#pragma once
#include "MeshField.h"
class WaveMeshField :
    public MeshField
{
    enum class WaveType {
        Linear,
        Circular,
    };
    float m_WaveHeight = 1.0f;
    float m_WaveLength = 1.0f;
    float m_WaveSpeed = 1.0f;
    float m_Time = 0.0f;

    WaveType m_WaveType = WaveType::Circular;

    void WaveLinear();
    void WaveCircular();
public:
    DECLARE_COMPONENT(WaveMeshField)
    WaveMeshField();
    virtual ~WaveMeshField();
    void Update() override;
    void DrawGUI() override;
    void InitializeTag() override;
};

