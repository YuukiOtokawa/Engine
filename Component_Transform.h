#pragma once
#include "Component.h"

#include "Vector4O.h"

class Transform :
    public Component
{
private:
	Vector4O m_Position = Vector4O(0.0f,0.0f,0.0f);
	Vector4O m_Rotation = Vector4O(0.0f, 0.0f, 0.0f);
	Vector4O m_Scale = Vector4O(1.0f, 1.0f, 1.0f);

public:

	void Update() override;
	void Draw() override;

	void InitializeTag() override;

	void SetPosition(Vector4O position) { m_Position = position; }
	void SetRotation(Vector4O rotation) { m_Rotation = rotation; }
	void SetScale(Vector4O scale) { m_Scale = scale; }

	Vector4O GetPosition() { return m_Position; }
	Vector4O GetRotation() { return m_Rotation; }
	Vector4O GetScale() { return m_Scale; }
};

