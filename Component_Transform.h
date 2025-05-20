#pragma once
#include "Component.h"

#include "Vector4O.h"

class Component_Transform :
    public Component
{
private:
	Vector4O position = Vector4O(0.0f,0.0f,0.0f);
	Vector4O rotation = Vector4O(0.0f, 0.0f, 0.0f);
	Vector4O scale = Vector4O(1.0f, 1.0f, 1.0f);

public:

	void Update() override;
};

