#include "RenderTexture.h"

//#include "RenderCore.h"
#include "MainEngine.h"

#include "Component_MeshRenderer.h"

REGISTER_COMPONENT(RenderTexture)

RenderTexture::RenderTexture() : Component() {
	m_ClassID = CID_Component_RenderTexture;

	auto renderCore = MainEngine::GetInstance()->GetRenderCore();

	{
		D3D11_TEXTURE2D_DESC td = {};
		td.Width = m_Resolusion.x;
		td.Height = m_Resolusion.y;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		renderCore->GetDevice()->CreateTexture2D(&td, NULL, &m_pRenderTargetTexture);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
		srvd.Format = td.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;

		ID3D11ShaderResourceView* pSRV;
		renderCore->GetDevice()->CreateShaderResourceView(m_pRenderTargetTexture, &srvd, &pSRV);
		renderCore->GetDevice()->CreateRenderTargetView(m_pRenderTargetTexture, NULL, &m_pRenderTargetView);

		m_pTexture = new Texture;
		m_pTexture->shader_resource_view = pSRV;
		m_pTexture->width = m_Resolusion.x;
		m_pTexture->height = m_Resolusion.y;
		m_pTexture->toExport = false;

		m_TextureFID = renderCore->AddTexture(m_pTexture);
	}

	{
		D3D11_TEXTURE2D_DESC td = {};
		td.Width = m_Resolusion.x;
		td.Height = m_Resolusion.y;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		renderCore->GetDevice()->CreateTexture2D(&td, NULL, &m_pDepthStencilTexture);
		
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
		dsvd.Format = td.Format;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Flags = 0;
		renderCore->GetDevice()->CreateDepthStencilView(m_pDepthStencilTexture, &dsvd, &m_pDepthStencilView);
	}

	
}

RenderTexture::~RenderTexture()
{
	if (m_pRenderTargetView) {
		m_pRenderTargetView->Release();
		m_pRenderTargetView = nullptr;
	}
	if (m_pRenderTargetTexture) {
		m_pRenderTargetTexture->Release();
		m_pRenderTargetTexture = nullptr;
	}
	if (m_pDepthStencilView) {
		m_pDepthStencilView->Release();
		m_pDepthStencilView = nullptr;
	}
	if (m_pDepthStencilTexture) {
		m_pDepthStencilTexture->Release();
		m_pDepthStencilTexture = nullptr;
	}


}

#include "Component_Transform.h"

void RenderTexture::Update()
{
	if (!m_pCamera) return;

	if (!m_IsTextureAttached) {
		auto meshRenderer = owner->GetComponent<MeshRenderer>();
		if (meshRenderer) {
			meshRenderer->GetMaterial()->SetTexture(m_TextureFID);
			m_IsTextureAttached = true;
		}
	}

	auto scale = owner->GetComponent<Transform>()->GetScale();
	m_Resolusion = Vector2O(scale.x, scale.y);

	// 解像度が変わったらレンダーターゲットを再作成

	if (m_Resolusion == m_LastResolusion) return;

	if (m_pRenderTargetView) {
		m_pRenderTargetView->Release();
		m_pRenderTargetView = nullptr;
	}
	if (m_pRenderTargetTexture) {
		m_pRenderTargetTexture->Release();
		m_pRenderTargetTexture = nullptr;
	}
	if (m_pDepthStencilView) {
		m_pDepthStencilView->Release();
		m_pDepthStencilView = nullptr;
	}
	if (m_pDepthStencilTexture) {
		m_pDepthStencilTexture->Release();
		m_pDepthStencilTexture = nullptr;
	}

	auto renderCore = MainEngine::GetInstance()->GetRenderCore();

	{
		D3D11_TEXTURE2D_DESC td = {};
		td.Width = m_Resolusion.x;
		td.Height = m_Resolusion.y;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		renderCore->GetDevice()->CreateTexture2D(&td, NULL, &m_pRenderTargetTexture);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
		srvd.Format = td.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;

		ID3D11ShaderResourceView* pSRV;
		renderCore->GetDevice()->CreateShaderResourceView(m_pRenderTargetTexture, &srvd, &pSRV);
		renderCore->GetDevice()->CreateRenderTargetView(m_pRenderTargetTexture, NULL, &m_pRenderTargetView);

		m_pTexture->shader_resource_view = pSRV;
		m_pTexture->width = m_Resolusion.x;
		m_pTexture->height = m_Resolusion.y;
	}

	{
		D3D11_TEXTURE2D_DESC td = {};
		td.Width = m_Resolusion.x;
		td.Height = m_Resolusion.y;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		renderCore->GetDevice()->CreateTexture2D(&td, NULL, &m_pDepthStencilTexture);

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
		dsvd.Format = td.Format;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Flags = 0;
		renderCore->GetDevice()->CreateDepthStencilView(m_pDepthStencilTexture, &dsvd, &m_pDepthStencilView);
	}

	auto meshRenderer = owner->GetComponent<MeshRenderer>();
	if (meshRenderer) {
		meshRenderer->GetMaterial()->SetTexture(m_TextureFID);
	}


	m_LastResolusion = m_Resolusion;
}

void RenderTexture::DrawRenderTexture()
{


	if (m_pCamera == nullptr) {
		return;
	}
	auto renderCore = MainEngine::GetInstance()->GetRenderCore();

	renderCore->GetDeviceContext()->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = m_Resolusion.x;
	vp.Height = m_Resolusion.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	renderCore->GetDeviceContext()->RSSetViewports(1, &vp);

	float clearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	renderCore->GetDeviceContext()->ClearRenderTargetView(m_pRenderTargetView, clearColor);
	renderCore->GetDeviceContext()->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);



	Editor::GetInstance()->DrawGame(m_pCamera, owner);


}

#include "EngineMetaFile.h"

void RenderTexture::DrawGUI()
{
	static int cameraIndex = -1;
	std::list<Component*> cameras = Editor::GetInstance()->GetComponentsByClassID(CID_Component_Camera);
	
	std::vector<std::string> cameraNameStrings;
	std::vector<const char*> cameraNames;

	for (auto cam : cameras) {
		cameraNameStrings.push_back(cam->owner->GetName());
	}

	// Cameraコンポーネントの名前リストを作成
	for (const auto& name : cameraNameStrings) {
		cameraNames.push_back(name.c_str());
	}

	if (m_pCamera)
		cameraIndex = std::distance(cameras.begin(),
									std::find(cameras.begin(),
													cameras.end(), 
													m_pCamera->GetComponent<Camera>()
													)
									);

	ImGui::Indent();

	char* buf = std::to_string(m_TextureFID).data();

	ImGui::DragFloat2("Resolution", &m_Resolusion.x, 1.0f, 1.0f, 4096.0f);
	ImGui::InputText("Texture FID", buf, ImGuiInputTextFlags_ReadOnly);

	if (ImGui::Combo("Camera", &cameraIndex, cameraNames.data(),
		static_cast<int>(cameraNames.size()))) {
		auto it = cameras.begin();
		std::advance(it, cameraIndex);
		m_pCamera = (*it)->owner;
	}

	ImGui::Unindent();
}

void RenderTexture::InitializeTag()
{
}
