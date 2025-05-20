#pragma once

#include "Vector4O.h"
#include "d3d11.h"

class GUI {
private:
	bool checkbox = false; 
	float sliderfloat = 0.0f;
	float Float[4] = {};
	int count = 0;

	void CreateRenderTarget();
	void CleanupRenderTarget();

	ID3D11RenderTargetView* m_mainRenderTargetView = nullptr;
public:
	void Start();
	void End();
	void Draw();
};

