// ========================================================
//
// レンダラークラス[Renderer.h]
// 
//									Date:20250514
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// 繝倥ャ繝繝ｼ繧､繝ｳ繧ｯ繝ｫ繝ｼ繝�
//==========================================================================

#include "FrameWork.h"

#include <d3d11.h>
#include "DirectXTex.h"

#include <map>

#include "Vector4O.h"

#include <vector>
#include <string>

//==========================================================================
// 繧ｯ繝ｩ繧ｹ螳夂ｾｩ
//==========================================================================

class Renderer
{
private:
	// 繝�繧ｯ繧ｹ繝√Ε縺ｮ讒矩�菴灘ｮ夂ｾｩ
	struct Texture {
		std::wstring filename;
		ID3D11ShaderResourceView* shader_resource_view;
		int width = 0;
		int height = 0;
	};

	// DirectX 11髢｢菫ゅ�ｮ繝昴う繝ｳ繧ｿ
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11Texture2D* m_pDepthStencilTexture = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_pDepthStencilView;

	// 繝ｩ繧ｹ繧ｿ繝ｩ繧､繧ｶ繧ｹ繝�繝ｼ繝医�ｮ繝昴う繝ｳ繧ｿ
	ID3D11RasterizerState* m_pRasterizerState = NULL;
    ID3D11RasterizerState* m_pRasterizerState2D = NULL;
	// 繝悶Ξ繝ｳ繝峨せ繝�繝ｼ繝医�ｮ繝昴う繝ｳ繧ｿ
	ID3D11BlendState* m_pBlendState = NULL;
	ID3D11BlendState* m_pBlendStateAdd = NULL;

	// 豺ｱ蠎ｦ繧ｹ繝�繝ｳ繧ｷ繝ｫ繧ｹ繝�繝ｼ繝医�ｮ繝昴う繝ｳ繧ｿ
	ID3D11DepthStencilState* m_pDepthStencilStateDepthEnable = NULL;
	ID3D11DepthStencilState* m_pDepthStencilStateDepthDisable = NULL;
	ID3D11DepthStencilState* m_pDepthStencilStateShadow = NULL;
	ID3D11DepthStencilState* m_pDepthStencilStateBackGround = NULL;
	ID3D11DepthStencilState* m_pDepthStencilState3D = NULL;

	// 繧ｵ繝ｳ繝励Λ繝ｼ繧ｹ繝�繝ｼ繝医�ｮ繝昴う繝ｳ繧ｿ
	ID3D11SamplerState* m_pSamplerState = NULL;

	// 蜷�遞ｮ螳壽焚繝舌ャ繝輔ぃ縺ｮ繝昴う繝ｳ繧ｿ
	ID3D11Buffer* m_pTranslationBuffer = NULL;
	ID3D11Buffer* m_pAngleBuffer = NULL;
	ID3D11Buffer* m_pScaleBuffer = NULL;
	ID3D11Buffer* m_pViewBuffer = NULL;
	ID3D11Buffer* m_pProjectionBuffer = NULL;
	ID3D11Buffer* m_pMaterialBuffer = NULL;
	ID3D11Buffer* m_pLightBuffer = NULL;
	ID3D11Buffer* m_pCameraBuffer = NULL;
	ID3D11Buffer* m_pParameterBuffer = NULL;

	// 繧ｯ繝ｩ繧､繧｢繝ｳ繝医し繧､繧ｺ
	Vector4O m_ClientSize{ SCREEN_WIDTH_DEFAULT,SCREEN_HEIGHT_DEFAULT };

	// 繧ｦ繧｣繝ｳ繝峨え繝上Φ繝峨Ν
	HWND m_Handle;

	// 鬆らせ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｨ繝斐け繧ｻ繝ｫ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｮ繝槭ャ繝�
	std::map<std::string, ID3D11VertexShader*> m_VertexShaders;
	std::map<std::string, ID3D11PixelShader*> m_PixelShaders;
	// 蜈･蜉帙Ξ繧､繧｢繧ｦ繝医�ｮ繝昴う繝ｳ繧ｿ
	ID3D11InputLayout* m_pInputLayout = nullptr;

