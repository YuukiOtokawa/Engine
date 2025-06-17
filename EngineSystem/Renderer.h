// ========================================================
//
// レンダラークラス[Renderer.h]
// 
//									Date:20250514
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "FrameWork.h"

#include <d3d11.h>
#include "DirectXTex.h"

#include <map>

#include "Vector4O.h"

#include <vector>

//==========================================================================
// クラス定義
//==========================================================================

class Renderer
{
private:
	// テクスチャの構造体定義
	struct Texture {
		std::wstring filename;
		ID3D11ShaderResourceView* shader_resource_view;
		int width;
		int height;
	};

	// DirectX 11関係のポインタ
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11Texture2D* m_pDepthStencilTexture;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_pDepthStencilView;

	// ラスタライザステートのポインタ
	ID3D11RasterizerState* m_pRasterizerState = NULL;
	// ブレンドステートのポインタ
	ID3D11BlendState* m_pBlendState = NULL;
	ID3D11BlendState* m_pBlendStateAdd = NULL;

	// 深度ステンシルステートのポインタ
	ID3D11DepthStencilState* m_pDepthStencilStateDepthEnable = NULL;
	ID3D11DepthStencilState* m_pDepthStencilStateDepthDisable = NULL;
	ID3D11DepthStencilState* m_pDepthStencilStateShadow = NULL;
	ID3D11DepthStencilState* m_pDepthStencilStateBackGround = NULL;
	ID3D11DepthStencilState* m_pDepthStencilState3D = NULL;

	// サンプラーステートのポインタ
	ID3D11SamplerState* m_pSamplerState = NULL;

	// 各種定数バッファのポインタ
	ID3D11Buffer* m_pTranslationBuffer = NULL;
	ID3D11Buffer* m_pAngleBuffer = NULL;
	ID3D11Buffer* m_pScaleBuffer = NULL;
	ID3D11Buffer* m_pViewBuffer = NULL;
	ID3D11Buffer* m_pProjectionBuffer = NULL;
	ID3D11Buffer* m_pMaterialBuffer = NULL;
	ID3D11Buffer* m_pLightBuffer = NULL;
	ID3D11Buffer* m_pCameraBuffer = NULL;
	ID3D11Buffer* m_pParameterBuffer = NULL;

	// クライアントサイズ
	Vector4O m_ClientSize{ SCREEN_WIDTH_DEFAULT,SCREEN_HEIGHT_DEFAULT };

	// ウィンドウハンドル
	HWND m_Handle;

	// 頂点シェーダーとピクセルシェーダーのマップ
	std::map<std::string, ID3D11VertexShader*> m_VertexShaders;
	std::map<std::string, ID3D11PixelShader*> m_PixelShaders;
	// 入力レイアウトのポインタ
	ID3D11InputLayout* m_pInputLayout = nullptr;

	// 現在使用中のシェーダーキー
	std::string m_CurrentVertexShaderKey;
	std::string m_CurrentPixelShaderKey;

	// テクスチャのベクター
	std::vector<Texture> m_Textures;


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

	/// @brief サンプラーステートを作成します。
	void CreateSamplerState();


public:
	/// @brief 指定されたウィンドウハンドルに関連付けられたRendererオブジェクトを作成します。
	/// @param hWnd 描画対象となるウィンドウのハンドル。
	Renderer(HWND hWnd);
	/// @brief Renderer クラスのデストラクタです。
	~Renderer();

	/// @brief バッファの内容をクリアします。
	void BufferClear();
	/// @brief バッファの内容を表示または出力します。
	void BufferPresent();
	/// @brief クライアント領域のサイズを指定した幅と高さに変更します。
	/// @param width 新しいクライアント領域の幅（ピクセル単位）。
	/// @param height 新しいクライアント領域の高さ（ピクセル単位）。
	void ResizeClient(int width, int height);

