// ========================================================
//
// レンダラークラス[Renderer.cpp]
// 
//									Date:20250514
//									Author:Yuuki Otokawa
// ========================================================

//==========================================================================
// マクロ定義
//==========================================================================

#define _CRT_SECURE_NO_WARNINGS

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "RenderCore.h"

#include <d3dcompiler.h>
#include <io.h>
#include "imgui.h"

//==========================================================================
// メンバ関数
//==========================================================================

RenderCore::RenderCore(HWND hWnd) : m_Handle(hWnd) {
	// Direct 3Dバージョンの定義
	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

	// 描画領域のサイズを定義
	// ウィンドウのクライアント領域のサイズを取得
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// クライアント領域の幅と高さを計算
	int renderWidth = clientRect.right - clientRect.left;
	int renderHeight = clientRect.bottom - clientRect.top;

	// タスクバーの高さを考慮して、描画領域の高さを調整
	RECT workArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

	m_ClientSize.x = (float)renderWidth;
	m_ClientSize.y = (float)renderHeight;

	int taskbarHeight = 0;
	taskbarHeight = renderHeight - workArea.bottom;
	m_ClientSize.y -= taskbarHeight;

	//m_ClientSize = Vector2O(1920, 1030);

	// スワップチェーンの設定を定義
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = (UINT)m_ClientSize.x;
	sd.BufferDesc.Height = (UINT)m_ClientSize.y;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_Handle;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	// デバイスとスワップチェーンの作成
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&m_pSwapChain,
		&m_pDevice,
		&feature_level,
		&m_pDeviceContext);

	CreateDepthStencil();
	CreateRenderTargetView();
	CreateViewPort();
	CreateRasterizer();
	CreateBlendState();
	CreateDepthStencilState();
	CreateSamplerState();
	CreateConstantBuffer();
	m_pDeviceContext->RSSetState(m_pRasterizerState);
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilStateDepthEnable, 1);
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	TextureLoad(L"asset/texture/Default_White.png");
	CreatePostProcessBuffer();
	CreateSceneGameViewBuffer();
	InitializeFullScreenQuad();

	CreateVertexShader("cso/vertexShader.cso", "vertex");
}

 RenderCore::~RenderCore()
{
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDepthStencilTexture);

	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pDevice);

	SAFE_RELEASE(m_pRasterizerState);
	SAFE_RELEASE(m_pBlendState);
	SAFE_RELEASE(m_pBlendStateAdd);
	SAFE_RELEASE(m_pDepthStencilStateDepthEnable);
	SAFE_RELEASE(m_pDepthStencilStateDepthDisable);

	SAFE_RELEASE(m_pTranslationBuffer);
	SAFE_RELEASE(m_pAngleBuffer);
	SAFE_RELEASE(m_pScaleBuffer);
	SAFE_RELEASE(m_pViewBuffer);
	SAFE_RELEASE(m_pProjectionBuffer);
	SAFE_RELEASE(m_pLightBuffer);
	SAFE_RELEASE(m_pCameraBuffer);
	SAFE_RELEASE(m_pParameterBuffer);
	SAFE_RELEASE(m_pSamplerState);

	for (auto& shader : m_VertexShaders) {
		SAFE_RELEASE(shader.second);
	}
	for (auto& shader : m_PixelShaders) {
		SAFE_RELEASE(shader.second);
	}
	SAFE_RELEASE(m_pInputLayout);

	for (int i = 0;i<3;i++) {
		SAFE_RELEASE(m_pPostProcessRTV[i]);
		SAFE_RELEASE(m_pPostProcessSRV[i]);
	}

	SAFE_RELEASE(m_pFullScreenQuadVB);
}

void RenderCore::BufferClear()
{
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	float clear_color[4] = { 0.5f, 0.0f, 0.0f, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clear_color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 255);
}

void RenderCore::BufferPresent()
{
	m_pSwapChain->Present(0, 0);
}

void RenderCore::BeginPE(int n)
{
	m_pDeviceContext->OMSetRenderTargets(1, &m_pPostProcessRTV[n], m_pDepthStencilView);
	float clear_color[4] = { 0.0f, 0.0f, 0.5f, 1.0f };

	if (n != 0) clear_color[1] = 0.5f;

	m_pDeviceContext->ClearRenderTargetView(m_pPostProcessRTV[n], clear_color);

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 255);
}

