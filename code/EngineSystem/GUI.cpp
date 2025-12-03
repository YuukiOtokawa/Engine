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


//==========================================================================
// 名前空間定義
//==========================================================================

ImFont* GUI::m_pFontDefault = nullptr;
ImFont* GUI::m_pFontObjectName = nullptr;
ImFont* GUI::m_pCurrentFont = nullptr;

GUI* GUI::m_pInstance = nullptr;

int GUI::m_GUIWidgetIDCounter = 0;

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

	//if (m_IsFirstFrame) {
	//	m_IsFirstFrame = false;

	//	ImGui::DockBuilderRemoveNode(m_DockSpaceID);
	//	ImGui::DockBuilderAddNode(m_DockSpaceID, ImGuiDockNodeFlags_DockSpace);
	//	ImGui::DockBuilderSetNodeSize(m_DockSpaceID, viewport->Size);

	//	ImGuiID dockMainID = m_DockSpaceID;
	//	// 右にInspector
	//	ImGuiID dockInspectorID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, 0.25f, nullptr, &dockMainID);
	//	// 左にHierarchy
	//	ImGuiID dockHierarchyID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, 0.25f, nullptr, &dockMainID);
	//	// 下にConsole（全幅）
	//	ImGuiID dockConsoleID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.25f, nullptr, &dockMainID);
	//	// Consoleの上にScene View
	//	ImGuiID dockSceneViewID = dockMainID;
	//	// Scene Viewの左下にSystem Monitor
	//	ImGuiID dockSystemMonitorID = ImGui::DockBuilderSplitNode(dockConsoleID, ImGuiDir_Left, 0.5f, nullptr, &dockConsoleID);

	//	ImGui::DockBuilderDockWindow("Inspector", dockInspectorID);
	//	ImGui::DockBuilderDockWindow("Hierarchy", dockHierarchyID);
	//	ImGui::DockBuilderDockWindow("Scene View", dockSceneViewID);
	//	ImGui::DockBuilderDockWindow("Game View", dockSceneViewID);
	//	ImGui::DockBuilderDockWindow("System Monitor", dockSystemMonitorID);
	//	ImGui::DockBuilderDockWindow("Console", dockConsoleID);

	//	ImGui::DockBuilderFinish(m_DockSpaceID);
	//}


	ImGui::End();

	auto camera = Editor::GetInstance()->GetActiveCamera()->GetComponent<Camera>();

	//m_pNodeEditor->Render();


	//ImGuizmo::Manipulate((float*)&camera->GetView(), (float*)&camera->GetProjection(), ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, (float*)&Editor::GetInstance()->GetActiveCamera()->GetComponent<Transform>()->GetWorldMatrix(), nullptr, nullptr);
}

void GUI::StartToolbar()
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

void GUI::StartHierarchy() {
	if (m_IsFirstFrameHierarchy) {
		m_IsFirstFrameHierarchy = false;
	}

	Begin("Hierarchy");
}

void GUI::StartInspector()
{
	if (m_IsFirstFrameInspector) {
		m_IsFirstFrameInspector = false;
	}

	Begin("Inspector");
}

void GUI::StartSceneView()
{
	if (m_IsFirstFrameSceneView) {
		m_IsFirstFrameSceneView = false;
	}

	Begin("Scene View", ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
}

void GUI::StartGameView()
{
	if (m_IsFirstFrameGameView)
		m_IsFirstFrameGameView = false;

	Begin("Game View", ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
}

void GUI::StartSystemMonitor()
{
	if (m_IsFirstFrameSystemMonitor) {
		m_IsFirstFrameSystemMonitor = false;
	}
	Begin("System Monitor");
}

void GUI::StartConsole()
{
	if (m_IsFirstFrameSystemMonitor) {
		m_IsFirstFrameSystemMonitor = false;
	}
	Begin("Console");
}

void GUI::StartProjectWindow()
{
	if (m_IsFirstFrameProjectWindow) {
		m_IsFirstFrameProjectWindow = false;
	}
	Begin("Project");
}

void GUI::EndWindow() {
	//ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f, 0.0f), ImVec2(1920.0f, 1080.0f),1);
	ImGui::End();



	//const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	//System::GetDirectXManager()->GetDeviceContext()->OMSetRenderTargets(1, &m_mainRenderTargetView, nullptr);
	//System::GetDirectXManager()->GetDeviceContext()->ClearRenderTargetView(m_mainRenderTargetView, clear_color_with_alpha);
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

void GUI::Begin(const char* name, ImGuiWindowFlags flag)
{
	SetWindowPadding();
	ImGui::Begin(name, nullptr, flag);
	SetItemPadding();
}
