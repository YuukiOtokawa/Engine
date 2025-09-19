#include "PostProcessTexture.h"

#include "MainEngine.h"
#include "Editor.h"
#include "Component_Transform.h"
#include "PostProcessRenderer.h"

PostProcessTexture::PostProcessTexture()
{
	for (int i = 0; i < 3; i++) {
		m_pPostProcessTexture[i] = new Texture;
		m_pPostProcessTexture[i]->shader_resource_view = nullptr;
		m_pPostProcessTexture[i]->width = m_Resolusion.x;
		m_pPostProcessTexture[i]->height = m_Resolusion.y;
		m_pPostProcessTexture[i]->toExport = false;
	}
}

void PostProcessTexture::UpdateComponent()
{
	//m_pCamera = Editor::GetInstance()->GetActiveCamera();

	//if (!m_IsTextureAttached) {
	//	auto meshRenderer = owner->GetComponent<MeshRenderer>();
	//	if (meshRenderer) {
	//		meshRenderer->GetMaterial()->SetTexture(m_TextureFID);
	//		m_IsTextureAttached = true;
	//	}
	//}

	//auto scale = owner->GetComponent<Transform>()->GetScale();
	//m_Resolusion = Vector2O(scale.x, scale.y);

	//// 解像度が変わったらレンダーターゲットを再作成

	//if (m_Resolusion == m_LastResolusion) return;

	//if (m_pRenderTargetView) {
	//	m_pRenderTargetView->Release();
	//	m_pRenderTargetView = nullptr;
	//}
	//if (m_pRenderTargetTexture) {
	//	m_pRenderTargetTexture->Release();
	//	m_pRenderTargetTexture = nullptr;
	//}
	//if (m_pDepthStencilView) {
	//	m_pDepthStencilView->Release();
	//	m_pDepthStencilView = nullptr;
	//}
	//if (m_pDepthStencilTexture) {
	//	m_pDepthStencilTexture->Release();
	//	m_pDepthStencilTexture = nullptr;
	//}

	//auto renderCore = MainEngine::GetInstance()->GetRenderCore();

	//{
	//	D3D11_TEXTURE2D_DESC td = {};
	//	td.Width = m_Resolusion.x;
	//	td.Height = m_Resolusion.y;
	//	td.MipLevels = 1;
	//	td.ArraySize = 1;
	//	td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	//	td.SampleDesc.Count = 1;
	//	td.SampleDesc.Quality = 0;
	//	td.Usage = D3D11_USAGE_DEFAULT;
	//	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//	td.CPUAccessFlags = 0;
	//	td.MiscFlags = 0;
	//	renderCore->GetDevice()->CreateTexture2D(&td, NULL, &m_pRenderTargetTexture);

	//	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	//	srvd.Format = td.Format;
	//	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//	srvd.Texture2D.MipLevels = 1;

	//	ID3D11ShaderResourceView* pSRV;
	//	renderCore->GetDevice()->CreateShaderResourceView(m_pRenderTargetTexture, &srvd, &pSRV);
	//	renderCore->GetDevice()->CreateRenderTargetView(m_pRenderTargetTexture, NULL, &m_pRenderTargetView);

	//	m_pTexture->shader_resource_view = pSRV;
	//	m_pTexture->width = m_Resolusion.x;
	//	m_pTexture->height = m_Resolusion.y;
	//}

	//{
	//	D3D11_TEXTURE2D_DESC td = {};
	//	td.Width = m_Resolusion.x;
	//	td.Height = m_Resolusion.y;
	//	td.MipLevels = 1;
	//	td.ArraySize = 1;
	//	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//	td.SampleDesc.Count = 1;
	//	td.SampleDesc.Quality = 0;
	//	td.Usage = D3D11_USAGE_DEFAULT;
	//	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//	td.CPUAccessFlags = 0;
	//	td.MiscFlags = 0;
	//	renderCore->GetDevice()->CreateTexture2D(&td, NULL, &m_pDepthStencilTexture);

	//	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	//	dsvd.Format = td.Format;
	//	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//	dsvd.Flags = 0;
	//	renderCore->GetDevice()->CreateDepthStencilView(m_pDepthStencilTexture, &dsvd, &m_pDepthStencilView);
	//}

	//auto meshRenderer = owner->GetComponent<MeshRenderer>();
	//if (meshRenderer) {
	//	meshRenderer->GetMaterial()->SetTexture(m_TextureFID);
	//}


	//m_LastResolusion = m_Resolusion;
}

void PostProcessTexture::SetShaderResourceView(Texture* texture, int n)
{
	auto renderCore = MainEngine::GetInstance()->GetRenderCore();
	if (m_pPostProcessTexture[n]) {
		m_pPostProcessTexture[n] = texture;
		auto renderer = owner->GetComponent<PostProcessRenderer>();
		if (renderer) {
			renderer->GetPostProcessMaterial(n)->SetTexture(m_pPostProcessTexture[n]->GetFileID());
		}
	}
}

void PostProcessTexture::DrawGUI()
{
	ImGui::Image((ImTextureID)m_pPostProcessTexture[0]->shader_resource_view, ImVec2(300, 300));
	ImGui::Image((ImTextureID)m_pPostProcessTexture[1]->shader_resource_view, ImVec2(300, 300));
	ImGui::Image((ImTextureID)m_pPostProcessTexture[2]->shader_resource_view, ImVec2(300, 300));

}