void RenderCore::ResizeClient(int width, int height)
{
	// サイズが0の場合は何もしない（最小化時など）
	if (width == 0 || height == 0) {
		return;
	}

	// 同じサイズの場合は何もしない
	if (m_ClientSize.x == width && m_ClientSize.y == height) {
		return;
	}

	// 新しいサイズを保存
	m_ClientSize = Vector2O((float)width, (float)height);

	// レンダーターゲットとデプスステンシルを解放
	ReleaseRenderTargets();

	// スワップチェーンのバッファをリサイズ
	HRESULT hr = m_pSwapChain->ResizeBuffers(
		1,
		width,
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0
	);

	if (FAILED(hr)) {
		MessageBoxA(NULL, "Failed to resize swap chain buffers", "Error", MB_ICONERROR);
		return;
	}

	// レンダーターゲットとデプスステンシルを再作成
	RecreateRenderTargets();

	// ImGuiのディスプレイサイズを更新
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)width, (float)height);
}

void RenderCore::ReleaseRenderTargets()
{
	// レンダーターゲットビューを解放
	if (m_pRenderTargetView) {
		m_pRenderTargetView->Release();
		m_pRenderTargetView = nullptr;
	}

	// デプスステンシルビューを解放
	if (m_pDepthStencilView) {
		m_pDepthStencilView->Release();
		m_pDepthStencilView = nullptr;
	}

	// デプスステンシルテクスチャを解放
	if (m_pDepthStencilTexture) {
		m_pDepthStencilTexture->Release();
		m_pDepthStencilTexture = nullptr;
	}

	// ポストプロセスバッファを解放
	for (int i = 0; i < 3; i++) {
		if (m_pPostProcessRTV[i]) {
			m_pPostProcessRTV[i]->Release();
			m_pPostProcessRTV[i] = nullptr;
		}
		if (m_pPostProcessSRV[i]) {
			m_pPostProcessSRV[i]->Release();
			m_pPostProcessSRV[i] = nullptr;
		}
	}

	// シーンビューバッファを解放
	if (m_pSceneViewRTV) {
		m_pSceneViewRTV->Release();
		m_pSceneViewRTV = nullptr;
	}
	if (m_pSceneViewSRV) {
		m_pSceneViewSRV->Release();
		m_pSceneViewSRV = nullptr;
	}

	// ゲームビューバッファを解放
	if (m_pGameViewRTV) {
		m_pGameViewRTV->Release();
		m_pGameViewRTV = nullptr;
	}
	if (m_pGameViewSRV) {
		m_pGameViewSRV->Release();
		m_pGameViewSRV = nullptr;
	}
}

void RenderCore::RecreateRenderTargets()
{
	// レンダーターゲットビューを再作成
	CreateRenderTargetView();

	// デプスステンシルを再作成
	CreateDepthStencil();

	// ビューポートを更新
	CreateViewPort();

	// ポストプロセスバッファを再作成
	CreatePostProcessBuffer();

	// シーン/ゲームビューバッファを再作成
	CreateSceneGameViewBuffer();

	// レンダーターゲットを設定
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
}

void RenderCore::CreateRenderTargetView()
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (pBackBuffer == nullptr) {
		MessageBoxA(NULL, "Failed to get back buffer", "Error", MB_ICONERROR);
		return;
	}
	HRESULT hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	pBackBuffer->Release();
	SetRenderTargetView(m_pRenderTargetView);
}

void RenderCore::CreateDepthStencil()
{
	D3D11_TEXTURE2D_DESC td = {};
	td.Width = (UINT)m_ClientSize.x;
	td.Height = (UINT)m_ClientSize.y;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	HRESULT hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pDepthStencilTexture);
	if (!m_pDepthStencilTexture) {
		MessageBoxA(NULL, "Failed to create depth stencil texture", "Error", MB_ICONERROR);
		return;
	}
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags = 0;
	hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilTexture, &dsvd, &m_pDepthStencilView);
}

void RenderCore::CreateViewPort()
{
	D3D11_VIEWPORT vp = {};
	vp.Width = (FLOAT)m_ClientSize.x;
	vp.Height = (FLOAT)m_ClientSize.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pDeviceContext->RSSetViewports(1, &vp);
}

