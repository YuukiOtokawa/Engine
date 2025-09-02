// ========================================================
//
// imguiクラス[GUI.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

#include "d3d11.h"

//==========================================================================
// ヘッダーインクルード
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
	/// @brief レンダーターゲットをクリーンアップします。
	void CleanupRenderTarget();

    // DirectX 11のレンダリングターゲットビュー
	ID3D11RenderTargetView* m_mainRenderTargetView = nullptr;
public:

	void Initialize();
	void StartImGui();
	void StartHierarchy();
	void StartInspector();
	void EndWindow();
	void EndImGui();
    //void Begin(const char* name, ImVec2 pos, ImVec2 size);

	static void SetFontDefault();
	static void SetFontObjectName();
};

