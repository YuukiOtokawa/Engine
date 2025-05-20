#pragma once

#include "Object.h"

class Object;
class Component
{
protected:
	Object* owner = nullptr;
	bool isActive = true;
	ComponentTag tag;

public:
	void SetOwner(Object* obj) { owner = obj; }

	virtual void Update() = 0;
	virtual void Draw() {}
};