void RenderCore::CreateRasterizer()
{
	D3D11_RASTERIZER_DESC rd = {};
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = FALSE;
	m_pDevice->CreateRasterizerState(&rd, &m_pRasterizerState);

	rd.CullMode = D3D11_CULL_NONE;
	m_pDevice->CreateRasterizerState(&rd, &m_pRasterizerState2D);

}

void RenderCore::CreateBlendState()
{
	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = FALSE;
	bd.IndependentBlendEnable = FALSE;
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	float blendFactor[4] = { 0.0f,0.0f,0.0f,0.0f };
	m_pDevice->CreateBlendState(&bd, &m_pBlendState);
	m_pDeviceContext->OMSetBlendState(m_pBlendState, blendFactor, 0xffffffff);
}

void RenderCore::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC dsd = {};

	//深度有効ステート
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsd.StencilEnable = FALSE;

	m_pDevice->CreateDepthStencilState(&dsd, &m_pDepthStencilStateDepthEnable);

	//深度無効ステート
	dsd.DepthEnable = FALSE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	m_pDevice->CreateDepthStencilState(&dsd, &m_pDepthStencilStateDepthDisable);

}

void RenderCore::CreateSamplerState()
{

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	m_pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);


	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;

	m_pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	m_pDeviceContext->PSSetSamplers(1, 1, &m_pSamplerState);
}

void RenderCore::CreatePostProcessBuffer()
{
	ID3D11Texture2D* pTexture = nullptr;
	D3D11_TEXTURE2D_DESC td = {};

	td.Width = (UINT)m_ClientSize.x;
	td.Height = (UINT)m_ClientSize.y;

	td.MipLevels = 1;
	td.ArraySize = 1;

	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;

	td.Usage = D3D11_USAGE_DEFAULT;

	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	HRESULT hr = m_pDevice->CreateTexture2D(&td, NULL, &pTexture);

	D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
	rtvd.Format = td.Format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = m_pDevice->CreateRenderTargetView(pTexture, &rtvd, &m_pPostProcessRTV[0]);
	hr = m_pDevice->CreateRenderTargetView(pTexture, &rtvd, &m_pPostProcessRTV[1]);
	hr = m_pDevice->CreateRenderTargetView(pTexture, &rtvd, &m_pPostProcessRTV[2]);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = td.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	hr = m_pDevice->CreateShaderResourceView(pTexture, &srvd, &m_pPostProcessSRV[0]);
	hr = m_pDevice->CreateShaderResourceView(pTexture, &srvd, &m_pPostProcessSRV[1]);
	hr = m_pDevice->CreateShaderResourceView(pTexture, &srvd, &m_pPostProcessSRV[2]);

	Texture* texture[3];
	for (int i = 0; i < 3; i++) {
		texture[i] = new Texture();
		texture[i]->width = m_ClientSize.x;
		texture[i]->height = m_ClientSize.y;
		texture[i]->shaderResourceView = m_pPostProcessSRV[i];
		texture[i]->toExport = false;
		//AddTexture(texture[i]);
		m_PostProcessTexture[i] = texture[i];
	}
}

