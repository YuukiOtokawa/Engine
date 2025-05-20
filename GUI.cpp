#include "GUI.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui.h"

#include "MainEngine.h"

using namespace ImGui;
void GUI::Start() {
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	NewFrame();

	SetNextWindowPos(ImVec2(0, 0));
	SetNextWindowSize(ImVec2(500, 1009));
	SetNextWindowSizeConstraints(ImVec2(250, 0), ImVec2(500, 1080));

	Begin("my Window");
}
void GUI::End() {
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f, 0.0f), ImVec2(1920.0f, 1080.0f),1);
	ImGui::End();



	Render();
	//const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	//System::GetDirectXManager()->GetDeviceContext()->OMSetRenderTargets(1, &m_mainRenderTargetView, nullptr);
	//System::GetDirectXManager()->GetDeviceContext()->ClearRenderTargetView(m_mainRenderTargetView, clear_color_with_alpha);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
void GUI::Draw() {

	bool SwapChainOccluded = false;

	auto windowSize = MainEngine::GetInstance()->GetRenderer()->GetClientSize();

	//if (windowSize.x != 0 && windowSize.y != 0) {
	//	CleanupRenderTarget();
	//	MainEngine::GetInstance()->GetRenderer()->GetSwapChain()->ResizeBuffers(0, windowSize.x, windowSize.y, DXGI_FORMAT_UNKNOWN, 0);
	//	CreateRenderTarget();
	//}
	//SetNextWindowSize(ImVec2(windowSize.x * 0.2f, windowSize.y * 0.2f));


	Text("Hello World!");
	Checkbox("checkbox",&checkbox);
	SliderFloat("sliderfloat", &sliderfloat, 0.0f, 1.0f);
	InputFloat("inputfloat", &sliderfloat);
	SliderFloat4("SliderFloat4", Float, 0.0f, 1.0f);
	InputFloat4("InputFloat4", Float);
	Button("count", ImVec2(100,100));
	



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
