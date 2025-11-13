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

class NodeEditor;

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
    bool m_IsFirstFrameSystemMonitor = true;
    bool m_IsFirstFrameGameView = true;

    ImGuiID m_DockSpaceID = 0;

    NodeEditor* m_pNodeEditor;

	/// @brief レンダーターゲットを作成します。
	void CreateRenderTarget();
	/// @brief レンダーターゲットをクリーンアップします。
	void CleanupRenderTarget();

    void SetWindowPadding();
    void SetItemPadding();

    void Begin(const char* name, ImGuiWindowFlags flag = ImGuiWindowFlags_None);

    // DirectX 11のレンダリングターゲットビュー
	ID3D11RenderTargetView* m_mainRenderTargetView = nullptr;

    static GUI* m_pInstance;
    GUI() = default;
    GUI(const GUI&) = delete;
    GUI& operator=(const GUI&) = delete;

public:


	void Initialize();
    void Finalize();

	void StartImGui();
	void StartHierarchy();
	void StartInspector();
    void StartSceneView();
    void StartGameView();
    void StartSystemMonitor();
    void StartConsole();
	void EndWindow();
	void EndImGui();

	static void SetFontDefault();
	static void SetFontObjectName();

    void SetNodeEditorVisible();

    static GUI* GetInstance() {
        if (m_pInstance == nullptr) {
            m_pInstance = new GUI();
        }
        return m_pInstance;
    }
};