void RenderCore::CreateSceneGameViewBuffer()
{
	// SceneView用のテクスチャを作成
	ID3D11Texture2D* pSceneTexture = nullptr;
	D3D11_TEXTURE2D_DESC td = {};

	td.Width = (UINT)m_ClientSize.x;
	td.Height = (UINT)m_ClientSize.y;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	HRESULT hr = m_pDevice->CreateTexture2D(&td, NULL, &pSceneTexture);

	// GameView用のテクスチャを作成
	ID3D11Texture2D* pGameTexture = nullptr;
	hr = m_pDevice->CreateTexture2D(&td, NULL, &pGameTexture);

	// SceneView用のRenderTargetViewとShaderResourceViewを作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
	rtvd.Format = td.Format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = m_pDevice->CreateRenderTargetView(pSceneTexture, &rtvd, &m_pSceneViewRTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = td.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	hr = m_pDevice->CreateShaderResourceView(pSceneTexture, &srvd, &m_pSceneViewSRV);

	// GameView用のRenderTargetViewとShaderResourceViewを作成
	hr = m_pDevice->CreateRenderTargetView(pGameTexture, &rtvd, &m_pGameViewRTV);
	hr = m_pDevice->CreateShaderResourceView(pGameTexture, &srvd, &m_pGameViewSRV);

	// SceneView用のTextureオブジェクトを作成
	Texture* sceneTexture = new Texture();
	sceneTexture->width = m_ClientSize.x;
	sceneTexture->height = m_ClientSize.y;
	sceneTexture->shaderResourceView = m_pSceneViewSRV;
	sceneTexture->toExport = false;
	m_pSceneViewTexture = sceneTexture;

	// GameView用のTextureオブジェクトを作成
	Texture* gameTexture = new Texture();
	gameTexture->width = m_ClientSize.x;
	gameTexture->height = m_ClientSize.y;
	gameTexture->shaderResourceView = m_pGameViewSRV;
	gameTexture->toExport = false;
	m_pGameViewTexture = gameTexture;

	// テクスチャリソースを解放（ViewがテクスチャをReference countしているため）
	pSceneTexture->Release();
	pGameTexture->Release();
}
void RenderCore::BeginSceneView()
{
	m_pDeviceContext->OMSetRenderTargets(1, &m_pSceneViewRTV, m_pDepthStencilView);
	float clear_color[4] = { 0.0f, 0.0f, 0.5f, 1.0f };


	m_pDeviceContext->ClearRenderTargetView(m_pSceneViewRTV, clear_color);

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 255);
}

void RenderCore::BeginGameView()
{
	m_pDeviceContext->OMSetRenderTargets(1, &m_pGameViewRTV, m_pDepthStencilView);
	float clear_color[4] = { 0.0f, 0.0f, 0.5f, 1.0f };


	m_pDeviceContext->ClearRenderTargetView(m_pGameViewRTV, clear_color);

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 255);
}

std::vector<Texture*> RenderCore::GetTextureInfo()
{
	return m_Textures;
}

std::string RenderCore::CreateVertexShader(std::string filename, std::string key)
{
	if (m_VertexShaders.find(key) != m_VertexShaders.end()) {
		// すでに同じキーのシェーダーが存在する場合は、何もせずキーを返す
		return key;
	}

	// コンパイルされたシェーダーファイルを開く
	FILE* file;

	file = fopen(filename.c_str(), "rb");
	assert(file);

	long int fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);


	// シェーダーを作成
	ID3D11VertexShader* pVertexShader = NULL;

	HRESULT hr = m_pDevice->CreateVertexShader(buffer, fsize, NULL, &pVertexShader);

	for (auto shader : m_VertexShaders) {
		// すでに同じキーのシェーダーが存在する場合は、何もせずキーを返す
		if (shader.second == pVertexShader) {
			return shader.first;
		}
	}

	m_VertexShaders.emplace(key, pVertexShader);

	// 入力レイアウトを作成
	if (m_pInputLayout == nullptr) {
		m_pInputLayout = CreateInputLayout(buffer, fsize);
		m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	}

	delete[] buffer;

	return key;
}

std::string RenderCore::CreatePixelShader(std::string filename, std::string key)
{
	if (m_PixelShaders.find(key) != m_PixelShaders.end()) {
		// すでに同じキーのシェーダーが存在する場合は、何もせずキーを返す
		return key;
	}

	FILE* file;

	file = fopen(filename.c_str(), "rb");
	assert(file);

	long int fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);


	ID3D11PixelShader* pPixelShader = NULL;

	HRESULT hr = m_pDevice->CreatePixelShader(buffer, fsize, NULL, &pPixelShader);

	for (auto shader : m_PixelShaders) {
		// すでに同じキーのシェーダーが存在する場合は、何もせずキーを返す
		if (shader.second == pPixelShader) {
			return shader.first;
		}
	}


	m_PixelShaders.emplace(key, pPixelShader);
	delete[] buffer;

	return key;
}

ID3D11InputLayout* RenderCore::CreateInputLayout(unsigned char* pByteCode, long byteCodeLength)
{
	// 入力レイアウト生成
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);


	ID3D11InputLayout* pVertexLayout = NULL;

	HRESULT hr = m_pDevice->CreateInputLayout(layout,
		numElements,
		pByteCode,
		byteCodeLength,
		&pVertexLayout);

	return pVertexLayout;
}