	// 迴ｾ蝨ｨ菴ｿ逕ｨ荳ｭ縺ｮ繧ｷ繧ｧ繝ｼ繝繝ｼ繧ｭ繝ｼ
	std::string m_CurrentVertexShaderKey;
	std::string m_CurrentPixelShaderKey;

	// 繝�繧ｯ繧ｹ繝√Ε縺ｮ繝吶け繧ｿ繝ｼ
	std::vector<Texture> m_Textures;


	/// @brief 繝ｬ繝ｳ繝繝ｼ繧ｿ繝ｼ繧ｲ繝�繝医ン繝･繝ｼ縺ｮ逕滓��
	void CreateRenderTargetView();

	/// @brief 繝�繝励せ繧ｹ繝�繝ｳ繧ｷ繝ｫ繝薙Η繝ｼ繝ｻ繝�繝励せ繧ｹ繝�繝ｳ繧ｷ繝ｫ逕ｨ繝�繧ｯ繧ｹ繝√Ε繝ｻ繝�繝励せ繧ｹ繝�繝ｳ繧ｷ繝ｫ繧ｿ繝ｼ繧ｲ繝�繝医�ｮ逕滓��
	void CreateDepthStencil();

	/// @brief 繝薙Η繝ｼ繝昴�ｼ繝郁ｨｭ螳�
	void CreateViewPort();

	/// @brief 繝ｩ繧ｹ繧ｿ繝ｩ繧､繧ｶ繧ｹ繝�繝ｼ繝井ｽ懈��
	void CreateRasterizer();

	/// @brief 繝悶Ξ繝ｳ繝峨せ繝�繝ｼ繝郁ｨｭ螳�
	void CreateBlendState();

	/// @brief 豺ｱ蠎ｦ繧ｹ繝�繝ｼ繝井ｽ懈��
	void CreateDepthStencilState();

	/// @brief 繧ｵ繝ｳ繝励Λ繝ｼ繧ｹ繝�繝ｼ繝医ｒ菴懈�舌＠縺ｾ縺吶�
	void CreateSamplerState();


public:
	/// @brief 謖�螳壹＆繧後◆繧ｦ繧｣繝ｳ繝峨え繝上Φ繝峨Ν縺ｫ髢｢騾｣莉倥￠繧峨ｌ縺欒enderer繧ｪ繝悶ず繧ｧ繧ｯ繝医ｒ菴懈�舌＠縺ｾ縺吶�
	/// @param hWnd 謠冗判蟇ｾ雎｡縺ｨ縺ｪ繧九え繧｣繝ｳ繝峨え縺ｮ繝上Φ繝峨Ν縲�
	Renderer(HWND hWnd);
	/// @brief Renderer 繧ｯ繝ｩ繧ｹ縺ｮ繝�繧ｹ繝医Λ繧ｯ繧ｿ縺ｧ縺吶�
	~Renderer();

	/// @brief 繝舌ャ繝輔ぃ縺ｮ蜀�螳ｹ繧偵け繝ｪ繧｢縺励∪縺吶�
	void BufferClear();
	/// @brief 繝舌ャ繝輔ぃ縺ｮ蜀�螳ｹ繧定｡ｨ遉ｺ縺ｾ縺溘�ｯ蜃ｺ蜉帙＠縺ｾ縺吶�
	void BufferPresent();
	/// @brief 繧ｯ繝ｩ繧､繧｢繝ｳ繝磯�伜沺縺ｮ繧ｵ繧､繧ｺ繧呈欠螳壹＠縺溷ｹ�縺ｨ鬮倥＆縺ｫ螟画峩縺励∪縺吶�
	/// @param width 譁ｰ縺励＞繧ｯ繝ｩ繧､繧｢繝ｳ繝磯�伜沺縺ｮ蟷��ｼ医ヴ繧ｯ繧ｻ繝ｫ蜊倅ｽ搾ｼ峨�
	/// @param height 譁ｰ縺励＞繧ｯ繝ｩ繧､繧｢繝ｳ繝磯�伜沺縺ｮ鬮倥＆�ｼ医ヴ繧ｯ繧ｻ繝ｫ蜊倅ｽ搾ｼ峨�
	void ResizeClient(int width, int height);

