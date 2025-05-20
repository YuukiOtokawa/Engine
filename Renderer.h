#pragma once

#include "FrameWork.h"

#include <d3d11.h>
#include "DirectXTex.h"

#include <map>

#include "Vector4O.h"

class Renderer
{
private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11Texture2D* m_pDepthStencilTexture;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_pDepthStencilView;

	ID3D11RasterizerState* m_pRasterizerState = NULL;
	ID3D11BlendState* m_pBlendState = NULL;
	ID3D11BlendState* m_pBlendStateAdd = NULL;

	ID3D11DepthStencilState* m_pDepthStencilStateDepthEnable = NULL;
	ID3D11DepthStencilState* m_pDepthStencilStateDepthDisable = NULL;
	ID3D11DepthStencilState* m_pDepthStencilStateShadow = NULL;
	ID3D11DepthStencilState* m_pDepthStencilStateBackGround = NULL;
	ID3D11DepthStencilState* m_pDepthStencilState3D = NULL;

	ID3D11SamplerState* m_pSamplerState = NULL;


	Vector4O m_ClientSize{ SCREEN_WIDTH_DEFAULT,SCREEN_HEIGHT_DEFAULT };

	HWND m_Handle;

	std::map<std::string, ID3D11VertexShader*> m_VertexShaders;
	std::map<std::string, ID3D11PixelShader*> m_PixelShaders;
	ID3D11InputLayout* m_pInputLayout = nullptr;

	std::string m_CurrentVertexShaderKey;
	std::string m_CurrentPixelShaderKey;

	ID3D11Buffer* m_pConstantBuffer = NULL;

	/// @brief レンダーターゲットビューの生成
	void CreateRenderTargetView();

	/// @brief デプスステンシルビュー・デプスステンシル用テクスチャ・デプスステンシルターゲットの生成
	void CreateDepthStencil();

	/// @brief ビューポート設定
	void CreateViewPort();

	/// @brief ラスタライザステート作成
	void CreateRasterizer();

	/// @brief ブレンドステート設定
	void CreateBlendState();

	/// @brief 深度ステート作成
	void CreateDepthStencilState();

	void CreateSamplerState();


public:
	Renderer(HWND hWnd);

	~Renderer();

	void BufferClear();
	void BufferPresent();
	void ResizeClient(int width, int height);

	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }
	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
	void SetRenderTargetView() { m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView); }
	Vector4O GetClientSize() { return m_ClientSize; }



	std::string CreateVertexShader(std::string filename, std::string key);
	std::string CreatePixelShader(std::string filename, std::string key);
	ID3D11InputLayout* CreateInputLayout(unsigned char* pByteCode, long byteCodeLength);
	void CreateConstantBuffer();

	void SetVertexShader(std::string key);
	void SetPixelShader(std::string key);

	void SetConstantBuffer(const ConstantBuffer* matrix);

	void SetRenderTargetView(ID3D11RenderTargetView* renderTargetView)
	{
		m_pDeviceContext->OMSetRenderTargets(1, &renderTargetView, m_pDepthStencilView);
	}
};