void RenderCore::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC hBufferDesc = {};
	hBufferDesc.ByteWidth = sizeof(XMFLOAT4X4);
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	//================================================
// WorldViewProjection行列用定数バッファ生成
	//行列オブジェクトをシェーダーへ接続　b0をつかう
	m_pDevice->CreateBuffer(&hBufferDesc, NULL, &m_pTranslationBuffer);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pTranslationBuffer);
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pTranslationBuffer);

	m_pDevice->CreateBuffer(&hBufferDesc, NULL, &m_pAngleBuffer);
	m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pAngleBuffer);
	m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pAngleBuffer);

	m_pDevice->CreateBuffer(&hBufferDesc, NULL, &m_pScaleBuffer);
	m_pDeviceContext->VSSetConstantBuffers(2, 1, &m_pScaleBuffer);
	m_pDeviceContext->PSSetConstantBuffers(2, 1, &m_pScaleBuffer);

	m_pDevice->CreateBuffer(&hBufferDesc, NULL, &m_pViewBuffer);
	m_pDeviceContext->VSSetConstantBuffers(3, 1, &m_pViewBuffer);
	m_pDeviceContext->PSSetConstantBuffers(3, 1, &m_pViewBuffer);

	m_pDevice->CreateBuffer(&hBufferDesc, NULL, &m_pProjectionBuffer);
	m_pDeviceContext->VSSetConstantBuffers(4, 1, &m_pProjectionBuffer);
	m_pDeviceContext->PSSetConstantBuffers(4, 1, &m_pProjectionBuffer);

	hBufferDesc.ByteWidth = sizeof(LIGHT_BUFFER);

	HRESULT hr =m_pDevice->CreateBuffer(&hBufferDesc, NULL, &m_pLightBuffer);
	m_pDeviceContext->VSSetConstantBuffers(5, 1, &m_pLightBuffer);
	m_pDeviceContext->PSSetConstantBuffers(5, 1, &m_pLightBuffer);

	hBufferDesc.ByteWidth = sizeof(Vector4O);
	m_pDevice->CreateBuffer(&hBufferDesc, NULL, &m_pCameraBuffer);
	m_pDeviceContext->PSSetConstantBuffers(6, 1, &m_pCameraBuffer);

	m_pDevice->CreateBuffer(&hBufferDesc, NULL, &m_pParameterBuffer);
	m_pDeviceContext->PSSetConstantBuffers(7, 1, &m_pParameterBuffer);

	hBufferDesc.ByteWidth = sizeof(MATERIAL);
	m_pDevice->CreateBuffer(&hBufferDesc, NULL, &m_pMaterialBuffer);
	m_pDeviceContext->VSSetConstantBuffers(8, 1, &m_pMaterialBuffer);  // Vertex Shaderに追加
	m_pDeviceContext->PSSetConstantBuffers(8, 1, &m_pMaterialBuffer);

	hBufferDesc.ByteWidth = sizeof(float) * 8;
	m_pDevice->CreateBuffer(&hBufferDesc, NULL, &m_pWeightBuffer);
	m_pDeviceContext->PSSetConstantBuffers(9, 1, &m_pWeightBuffer);  // Pixel Shaderに追加
}

