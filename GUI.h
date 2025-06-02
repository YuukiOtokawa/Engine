#pragma once

#include "Vector4O.h"
#include "d3d11.h"

#include <list>

class GUI {
private:
	bool checkbox = false; 
	float sliderfloat = 0.0f;
	float Float[4] = {};
	int count = 0;

	//std::list<ImFont*> fonts;
	static ImFont* m_pFontDefault;
	static ImFont* m_pFontObjectName;
	static ImFont* m_pCurrentFont;

	void CreateRenderTarget();
	void CleanupRenderTarget();

	ID3D11RenderTargetView* m_mainRenderTargetView = nullptr;
public:
	void Initialize();
	void StartImGui();
	void StartHierarchy();
	void StartInspector();
	void EndWindow();
	void EndImGui();
	void Draw();

	static void SetFontDefault();
	static void SetFontObjectName();
};

