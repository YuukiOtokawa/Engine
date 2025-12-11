// ========================================================
//
// imguiクラス[GUI.cpp]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "GUI.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "MainEngine.h"

#include "Component_Transform.h"

#include "ImGuizmo.h"

#include "GUIWindow.h"

#include "ProjectWindow.h"

//==========================================================================
// 名前空間定義
//==========================================================================

ImFont* GUI::m_pFontDefault = nullptr;
ImFont* GUI::m_pFontObjectName = nullptr;
ImFont* GUI::m_pCurrentFont = nullptr;

GUI* GUI::m_pInstance = nullptr;

GUIWindow* GUI::m_GameViewWindow = nullptr;
GUIWindow* GUI::m_EditorViewWindow = nullptr;
GUIWindow* GUI::m_SystemMonitorWindow = nullptr;
GUIWindow* GUI::m_ConsoleWindow = nullptr;
GUIWindow* GUI::m_ProjectWindow = nullptr;
GUIWindow* GUI::m_InspectorWindow = nullptr;
GUIWindow* GUI::m_HierarchyWindow = nullptr;

//==========================================================================
// メンバ関数
//==========================================================================

void GUI::Initialize() {
	auto io = ImGui::GetIO();


	//フォント設定
	// Source Han Code JP ウェイト: 0=ExtraLight, 1=Light, 2=Normal, 3=Medium, 4=Bold, 5=Heavy
	ImFontConfig fontConfig;
	fontConfig.FontNo = 4; // Normal

	// 通常のフォントを設定（日本語対応）
	m_pFontDefault = io.Fonts->AddFontFromFileTTF("asset\\font\\SourceHanCodeJP.ttc", 16.0f, &fontConfig);

	// オブジェクト名用のフォントを設定（日本語対応）
	m_pFontObjectName = io.Fonts->AddFontFromFileTTF("asset\\font\\SourceHanCodeJP.ttc", 24.0f, &fontConfig);

	io.Fonts->Build();

	dynamic_cast<ProjectWindow*>(m_ProjectWindow)->Initialize("scripts", "OtokawaEngine.vcxproj");

}

void GUI::Finalize()
{
}

void GUI::StartImGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();

	ImGui::PushFont(m_pFontDefault);
	m_pCurrentFont = m_pFontDefault;

	ImGuiViewport* viewport = ImGui::GetMainViewport();

	// ドッキングスペースはツールバーの下から開始
	ImVec2 dockspacePos = ImVec2(viewport->Pos.x, viewport->Pos.y + TOOLBAR_HEIGHT);
	ImVec2 dockspaceSize = ImVec2(viewport->Size.x, viewport->Size.y - TOOLBAR_HEIGHT);

	ImGui::SetNextWindowPos(dockspacePos);
	ImGui::SetNextWindowSize(dockspaceSize);

	ImGui::Begin("DockSpace", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);

	m_DockSpaceID = ImGui::GetID("DockSpace");

	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGui::DockSpace(m_DockSpaceID, ImVec2(0.0f, 0.0f), dockspace_flags);



	ImGui::End();

	auto camera = Editor::GetInstance()->GetActiveCamera()->GetComponent<Camera>();

}

void GUI::DrawGUI()
{
	// ツールバー（再生/停止ボタン）
	DrawToolbar();

	DrawSceneView();
	DrawGameView();

	//システムモニターウィンドウ
	DrawSystemMonitorWindow();

	//プロジェクトウィンドウ
	DrawProjectWindow();

	//インスペクターウィンドウ
	DrawInspectorWindow();

	//コンソールウィンドウ
	DrawConsoleWindow();

	//ヒエラルキウィンドウ
	DrawHierarchyWindow();
}