int RenderCore::TextureLoad(const std::wstring& filename, int fileID)
{
	// すでに同名のテクスチャが読み込まれていないか確認する
	{
		for (auto it : m_Textures) {
			if (it->filename == filename) {
				return it->GetFileID(); // すでに読み込まれていたのでidをわたす
			}
		}

	}
	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	ScratchImage mipChain;
	LoadFromWICFile(filename.c_str(), WIC_FLAGS_NONE, &metadata, image);

	GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);

	Texture* texture = new Texture();
	CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), metadata, &texture->shaderResourceView);
	CreateShaderResourceView(m_pDevice, mipChain.GetImages(), mipChain.GetImageCount(), metadata, &texture->mipMap);
	texture->width = (int)metadata.width;
	texture->height = (int)metadata.height;
	texture->filename = filename;
	
	std::string str;

	if (!texture->shaderResourceView) {
		MessageBoxW(NULL, L"ファイルが読み込めなかった", filename.c_str(), MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	if (fileID != -1)
		texture->SetFileID(fileID);

	m_Textures.push_back(texture);

	return texture->GetFileID(); // 新しく読み込んだテクスチャのIDを返す
}

int RenderCore::AddTexture(Texture* texture)
{
	if (!texture->shaderResourceView && texture->toExport) {
		return 0;
	}

	m_Textures.push_back(texture);

	return texture->GetFileID(); // 新しく読み込んだテクスチャのIDを返す

}

void RenderCore::ResetTexture()
{
	for (auto it : m_Textures) {
		SAFE_RELEASE(it->shaderResourceView);
		delete it;
	}
	m_Textures.clear();

	TextureLoad(L"asset/texture/Default_White.png");
}

#include "Editor.h"

ID3D11ShaderResourceView** RenderCore::GetTexture(int index)
{
	for (auto it : m_Textures) {
		if (it->GetFileID() == index) {
			return &it->shaderResourceView;
		}
	}
	return nullptr;
}

int RenderCore::GetTextureWidth(int index)
{
	return m_Textures[index]->width;
}

int RenderCore::GetTextureHeight(int index)
{
	return m_Textures[index]->height;
}

void RenderCore::SetVertexShader(std::string key)
{
	if (m_CurrentVertexShaderKey == key) return;
	m_pDeviceContext->VSSetShader(m_VertexShaders[key], NULL, 0);
	m_CurrentVertexShaderKey = key;
}

void RenderCore::SetPixelShader(std::string key)
{
	if (m_CurrentPixelShaderKey == key) return;
	m_pDeviceContext->PSSetShader(m_PixelShaders[key], NULL, 0);
	m_CurrentPixelShaderKey = key;
}

void RenderCore::SetWorldViewProjection2D() {
	//2D用正射影行列をセット
// C++側
	float screenWidth = m_ClientSize.x;
	float screenHeight = m_ClientSize.y;

	if (screenWidth == 0.0f || screenHeight == 0.0f) {
		screenWidth = 1920;
		screenHeight = 1080;
	}

	// (0,0)が画面中央になるようにプロジェクション行列を設定
	XMMATRIX projectionMatrix = XMMatrixOrthographicOffCenterLH(
		-screenWidth / 2.0f,    // left:   左端を-width/2に
		screenWidth / 2.0f,    // right:  右端を+width/2に
		screenHeight / 2.0f,   // bottom: 下端を+height/2に
		-screenHeight / 2.0f,   // top:    上端を-height/2に
		0.0f,                   // zNear
		1.0f                    // zFar
	);

	// この 'Projection' 行列をシェーダーに渡します。
	SetProjectionMatrix(projectionMatrix);
	//行列を単位行列にして初期化
	SetViewMatrix(XMMatrixIdentity());
	SetTranslationMatrix(XMMatrixIdentity());
	SetAngleMatrix(XMMatrixIdentity());
	SetScaleMatrix(XMMatrixIdentity());

}

void RenderCore::ResetWorldViewProjection3D()
{

}

void RenderCore::SetRasterizerState3D()
{
	m_pDeviceContext->RSSetState(m_pRasterizerState);
}

void RenderCore::SetRasterizerState2D()
{
	m_pDeviceContext->RSSetState(m_pRasterizerState2D);
}

void RenderCore::SetTranslationMatrix(XMMATRIX translation)
{
	XMMATRIX transMatrix = XMMatrixTranspose(translation);
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, transMatrix);
	m_pDeviceContext->UpdateSubresource(
		m_pTranslationBuffer,
		0,
		NULL,
		&matrix,
		0,
		0);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pTranslationBuffer);
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pTranslationBuffer);
}

void RenderCore::SetAngleMatrix(XMMATRIX angle)
{
	XMMATRIX angleMatrix = XMMatrixTranspose(angle);
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, angleMatrix);
	m_pDeviceContext->UpdateSubresource(
		m_pAngleBuffer,
		0,
		NULL,
		&matrix,
		0,
		0);
	m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pAngleBuffer);
	m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pAngleBuffer);
}

void RenderCore::SetScaleMatrix(XMMATRIX scale)
{
	XMMATRIX scaleMatrix = XMMatrixTranspose(scale);
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, scaleMatrix);
	m_pDeviceContext->UpdateSubresource(
		m_pScaleBuffer,
		0,
		NULL,
		&matrix,
		0,
		0);
	m_pDeviceContext->VSSetConstantBuffers(2, 1, &m_pScaleBuffer);
	m_pDeviceContext->PSSetConstantBuffers(2, 1, &m_pScaleBuffer);
}

