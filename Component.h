#pragma once

#include "FrameWork.h"
#include "Object.h"

using namespace ComponentTag;

class Component
{
protected:
	Object* owner = nullptr;
	bool isActive = true;
	Tag tag;

public:
	Component() = default;
	~Component() = default;
	
	virtual void SetOwner(Object* obj) { 
		owner = obj;
		InitializeTag();
	}

	virtual void InitializeTag() = 0;

	virtual void Update() = 0;
	virtual void Draw() {}
	virtual void DrawGUI() {}
};

