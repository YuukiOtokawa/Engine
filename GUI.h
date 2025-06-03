#pragma once

#include "d3d11.h"

#include <list>
#include "imgui.h"

class GUI {
private:
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

	static void SetFontDefault();
	static void SetFontObjectName();
};