void RenderCore::SetViewMatrix(XMMATRIX view)
{
	XMMATRIX viewMatrix;
	viewMatrix = XMMatrixTranspose(view);
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, viewMatrix);
	m_pDeviceContext->UpdateSubresource(
		m_pViewBuffer,
		0,
		NULL,
		&matrix,
		0,
		0);
	m_pDeviceContext->VSSetConstantBuffers(3, 1, &m_pViewBuffer);
	m_pDeviceContext->PSSetConstantBuffers(3, 1, &m_pViewBuffer);
}

void RenderCore::SetProjectionMatrix(XMMATRIX projection)
{
	XMMATRIX projectionMatrix;
	projectionMatrix = XMMatrixTranspose(projection);
	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, projectionMatrix);
	m_pDeviceContext->UpdateSubresource(
		m_pProjectionBuffer,
		0,
		NULL,
		&matrix,
		0,
		0);
	m_pDeviceContext->VSSetConstantBuffers(4, 1, &m_pProjectionBuffer);
	m_pDeviceContext->PSSetConstantBuffers(4, 1, &m_pProjectionBuffer);
}

void RenderCore::SetLight(LIGHT_BUFFER light)
{
	GetDeviceContext()->UpdateSubresource(m_pLightBuffer, 0, NULL, &light, 0, 0);
}

void RenderCore::SetCamera(Vector4O position)
{
	Vector4O	temp = Vector4O(position.x, position.y, position.z, 0.0f);
	GetDeviceContext()->UpdateSubresource(m_pCameraBuffer, 0, NULL, &temp, 0, 0);
}

void RenderCore::SetParameter(Vector4O position)
{
	GetDeviceContext()->UpdateSubresource(m_pCameraBuffer, 0, NULL, &position, 0, 0);
}

void RenderCore::SetWeight(float* weight)
{
	m_pDeviceContext->UpdateSubresource(m_pWeightBuffer, 0, NULL, &weight[0], 0, 0);
}

void RenderCore::InitializeFullScreenQuad()
{
	// フルスクリーンクアッド用の頂点データ（SpriteMeshと同じ構造）
	VERTEX vertices[4] = {
		// vertex[0]: 左上
		{
			Vector3O(-1.0f, 1.0f, 0.0f),
			Vector3O(0.0f, 0.0f, -1.0f),
			Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
			Vector2O(0.0f, 0.0f)
		},
		// vertex[1]: 右上
		{
			Vector3O(1.0f, 1.0f, 0.0f),
			Vector3O(0.0f, 0.0f, -1.0f),
			Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
			Vector2O(1.0f, 0.0f)
		},
		// vertex[2]: 左下
		{
			Vector3O(-1.0f, -1.0f, 0.0f),
			Vector3O(0.0f, 0.0f, -1.0f),
			Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
			Vector2O(0.0f, 1.0f)
		},
		// vertex[3]: 右下
		{
			Vector3O(1.0f, -1.0f, 0.0f),
			Vector3O(0.0f, 0.0f, -1.0f),
			Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
			Vector2O(1.0f, 1.0f)
		}
	};

	// 頂点バッファを作成
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(VERTEX) * 4;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;

	HRESULT hr = m_pDevice->CreateBuffer(&bd, &initData, &m_pFullScreenQuadVB);
	if (FAILED(hr)) {
		// エラーハンドリング
		MessageBox(NULL, L"フルスクリーンクアッドの頂点バッファ作成に失敗しました", L"Error", MB_OK);
	}
}

void RenderCore::BeginPostProcess(int n)
{
	BeginPE(n);
}

void RenderCore::DrawFullScreenQuad(ID3D11RenderTargetView* renderTargetView, ID3D11ShaderResourceView* shaderResourceView)
{
	// レンダーターゲットを設定
	m_pDeviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

	// 入力テクスチャを設定
	m_pDeviceContext->PSSetShaderResources(0, 1, &shaderResourceView);

	// 頂点バッファを設定
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pFullScreenQuadVB, &stride, &offset);

	// プリミティブトポロジーを設定
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 描画
	m_pDeviceContext->Draw(4, 0);
}
