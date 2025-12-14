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

enum class WindowType {
    
};

class NodeEditor;
class GUIWindow;

class GUI {
private:
	//std::list<ImFont*> fonts;
	static ImFont* m_pFontDefault;
	static ImFont* m_pFontObjectName;
	static ImFont* m_pCurrentFont;

    // ツールバーの高さ
    static constexpr float TOOLBAR_HEIGHT = 50.0f;


    ImGuiID m_DockSpaceID = 0;

    NodeEditor* m_pNodeEditor;



    static GUIWindow* m_GameViewWindow;
    static GUIWindow* m_EditorViewWindow;
    static GUIWindow* m_SystemMonitorWindow;
    static GUIWindow* m_ConsoleWindow;
    static GUIWindow* m_ProjectWindow;
    static GUIWindow* m_InspectorWindow;
    static GUIWindow* m_HierarchyWindow;


	/// @brief レンダーターゲットを作成します。
	void CreateRenderTarget();
	/// @brief レンダーターゲットをクリーンアップします。
	void CleanupRenderTarget();

    static void SetWindowPadding();
    static void SetItemPadding();

    // DirectX 11のレンダリングターゲットビュー
	ID3D11RenderTargetView* m_mainRenderTargetView = nullptr;

    static GUI* m_pInstance;
    GUI();
    GUI(const GUI&) = delete;
    GUI& operator=(const GUI&) = delete;

	static void DrawToolbar();  // ツールバー（再生/停止ボタン）

    static void DrawGameView();
    static void DrawSceneView();
    static void DrawSystemMonitorWindow();

    static void DrawProjectWindow();

    static void DrawInspectorWindow();

    static void DrawConsoleWindow();

    static void DrawHierarchyWindow();

public:
    static bool BeginWindow(const char* name, ImGuiWindowFlags flag = ImGuiWindowFlags_None);

    static void DrawGUI();

	void Initialize();
    void Finalize();

	void StartImGui();


	static void EndWindow();
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

