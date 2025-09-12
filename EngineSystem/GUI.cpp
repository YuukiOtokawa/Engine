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

#include "imgui_internal.h"

#include "MainEngine.h"

//==========================================================================
// 名前空間定義
//==========================================================================

using namespace ImGui;

ImFont* GUI::m_pFontDefault = nullptr;
ImFont* GUI::m_pFontObjectName = nullptr;
ImFont* GUI::m_pCurrentFont = nullptr;

//==========================================================================
// メンバ関数
//==========================================================================

void GUI::Initialize() {
	auto io = ImGui::GetIO();


	//フォント設定
	// 通常のフォントを設定
	m_pFontDefault = io.Fonts->AddFontFromFileTTF("asset\\font\\CascadiaMono.ttf", 16.0f);
	// オブジェクト名用のフォントを設定
	m_pFontObjectName = io.Fonts->AddFontFromFileTTF("asset\\font\\CascadiaMono.ttf", 24.0f);

	io.Fonts->Build();

}

void GUI::StartImGui()
{
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	NewFrame();


	ImGui::PushFont(m_pFontDefault);
	m_pCurrentFont = m_pFontDefault;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	ImGui::Begin("DockSpace", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);

	m_DockSpaceID = ImGui::GetID("DockSpace");

	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGui::DockSpace(m_DockSpaceID, ImVec2(0.0f, 0.0f), dockspace_flags);

	if (m_IsFirstFrame) {
		m_IsFirstFrame = false;

		ImGui::DockBuilderRemoveNode(m_DockSpaceID);
		ImGui::DockBuilderAddNode(m_DockSpaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(m_DockSpaceID, viewport->Size);

		ImGuiID dockMainID = m_DockSpaceID;
		ImGuiID dockRightID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, 0.25f, nullptr, &dockMainID);
		ImGuiID dockLeftID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, 0.25f, nullptr, &dockMainID);

		ImGui::DockBuilderDockWindow("Inspector", dockRightID);
		ImGui::DockBuilderDockWindow("Hierarchy", dockLeftID);

		ImGui::DockBuilderFinish(m_DockSpaceID);
	}

	ImGui::End();

}
void GUI::StartHierarchy() {
	if (m_IsFirstFrameHierarchy) {
		m_IsFirstFrameHierarchy = false;
	}

	ImGui::Begin("Hierarchy");
}

void GUI::StartInspector()
{
	if (m_IsFirstFrameInspector) {
		m_IsFirstFrameInspector = false;
	}

	Begin("Inspector");
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

	Render();
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

void GUI::CreateRenderTarget() {
	ID3D11Texture2D* pBackBuffer;
	MainEngine::GetInstance()->GetRenderCore()->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	MainEngine::GetInstance()->GetRenderCore()->GetDevice()->CreateRenderTargetView(pBackBuffer, nullptr, &m_mainRenderTargetView);
	pBackBuffer->Release();
}

void GUI::CleanupRenderTarget() {
	if (m_mainRenderTargetView) { m_mainRenderTargetView->Release(); m_mainRenderTargetView = nullptr; }
}
