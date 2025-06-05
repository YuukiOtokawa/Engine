// ========================================================
//
// imgui�`��N���X[GUI.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

#include "d3d11.h"

//==========================================================================
// �N���X��`
//==========================================================================

#include <list>
#include "imgui.h"

class GUI {
private:
	//std::list<ImFont*> fonts;
	static ImFont* m_pFontDefault;
	static ImFont* m_pFontObjectName;
	static ImFont* m_pCurrentFont;

	/// @brief レンダーターゲットを作成します。
	void CreateRenderTarget();
	/// @brief レンダーターゲットのクリーンアップ処理を行います。
	void CleanupRenderTarget();

	// imgui用レンダーターゲット
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