	/// @brief 繝�繝舌う繧ｹ繧ｪ繝悶ず繧ｧ繧ｯ繝医ｒ蜿門ｾ励＠縺ｾ縺吶�
	/// @return ID3D11Device 繧ｪ繝悶ず繧ｧ繧ｯ繝医∈縺ｮ繝昴う繝ｳ繧ｿ縲�
	ID3D11Device* GetDevice() { return m_pDevice; }
	/// @brief 繝�繝舌う繧ｹ繧ｳ繝ｳ繝�繧ｭ繧ｹ繝医ｒ蜿門ｾ励＠縺ｾ縺吶�
	/// @return ID3D11DeviceContext 繧ｪ繝悶ず繧ｧ繧ｯ繝医∈縺ｮ繝昴う繝ｳ繧ｿ縲�
	ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }
	/// @brief 繧ｹ繝ｯ繝�繝励メ繧ｧ繝ｼ繝ｳ繧ｪ繝悶ず繧ｧ繧ｯ繝医∈縺ｮ繝昴う繝ｳ繧ｿ繧貞叙蠕励＠縺ｾ縺吶�
	/// @return IDXGISwapChain 繧ｪ繝悶ず繧ｧ繧ｯ繝医∈縺ｮ繝昴う繝ｳ繧ｿ縲�
	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
	/// @brief 繝ｬ繝ｳ繝繝ｼ繧ｿ繝ｼ繧ｲ繝�繝医ン繝･繝ｼ縺ｨ繝�繝励せ繧ｹ繝�繝ｳ繧ｷ繝ｫ繝薙Η繝ｼ繧偵ョ繝舌う繧ｹ繧ｳ繝ｳ繝�繧ｭ繧ｹ繝医↓險ｭ螳壹＠縺ｾ縺吶�
	void SetRenderTargetView() { m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView); }
	/// @brief 繧ｯ繝ｩ繧､繧｢繝ｳ繝磯�伜沺縺ｮ繧ｵ繧､繧ｺ繧貞叙蠕励＠縺ｾ縺吶�
	/// @return 繧ｯ繝ｩ繧､繧｢繝ｳ繝磯�伜沺縺ｮ繧ｵ繧､繧ｺ繧定｡ｨ縺� Vector4O 蝙九�ｮ蛟､縲�
	Vector4O GetClientSize() { return m_ClientSize; }

	/// @brief 謖�螳壹＆繧後◆繝輔ぃ繧､繝ｫ蜷阪→繧ｭ繝ｼ縺九ｉ鬆らせ繧ｷ繧ｧ繝ｼ繝繝ｼ繧剃ｽ懈�舌＠縺ｾ縺吶�
	/// @param filename 鬆らせ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｮ繧ｽ繝ｼ繧ｹ繧ｳ繝ｼ繝峨′蜷ｫ縺ｾ繧後ｋ繝輔ぃ繧､繝ｫ縺ｮ蜷榊燕縲�
	/// @param key 繧ｷ繧ｧ繝ｼ繝繝ｼ繧定ｭ伜挨縺ｾ縺溘�ｯ蜿門ｾ励☆繧九◆繧√�ｮ繧ｭ繝ｼ縲�
	/// @return 菴懈�舌＆繧後◆鬆らせ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｮ蜀�螳ｹ繧定｡ｨ縺呎枚蟄怜�励�
	std::string CreateVertexShader(std::string filename, std::string key);
	/// @brief 謖�螳壹＆繧後◆繝輔ぃ繧､繝ｫ蜷阪→繧ｭ繝ｼ縺九ｉ繝斐け繧ｻ繝ｫ繧ｷ繧ｧ繝ｼ繝繝ｼ繧剃ｽ懈�舌＠縺ｾ縺吶�
	/// @param filename 繝斐け繧ｻ繝ｫ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｮ繧ｽ繝ｼ繧ｹ繧ｳ繝ｼ繝峨′蜷ｫ縺ｾ繧後ｋ繝輔ぃ繧､繝ｫ縺ｮ蜷榊燕縲�
	/// @param key 繧ｷ繧ｧ繝ｼ繝繝ｼ繧定ｭ伜挨縺吶ｋ縺溘ａ縺ｮ繧ｭ繝ｼ縲�
	/// @return 菴懈�舌＆繧後◆繝斐け繧ｻ繝ｫ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｮ蜀�螳ｹ繧定｡ｨ縺呎枚蟄怜�励�
	std::string CreatePixelShader(std::string filename, std::string key);
	/// @brief 繧ｷ繧ｧ繝ｼ繝繝ｼ繝舌う繝医さ繝ｼ繝峨°繧迂D3D11InputLayout繧ｪ繝悶ず繧ｧ繧ｯ繝医ｒ菴懈�舌＠縺ｾ縺吶�
	/// @param pByteCode 蜈･蜉帙Ξ繧､繧｢繧ｦ繝医ｒ險倩ｿｰ縺吶ｋ繧ｷ繧ｧ繝ｼ繝繝ｼ繝舌う繝医さ繝ｼ繝峨∈縺ｮ繝昴う繝ｳ繧ｿ縲�
	/// @param byteCodeLength 繝舌う繝医さ繝ｼ繝峨�ｮ髟ｷ縺包ｼ医ヰ繧､繝亥腰菴搾ｼ峨�
	/// @return 菴懈�舌＆繧後◆ID3D11InputLayout繧ｪ繝悶ず繧ｧ繧ｯ繝医∈縺ｮ繝昴う繝ｳ繧ｿ縲ょ､ｱ謨励＠縺溷�ｴ蜷医�ｯnullptr繧定ｿ斐＠縺ｾ縺吶�
	ID3D11InputLayout* CreateInputLayout(unsigned char* pByteCode, long byteCodeLength);
	/// @brief 蜈･蜉帙Ξ繧､繧｢繧ｦ繝医ｒ蜿門ｾ励＠縺ｾ縺吶�
	/// @return 迴ｾ蝨ｨ縺ｮID3D11InputLayout繝昴う繝ｳ繧ｿ縲�
	ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; }
	/// @brief 螳壽焚繝舌ャ繝輔ぃ繧剃ｽ懈�舌＠縺ｾ縺吶�
	void CreateConstantBuffer();

	/// @brief 謖�螳壹＆繧後◆繝輔ぃ繧､繝ｫ蜷阪°繧峨ユ繧ｯ繧ｹ繝√Ε繧定ｪｭ縺ｿ霎ｼ縺ｿ縲！D3D11ShaderResourceView 繝昴う繝ｳ繧ｿ繧定ｿ斐＠縺ｾ縺吶�
	/// @param filename 隱ｭ縺ｿ霎ｼ繧繝�繧ｯ繧ｹ繝√Ε繝輔ぃ繧､繝ｫ縺ｮ繝代せ繧定｡ｨ縺吶Ρ繧､繝画枚蟄怜�励�
	/// @return 隱ｭ縺ｿ霎ｼ縺ｾ繧後◆繝�繧ｯ繧ｹ繝√Ε縺ｮ ID3D11ShaderResourceView 繝昴う繝ｳ繧ｿ縲ょ､ｱ謨励＠縺溷�ｴ蜷医�ｯ nullptr 繧定ｿ斐☆縺薙→縺後≠繧翫∪縺吶�
	ID3D11ShaderResourceView* TextureLoad(const std::wstring& filename);
	/// @brief 謖�螳壹＆繧後◆繧､繝ｳ繝�繝�繧ｯ繧ｹ縺ｮ繝�繧ｯ繧ｹ繝√Ε繝ｪ繧ｽ繝ｼ繧ｹ繝薙Η繝ｼ繧貞叙蠕励＠縺ｾ縺吶�
	/// @param index 蜿門ｾ励☆繧九ユ繧ｯ繧ｹ繝√Ε縺ｮ繧､繝ｳ繝�繝�繧ｯ繧ｹ縲�
	/// @return 謖�螳壹＆繧後◆繧､繝ｳ繝�繝�繧ｯ繧ｹ縺ｫ蟇ｾ蠢懊☆繧紀D3D11ShaderResourceView繝昴う繝ｳ繧ｿ縲りｩｲ蠖薙☆繧九ユ繧ｯ繧ｹ繝√Ε縺悟ｭ伜惠縺励↑縺�蝣ｴ蜷医�ｯnullptr繧定ｿ斐☆縺薙→縺後≠繧翫∪縺吶�
	ID3D11ShaderResourceView* GetTexture(int index);
	/// @brief 謖�螳壹＆繧後◆繧､繝ｳ繝�繝�繧ｯ繧ｹ縺ｮ繝�繧ｯ繧ｹ繝√Ε縺ｮ蟷�繧貞叙蠕励＠縺ｾ縺吶�
	/// @param index 蟷�繧貞叙蠕励☆繧九ユ繧ｯ繧ｹ繝√Ε縺ｮ繧､繝ｳ繝�繝�繧ｯ繧ｹ縲�
	/// @return 繝�繧ｯ繧ｹ繝√Ε縺ｮ蟷��ｼ医ヴ繧ｯ繧ｻ繝ｫ蜊倅ｽ搾ｼ峨�
	int GetTextureWidth(int index);
	/// @brief 謖�螳壹＆繧後◆繧､繝ｳ繝�繝�繧ｯ繧ｹ縺ｮ繝�繧ｯ繧ｹ繝√Ε縺ｮ鬮倥＆繧貞叙蠕励＠縺ｾ縺吶�
	/// @param index 鬮倥＆繧貞叙蠕励☆繧九ユ繧ｯ繧ｹ繝√Ε縺ｮ繧､繝ｳ繝�繝�繧ｯ繧ｹ縲�
	/// @return 繝�繧ｯ繧ｹ繝√Ε縺ｮ鬮倥＆�ｼ医ヴ繧ｯ繧ｻ繝ｫ蜊倅ｽ搾ｼ峨�
	int GetTextureHeight(int index);

	/// @brief 謖�螳壹＆繧後◆繧ｭ繝ｼ縺ｫ蟇ｾ蠢懊☆繧矩�らせ繧ｷ繧ｧ繝ｼ繝繝ｼ繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param key 險ｭ螳壹☆繧矩�らせ繧ｷ繧ｧ繝ｼ繝繝ｼ繧定ｭ伜挨縺吶ｋ縺溘ａ縺ｮ繧ｭ繝ｼ縲�
	void SetVertexShader(std::string key);
	/// @brief 謖�螳壹＆繧後◆繧ｭ繝ｼ縺ｫ蟇ｾ蠢懊☆繧九ヴ繧ｯ繧ｻ繝ｫ繧ｷ繧ｧ繝ｼ繝繝ｼ繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param key 險ｭ螳壹☆繧九ヴ繧ｯ繧ｻ繝ｫ繧ｷ繧ｧ繝ｼ繝繝ｼ繧定ｭ伜挨縺吶ｋ譁�蟄怜�励く繝ｼ縲�
	void SetPixelShader(std::string key);

	/// @brief 繝ｬ繝ｳ繝繝ｼ繧ｿ繝ｼ繧ｲ繝�繝医ン繝･繝ｼ繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param renderTargetView 險ｭ螳壹☆繧紀D3D11RenderTargetView縺ｸ縺ｮ繝昴う繝ｳ繧ｿ縲�
	void SetRenderTargetView(ID3D11RenderTargetView* renderTargetView)
	{
		// renderTargetView縺ｯ縺吶〒縺ｫ繝昴う繝ｳ繧ｿ縺ｪ縺ｮ縺ｧ縲√◎縺ｮ繧｢繝峨Ξ繧ｹ繧呈ｸ｡縺�
		m_pDeviceContext->OMSetRenderTargets(1, &renderTargetView, m_pDepthStencilView);
	}

	/// @brief 謖�螳壹＠縺溘せ繝ｭ繝�繝医↓繧ｷ繧ｧ繝ｼ繝繝ｼ繝ｪ繧ｽ繝ｼ繧ｹ繝薙Η繝ｼ繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param shaderResourceView 險ｭ螳壹☆繧紀D3D11ShaderResourceView縺ｸ縺ｮ繝昴う繝ｳ繧ｿ縲�
	/// @param slot 繝ｪ繧ｽ繝ｼ繧ｹ繝薙Η繝ｼ繧偵ヰ繧､繝ｳ繝峨☆繧九せ繝ｭ繝�繝育分蜿ｷ縲ゅョ繝輔か繝ｫ繝医�ｯ0縺ｧ縺吶�
	void SetShaderResourceView(ID3D11ShaderResourceView* shaderResourceView, UINT slot = 0)
	{
		m_pDeviceContext->PSSetShaderResources(slot, 1, &shaderResourceView);
	}

	/// @brief 2D逕ｨ縺ｮ繝ｯ繝ｼ繝ｫ繝峨�ｻ繝薙Η繝ｼ繝ｻ繝励Ο繧ｸ繧ｧ繧ｯ繧ｷ繝ｧ繝ｳ陦悟�励ｒ險ｭ螳壹＠縺ｾ縺吶�
	void SetWorldViewProjection2D();
	/// @brief 3D繝ｯ繝ｼ繝ｫ繝峨ン繝･繝ｼ蟆�蠖ｱ陦悟�励ｒ繝ｪ繧ｻ繝�繝医＠縺ｾ縺吶�
	void ResetWorldViewProjection3D();

    void SetRasterizerState3D();
    void SetRasterizerState2D();

	void SetTranslationMatrix(XMMATRIX translation);
	void SetAngleMatrix(XMMATRIX angle);
	void SetScaleMatrix(XMMATRIX scale);
	/// @brief 繝薙Η繝ｼ陦悟�励ｒ險ｭ螳壹＠縺ｾ縺吶�
	/// @param view 險ｭ螳壹☆繧九ン繝･繝ｼ陦悟�励�
	void SetViewMatrix(XMMATRIX view);
	/// @brief 蟆�蠖ｱ陦悟�励ｒ險ｭ螳壹＠縺ｾ縺吶�
	/// @param projection 險ｭ螳壹☆繧句ｰ�蠖ｱ陦悟�励�
	void SetProjectionMatrix(XMMATRIX projection);
	/// @brief 繝槭ユ繝ｪ繧｢繝ｫ繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param material 險ｭ螳壹☆繧貴ATERIAL蝙九�ｮ繝槭ユ繝ｪ繧｢繝ｫ縲�
	void SetMaterial(MATERIAL material);
	/// @brief 繝ｩ繧､繝医ｒ險ｭ螳壹＠縺ｾ縺吶�
	/// @param light 險ｭ螳壹☆繧九Λ繧､繝医�
	void SetLight(LIGHT light);
	/// @brief 繧ｫ繝｡繝ｩ縺ｮ菴咲ｽｮ繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param position 繧ｫ繝｡繝ｩ縺ｮ譁ｰ縺励＞菴咲ｽｮ繧定｡ｨ縺� Vector4O 蝙九�ｮ蛟､縲�
	void SetCamera(Vector4O position);
	/// @brief 菴咲ｽｮ繝代Λ繝｡繝ｼ繧ｿ繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param position 險ｭ螳壹☆繧�4谺｡蜈�繝吶け繝医Ν縺ｮ菴咲ｽｮ縲�
	void SetParameter(Vector4O position);

};

