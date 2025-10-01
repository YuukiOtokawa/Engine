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
#include "imgui_internal.h"

//#include "ImGuizmo.h"

class GUI {
private:
	//std::list<ImFont*> fonts;
	static ImFont* m_pFontDefault;
	static ImFont* m_pFontObjectName;
	static ImFont* m_pCurrentFont;

    bool m_IsFirstFrame = true;

    bool m_IsFirstFrameHierarchy = true;
    bool m_IsFirstFrameInspector = true;
    bool m_IsFirstFrameSceneView = true;

    ImGuiID m_DockSpaceID = 0;

	/// @brief レンダーターゲットを作成します。
	void CreateRenderTarget();
	/// @brief レンダーターゲットをクリーンアップします。
	void CleanupRenderTarget();

    void SetWindowPadding();
    void SetItemPadding();

    void Begin(const char* name);

    // DirectX 11のレンダリングターゲットビュー
	ID3D11RenderTargetView* m_mainRenderTargetView = nullptr;
public:

	void Initialize();
	void StartImGui();
	void StartHierarchy();
	void StartInspector();
    void StartSceneView();
	void EndWindow();
	void EndImGui();


	static void SetFontDefault();
	static void SetFontObjectName();
};

