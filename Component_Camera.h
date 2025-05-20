#pragma once
#include "Component.h"

#include "Vector4O.h"

#include "FrameWork.h"

class Component_Camera :
    public Component
{
private:
    Vector4O m_LastPosition = Vector4O::Zero();
    Vector4O m_Target;
    Vector4O m_Angle;

    float m_Fov = 90.0f;

    XMMATRIX m_ProjectionMatrix;
	XMMATRIX m_ViewMatrix;

	Vector4O m_Up = Vector4O::Up();

public:
    Component_Camera() = default;

	XMMATRIX GetProjectionMatrix() const { return m_ProjectionMatrix; }
	XMMATRIX GetViewMatrix() const { return m_ViewMatrix; }
	Vector4O GetAngle() const { return m_Angle; }

    void Update() override;
};

