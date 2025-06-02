#pragma once
#include "Component.h"

#include "Vector4O.h"

#include "FrameWork.h"

class Camera :
    public Component
{
private:
    Vector4O m_LastPosition = Vector4O::Zero();
    Vector4O m_Target;
    Vector4O m_Angle;

    float m_Fov = 90.0f;

    XMMATRIX m_Projection;
	XMMATRIX m_View;

	float m_Near = 0.1f;
    float m_Far = 1000.0f;

	Vector4O m_Up = Vector4O::Up();

public:
    Camera();

	XMMATRIX GetProjection() const { return m_Projection; }
	XMMATRIX GetView() const { return m_View; }
	Vector4O GetAngle() const { return m_Angle; }

    void Update() override;
	void Draw() override;
	void DrawGUI() override;
    void InitializeTag() override;

	void SetTarget(Vector4O target) { m_Target = target; }
	void SetAngle(Vector4O angle) { m_Angle = angle; }
	void SetFov(float fov) { m_Fov = fov; }

	Vector4O GetTarget() const { return m_Target; }
};