	/// @brief デバイスオブジェクトを取得します。
	/// @return ID3D11Device オブジェクトへのポインタ。
	ID3D11Device* GetDevice() { return m_pDevice; }
	/// @brief デバイスコンテキストを取得します。
	/// @return ID3D11DeviceContext オブジェクトへのポインタ。
	ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }
	/// @brief スワップチェーンオブジェクトへのポインタを取得します。
	/// @return IDXGISwapChain オブジェクトへのポインタ。
	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
	/// @brief レンダーターゲットビューとデプスステンシルビューをデバイスコンテキストに設定します。
	void SetRenderTargetView() { m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView); }
	/// @brief クライアント領域のサイズを取得します。
	/// @return クライアント領域のサイズを表す Vector4O 型の値。
	Vector4O GetClientSize() { return m_ClientSize; }

	/// @brief 指定されたファイル名とキーから頂点シェーダーを作成します。
	/// @param filename 頂点シェーダーのソースコードが含まれるファイルの名前。
	/// @param key シェーダーを識別または取得するためのキー。
	/// @return 作成された頂点シェーダーの内容を表す文字列。
	std::string CreateVertexShader(std::string filename, std::string key);
	/// @brief 指定されたファイル名とキーからピクセルシェーダーを作成します。
	/// @param filename ピクセルシェーダーのソースコードが含まれるファイルの名前。
	/// @param key シェーダーを識別するためのキー。
	/// @return 作成されたピクセルシェーダーの内容を表す文字列。
	std::string CreatePixelShader(std::string filename, std::string key);
	/// @brief シェーダーバイトコードからID3D11InputLayoutオブジェクトを作成します。
	/// @param pByteCode 入力レイアウトを記述するシェーダーバイトコードへのポインタ。
	/// @param byteCodeLength バイトコードの長さ（バイト単位）。
	/// @return 作成されたID3D11InputLayoutオブジェクトへのポインタ。失敗した場合はnullptrを返します。
	ID3D11InputLayout* CreateInputLayout(unsigned char* pByteCode, long byteCodeLength);
	/// @brief 入力レイアウトを取得します。
	/// @return 現在のID3D11InputLayoutポインタ。
	ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; }
	/// @brief 定数バッファを作成します。
	void CreateConstantBuffer();

	/// @brief 指定されたファイル名からテクスチャを読み込み、ID3D11ShaderResourceView ポインタを返します。
	/// @param filename 読み込むテクスチャファイルのパスを表すワイド文字列。
	/// @return 読み込まれたテクスチャの ID3D11ShaderResourceView ポインタ。失敗した場合は nullptr を返すことがあります。
	ID3D11ShaderResourceView* TextureLoad(const std::wstring& filename);
	/// @brief 指定されたインデックスのテクスチャリソースビューを取得します。
	/// @param index 取得するテクスチャのインデックス。
	/// @return 指定されたインデックスに対応するID3D11ShaderResourceViewポインタ。該当するテクスチャが存在しない場合はnullptrを返すことがあります。
	ID3D11ShaderResourceView* GetTexture(int index);
	/// @brief 指定されたインデックスのテクスチャの幅を取得します。
	/// @param index 幅を取得するテクスチャのインデックス。
	/// @return テクスチャの幅（ピクセル単位）。
	int GetTextureWidth(int index);
	/// @brief 指定されたインデックスのテクスチャの高さを取得します。
	/// @param index 高さを取得するテクスチャのインデックス。
	/// @return テクスチャの高さ（ピクセル単位）。
	int GetTextureHeight(int index);

	/// @brief 指定されたキーに対応する頂点シェーダーを設定します。
	/// @param key 設定する頂点シェーダーを識別するためのキー。
	void SetVertexShader(std::string key);
	/// @brief 指定されたキーに対応するピクセルシェーダーを設定します。
	/// @param key 設定するピクセルシェーダーを識別する文字列キー。
	void SetPixelShader(std::string key);

	/// @brief レンダーターゲットビューを設定します。
	/// @param renderTargetView 設定するID3D11RenderTargetViewへのポインタ。
	void SetRenderTargetView(ID3D11RenderTargetView* renderTargetView)
	{
		// renderTargetViewはすでにポインタなので、そのアドレスを渡す
		m_pDeviceContext->OMSetRenderTargets(1, &renderTargetView, m_pDepthStencilView);
	}

	/// @brief 指定したスロットにシェーダーリソースビューを設定します。
	/// @param shaderResourceView 設定するID3D11ShaderResourceViewへのポインタ。
	/// @param slot リソースビューをバインドするスロット番号。デフォルトは0です。
	void SetShaderResourceView(ID3D11ShaderResourceView* shaderResourceView, UINT slot = 0)
	{
		m_pDeviceContext->PSSetShaderResources(slot, 1, &shaderResourceView);
	}

	/// @brief 2D用のワールド・ビュー・プロジェクション行列を設定します。
	void SetWorldViewProjection2D();
	/// @brief 3Dワールドビュー射影行列をリセットします。
	void ResetWorldViewProjection3D();
	void SetTranslationMatrix(XMMATRIX translation);
	void SetAngleMatrix(XMMATRIX angle);
	void SetScaleMatrix(XMMATRIX scale);
	/// @brief ビュー行列を設定します。
	/// @param view 設定するビュー行列。
	void SetViewMatrix(XMMATRIX view);
	/// @brief 射影行列を設定します。
	/// @param projection 設定する射影行列。
	void SetProjectionMatrix(XMMATRIX projection);
	/// @brief マテリアルを設定します。
	/// @param material 設定するMATERIAL型のマテリアル。
	void SetMaterial(MATERIAL material);
	/// @brief ライトを設定します。
	/// @param light 設定するライト。
	void SetLight(LIGHT light);
	/// @brief カメラの位置を設定します。
	/// @param position カメラの新しい位置を表す Vector4O 型の値。
	void SetCamera(Vector4O position);
	/// @brief 位置パラメータを設定します。
	/// @param position 設定する4次元ベクトルの位置。
	void SetParameter(Vector4O position);

};

