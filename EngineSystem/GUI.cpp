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

}
void GUI::StartHierarchy() {
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(400, 1009));

	ImGui::Begin("Hierarchy");
}

void GUI::StartInspector()
{
	SetNextWindowPos(ImVec2(1920 - 400, 0));
	SetNextWindowSize(ImVec2(400, 1009));

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
	MainEngine::GetInstance()->GetRenderer()->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	MainEngine::GetInstance()->GetRenderer()->GetDevice()->CreateRenderTargetView(pBackBuffer, nullptr, &m_mainRenderTargetView);
	pBackBuffer->Release();
}

void GUI::CleanupRenderTarget() {
	if (m_mainRenderTargetView) { m_mainRenderTargetView->Release(); m_mainRenderTargetView = nullptr; }
}
