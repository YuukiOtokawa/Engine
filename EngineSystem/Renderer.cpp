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

#include "Renderer.h"

#include <d3dcompiler.h>
#include <io.h>

//==========================================================================
// メンバ関数
//==========================================================================

Renderer::Renderer(HWND hWnd) : m_Handle(hWnd) {
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

	m_ClientSize.x = (float)GetSystemMetrics(SM_CXSCREEN);
	m_ClientSize.y = (float)GetSystemMetrics(SM_CYSCREEN);

	m_ClientSize = Vector2O( renderWidth, renderHeight );

	int taskbarHeight = 0;
	//taskbarHeight = screenHeight - workArea.bottom;
	m_ClientSize.y -= taskbarHeight;

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
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

}

Renderer::~Renderer()
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
}

void Renderer::BufferClear()
{
	float clear_color[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clear_color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 255);
}

void Renderer::BufferPresent()
{
	m_pSwapChain->Present(0, 0);
}

void Renderer::ResizeClient(int width, int height)
{
	m_ClientSize = Vector2O( width, height );
}

void Renderer::CreateRenderTargetView()
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

void Renderer::CreateDepthStencil()
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

void Renderer::CreateViewPort()
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

void Renderer::CreateRasterizer()
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

void Renderer::CreateBlendState()
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

void Renderer::CreateDepthStencilState()
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

void Renderer::CreateSamplerState()
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

	ID3D11SamplerState* samplerState = NULL;
	m_pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
}

std::vector<Texture*> Renderer::GetTextureInfo()
{
	return m_Textures;
}

std::string Renderer::CreateVertexShader(std::string filename, std::string key)
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

std::string Renderer::CreatePixelShader(std::string filename, std::string key)
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

ID3D11InputLayout* Renderer::CreateInputLayout(unsigned char* pByteCode, long byteCodeLength)
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

void Renderer::CreateConstantBuffer()
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
	m_pDeviceContext->PSSetConstantBuffers(8, 1, &m_pMaterialBuffer);
}

int Renderer::TextureLoad(const std::wstring& filename, int fileID)
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
	LoadFromWICFile(filename.c_str(), WIC_FLAGS_NONE, &metadata, image);

	Texture* texture = new Texture();
	CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), metadata, &texture->shader_resource_view);
	texture->width = (int)metadata.width;
	texture->height = (int)metadata.height;
	texture->filename = filename;
	
	std::string str;

	if (!texture->shader_resource_view) {
		MessageBoxW(NULL, L"ファイルが読み込めなかった", filename.c_str(), MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	if (fileID != -1)
		texture->SetFileID(fileID);

	m_Textures.push_back(texture);

	return (UINT)m_Textures.size() - 1; // 新しく読み込んだテクスチャのIDを返す
}

int Renderer::AddTexture(Texture* texture)
{
	if (!texture->shader_resource_view) {
		return 0;
	}

	m_Textures.push_back(texture);

	return texture->GetFileID(); // 新しく読み込んだテクスチャのIDを返す

}

void Renderer::ResetTexture()
{
	for (auto it : m_Textures) {
		SAFE_RELEASE(it->shader_resource_view);
		delete it;
	}
	m_Textures.clear();
}

ID3D11ShaderResourceView** Renderer::GetTexture(int index)
{
	if (index == -1)
		return nullptr;
	return &m_Textures[index]->shader_resource_view;
}

int Renderer::GetTextureWidth(int index)
{
	return m_Textures[index]->width;
}

int Renderer::GetTextureHeight(int index)
{
	return m_Textures[index]->height;
}

void Renderer::SetVertexShader(std::string key)
{
	if (m_CurrentVertexShaderKey == key) return;
	m_pDeviceContext->VSSetShader(m_VertexShaders[key], NULL, 0);
	m_CurrentVertexShaderKey = key;
}

void Renderer::SetPixelShader(std::string key)
{
	if (m_CurrentPixelShaderKey == key) return;
	m_pDeviceContext->PSSetShader(m_PixelShaders[key], NULL, 0);
	m_CurrentPixelShaderKey = key;
}

void Renderer::SetWorldViewProjection2D() {
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

void Renderer::ResetWorldViewProjection3D()
{

}

void Renderer::SetRasterizerState3D()
{
	m_pDeviceContext->RSSetState(m_pRasterizerState);
}

void Renderer::SetRasterizerState2D()
{
	m_pDeviceContext->RSSetState(m_pRasterizerState2D);
}

void Renderer::SetTranslationMatrix(XMMATRIX translation)
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

void Renderer::SetAngleMatrix(XMMATRIX angle)
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

void Renderer::SetScaleMatrix(XMMATRIX scale)
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

void Renderer::SetViewMatrix(XMMATRIX view)
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

void Renderer::SetProjectionMatrix(XMMATRIX projection)
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

void Renderer::SetLight(LIGHT_BUFFER light)
{
	GetDeviceContext()->UpdateSubresource(m_pLightBuffer, 0, NULL, &light, 0, 0);
}

void Renderer::SetCamera(Vector4O position)
{
	Vector4O	temp = Vector4O(position.x, position.y, position.z, 0.0f);
	GetDeviceContext()->UpdateSubresource(m_pCameraBuffer, 0, NULL, &temp, 0, 0);
}

void Renderer::SetParameter(Vector4O position)
{
	GetDeviceContext()->UpdateSubresource(m_pCameraBuffer, 0, NULL, &position, 0, 0);
}