void GUI::DrawToolbar()
{
	// ツールバーウィンドウ
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, TOOLBAR_HEIGHT));
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
	                                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
	                                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
	ImGui::Begin("##Toolbar", nullptr, window_flags);

	Editor* editor = Editor::GetInstance();

	// 再生ボタンと停止ボタンを中央に配置
	float windowWidth = ImGui::GetWindowWidth();
	float buttonWidth = 100.0f;
	float spacing = 10.0f;
	float totalWidth = buttonWidth * 2 + spacing;
	ImGui::SetCursorPosX((windowWidth - totalWidth) * 0.5f);

	if (editor->IsPlaying()) {
		// 再生中は停止ボタンのみ表示
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
		if (ImGui::Button("Stop", ImVec2(buttonWidth, 30))) {
			editor->Stop();
		}
		ImGui::PopStyleColor(3);
	} else {
		// 停止中は再生ボタンのみ表示
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.9f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.7f, 0.1f, 1.0f));
		if (ImGui::Button("Play", ImVec2(buttonWidth, 30))) {
			editor->Play();
		}
		ImGui::PopStyleColor(3);
	}

	ImGui::End();
	ImGui::PopStyleVar();
}


void GUI::DrawGameView()
{
	m_GameViewWindow->Render();
}

void GUI::DrawSceneView()
{
	m_EditorViewWindow->Render();
	
}

void GUI::DrawSystemMonitorWindow()
{
	m_SystemMonitorWindow->Render();
}

void GUI::DrawProjectWindow()
{
	m_ProjectWindow->Render();
}

void GUI::DrawInspectorWindow()
{
	m_InspectorWindow->Render();
}

void GUI::DrawConsoleWindow()
{
	m_ConsoleWindow->Render();
}

void GUI::DrawHierarchyWindow()
{
	m_HierarchyWindow->Render();
}

void GUI::EndWindow() {
	ImGui::End();
}

void GUI::EndImGui()
{
	if (m_pCurrentFont != nullptr) {
		ImGui::PopFont();
		m_pCurrentFont = nullptr;
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void GUI::SetFontDefault()
{
	ImGui::PopFont();
	ImGui::PushFont(m_pFontDefault);
	m_pCurrentFont = m_pFontDefault;
}

void GUI::SetFontObjectName()
{
	ImGui::PopFont();
	ImGui::PushFont(m_pFontObjectName);
	m_pCurrentFont = m_pFontObjectName;
}

void GUI::SetNodeEditorVisible() {
}

void GUI::CreateRenderTarget() {
	ID3D11Texture2D* pBackBuffer;
	MainEngine::GetInstance()->GetRenderCore()->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	MainEngine::GetInstance()->GetRenderCore()->GetDevice()->CreateRenderTargetView(pBackBuffer, nullptr, &m_mainRenderTargetView);
	pBackBuffer->Release();
}

void GUI::CleanupRenderTarget() {
	if (m_mainRenderTargetView) { m_mainRenderTargetView->Release(); m_mainRenderTargetView = nullptr; }
}

void GUI::SetWindowPadding()
{
	ImGui::GetStyle().WindowPadding = ImVec2(8.0f, 8.0f);
}

void GUI::SetItemPadding()
{
	ImGui::GetStyle().WindowPadding = ImVec2(0.0f, 0.0f);
}

bool GUI::BeginWindow(const char* name, ImGuiWindowFlags flag)
{
	SetWindowPadding();
	bool isOpen;
	ImGui::Begin(name, &isOpen, flag);
	SetItemPadding();

	return isOpen;
}

#include "GameViewWindow.h"
#include "EditorViewWindow.h"
#include "SystemMonitorWindow.h"
#include "ConsoleWindow.h"
#include "ProjectWindow.h"
#include "InspectorWindow.h"
#include "HierarchyWindow.h"

GUI::GUI()
{
	m_GameViewWindow = new GameViewWindow();
	m_EditorViewWindow = new EditorViewWindow();
	m_SystemMonitorWindow = new SystemMonitorWindow();
	m_ConsoleWindow = new ConsoleWindow();
	m_ProjectWindow = new ProjectWindow();
	m_InspectorWindow = new InspectorWindow();
	m_HierarchyWindow = new HierarchyWindow();
}

