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

#include <list>
#include <map>

#include "VectorO.h"

#include <vector>
#include <string>

#include "Component_Light.h"

//==========================================================================
// クラス定義
//==========================================================================
// テクスチャ構造体
class Texture : public EngineMetaFile {
public:
    Texture() : EngineMetaFile() {}
    std::wstring filename;
    ID3D11ShaderResourceView* shaderResourceView;
    ID3D11ShaderResourceView* mipMap;
    int width = 0;
    int height = 0;
    bool toExport = true;

    std::string GetFileName() {
        return std::string(filename.begin(), filename.end());
    }
};

class RenderCore
{
private:


    // DirectX 11 関連のポインタ
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11Texture2D* m_pDepthStencilTexture = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_pDepthStencilView;

	// ラスタライザステート
	ID3D11RasterizerState* m_pRasterizerState = NULL;
    ID3D11RasterizerState* m_pRasterizerState2D = NULL;
	// ブレンドステート
	ID3D11BlendState* m_pBlendState = NULL;
	ID3D11BlendState* m_pBlendStateAdd = NULL;

    // 深度ステンシルステート
	ID3D11DepthStencilState* m_pDepthStencilStateDepthEnable = NULL;
	ID3D11DepthStencilState* m_pDepthStencilStateDepthDisable = NULL;
	ID3D11DepthStencilState* m_pDepthStencilStateShadow = NULL;
	ID3D11DepthStencilState* m_pDepthStencilStateBackGround = NULL;
	ID3D11DepthStencilState* m_pDepthStencilState3D = NULL;

	// サンプラーステート
	ID3D11SamplerState* m_pSamplerState = NULL;

	// 各種定数バッファ
	ID3D11Buffer* m_pTranslationBuffer = NULL;
	ID3D11Buffer* m_pAngleBuffer = NULL;
	ID3D11Buffer* m_pScaleBuffer = NULL;
	ID3D11Buffer* m_pViewBuffer = NULL;
	ID3D11Buffer* m_pProjectionBuffer = NULL;
	ID3D11Buffer* m_pLightBuffer = NULL;
	ID3D11Buffer* m_pCameraBuffer = NULL;
	ID3D11Buffer* m_pParameterBuffer = NULL;
    ID3D11Buffer* m_pMaterialBuffer = NULL;

    ID3D11Buffer* m_pWeightBuffer = NULL;

	// 描画領域サイズ
	Vector2O m_ClientSize{ SCREEN_WIDTH_DEFAULT,SCREEN_HEIGHT_DEFAULT };

	// ウィンドウハンドル
	HWND m_Handle;

    // シェーダー管理
	std::map<std::string, ID3D11VertexShader*> m_VertexShaders;
	std::map<std::string, ID3D11PixelShader*> m_PixelShaders;
	// 入力レイアウト
	ID3D11InputLayout* m_pInputLayout = nullptr;

    // 使用中のシェーダーキー
	std::string m_CurrentVertexShaderKey;
	std::string m_CurrentPixelShaderKey;

    // テクスチャ管理
	std::vector<Texture*> m_Textures;

    // ポストプロセス用レンダーテクスチャ
    ID3D11RenderTargetView* m_pPostProcessRTV[3] = {};
    ID3D11ShaderResourceView* m_pPostProcessSRV[3] = {};
    Texture* m_PostProcessTexture[3] = {};

    ID3D11RenderTargetView* m_pSceneViewRTV = {};
    ID3D11ShaderResourceView* m_pSceneViewSRV = {};
    Texture* m_pSceneViewTexture = {};
    ID3D11RenderTargetView* m_pGameViewRTV = {};
    ID3D11ShaderResourceView* m_pGameViewSRV = {};
    Texture* m_pGameViewTexture = {};

	/// @brief レンダーターゲットビューを作成します。
	void CreateRenderTargetView();

    /// @brief 深度ステンシルを作成します。
    void CreateDepthStencil();

	/// @brief ビューポートを作成します。
	void CreateViewPort();

	/// @brief ラスタライザーを作成します。
	void CreateRasterizer();

	/// @brief ブレンドステートを作成します。
	void CreateBlendState();

	/// @brief 深度ステンシルステートを作成します。
	void CreateDepthStencilState();

	/// @brief サンプラーステートを作成します。
	void CreateSamplerState();


public:
	/// @brief 指定されたウィンドウハンドルでRendererオブジェクトを初期化します。
	/// @param hWnd 描画対象となるウィンドウのハンドル。
	RenderCore(HWND hWnd);
	/// @brief Renderer クラスのデストラクタです。
	~RenderCore();

	/// @brief バッファの内容をすべてクリアします。
	void BufferClear();
	/// @brief バッファの内容を表示または出力します。
	void BufferPresent();

    void BeginPE(int n);

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
	/// @brief スワップチェーンオブジェクトを取得します。
	/// @return IDXGISwapChain オブジェクトへのポインタ。
	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
	/// @brief レンダーターゲットビューをデバイスコンテキストに設定します。
	void SetRenderTargetView() { m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView); }
	/// @brief クライアント領域のサイズを取得します。
	/// @return クライアント領域のサイズを表す Vector4O 型の値。
	Vector2O GetClientSize() {
        return m_ClientSize;
    }

    std::vector<Texture*> GetTextureInfo();

	/// @brief 指定されたファイル名とキーから頂点シェーダーを作成します。
	/// @param filename 頂点シェーダーのソースコードが含まれるファイルの名前。
	/// @param key シェーダーを識別または取得するためのキー。
	/// @return 作成された頂点シェーダーのキー。
	std::string CreateVertexShader(std::string filename, std::string key);
	/// @brief 指定されたファイル名とキーからピクセルシェーダーを作成します。
	/// @param filename ピクセルシェーダーのソースコードが含まれるファイルの名前。
	/// @param key シェーダーを識別するためのキー。
	/// @return 作成されたピクセルシェーダーの情報を含む文字列。
	std::string CreatePixelShader(std::string filename, std::string key);
	/// @brief シェーダーバイトコードからID3D11InputLayoutオブジェクトを作成します。
	/// @param pByteCode 入力レイアウトを記述するシェーダーバイトコードへのポインタ。
	/// @param byteCodeLength バイトコードの長さ（バイト単位）。
	/// @return 作成されたID3D11InputLayoutオブジェクトへのポインタ。失敗した場合はnullptrを返します。
	ID3D11InputLayout* CreateInputLayout(unsigned char* pByteCode, long byteCodeLength);
	/// @brief 入力レイアウトオブジェクトを取得します。
	/// @return 格納されている ID3D11InputLayout ポインタ。
	ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; }
	/// @brief 定数バッファを作成します。
	void CreateConstantBuffer();

	/// @brief 指定されたファイル名からテクスチャを読み込み、ID3D11ShaderResourceView ポインタを返します。
	/// @param filename 読み込むテクスチャファイルのパスを表すワイド文字列。
	/// @return 読み込まれたテクスチャの ID3D11ShaderResourceView ポインタ。失敗した場合は nullptr を返すことがあります。
	int TextureLoad(const std::wstring& filename, int fileID = -1);
    int AddTexture(Texture* texture);

    void ResetTexture();
	/// @brief 指定されたインデックスのテクスチャリソースビューを取得します。
	/// @param index 取得するテクスチャのインデックス。
	/// @return 指定したインデックスに対応するID3D11ShaderResourceViewポインタ。該当するテクスチャが存在しない場合はnullptrを返すことがあります。
	ID3D11ShaderResourceView** GetTexture(int fileID);
    std::string GetTextureFileName(int fileID) {
        for (auto texture : m_Textures) {
            if (texture->GetFileID() == fileID) {
                return std::string(texture->filename.begin(), texture->filename.end());
            }
        }
        return "";
    }

	/// @brief 指定されたインデックスのテクスチャの幅を取得します。
	/// @param index 幅を取得するテクスチャのインデックス。
	/// @return テクスチャの幅（ピクセル単位）。
	int GetTextureWidth(int index);
	/// @brief 指定されたインデックスのテクスチャの高さを取得します。
	/// @param index 高さを取得するテクスチャのインデックス。
	/// @return 指定したテクスチャの高さ（ピクセル単位）。
	int GetTextureHeight(int index);

	/// @brief 指定されたキーに対応する頂点シェーダーを設定します。
	/// @param key 設定する頂点シェーダーを識別する文字列キー。
	void SetVertexShader(std::string key);
	/// @brief 指定されたキーに対応するピクセルシェーダーを設定します。
	/// @param key 設定するピクセルシェーダーを識別するためのキー。
	void SetPixelShader(std::string key);

    std::vector<std::string> GetVertexShaderKeys() {
        std::vector<std::string> keys;
        for (const auto& shader : m_VertexShaders) {
            keys.push_back(shader.first);
        }
        return keys;
    }

    std::vector<std::string> GetPixelShaderKeys() {
        std::vector<std::string> keys;
        for (const auto& shader : m_PixelShaders) {
            keys.push_back(shader.first);
        }
        return keys;
    }

	/// @brief レンダーターゲットビューを設定します。
	/// @param renderTargetView 設定するID3D11RenderTargetViewへのポインタ。
	void SetRenderTargetView(ID3D11RenderTargetView* renderTargetView)
	{
        // レンダーターゲットビューを設定
		m_pDeviceContext->OMSetRenderTargets(1, &renderTargetView, m_pDepthStencilView);
	}

	/// @brief シェーダーリソースビューを指定したスロットに設定します。
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

    void SetRasterizerState3D();
    void SetRasterizerState2D();

	void SetTranslationMatrix(XMMATRIX translation);
	void SetAngleMatrix(XMMATRIX angle);
	void SetScaleMatrix(XMMATRIX scale);
	/// @brief ビュー行列を設定します。
	/// @param view 設定するビュー行列。
	void SetViewMatrix(XMMATRIX view);
	/// @brief 射影行列を設定します。
	/// @param projection 設定する射影行列。
	void SetProjectionMatrix(XMMATRIX projection);
	/// @brief ライトを設定します。
	/// @param light 設定するライト。
	void SetLight(LIGHT_BUFFER light);
	/// @brief カメラの位置を設定します。
	/// @param position カメラの新しい位置を表す Vector4O 型の値。
	void SetCamera(Vector4O position);
	/// @brief 位置パラメータを設定します。
	/// @param position 設定する位置を表す Vector4O 型の値。
	void SetParameter(Vector4O position);

    void SetLightBuffer(LIGHT_BUFFER lightBuffer) {
        m_pDeviceContext->UpdateSubresource(m_pLightBuffer, 0, NULL, &lightBuffer, 0, 0);
    }

    void SetMaterialBuffer(MATERIAL material) {
        m_pDeviceContext->UpdateSubresource(m_pMaterialBuffer, 0, NULL, &material, 0, 0);
    }

    void ResetRenderTarget() {
        m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
    }

    void ResetViewPort() {
        CreateViewPort();
    }

    Texture* GetPostProcessTexture(int n = 0) {
        return m_PostProcessTexture[n];
    }

    void SetWeight(float* weight);
    void CreatePostProcessBuffer();
    void CreateSceneGameViewBuffer();

    void BeginSceneView();
    void BeginGameView();

    Texture* GetSceneViewTexture() {
        return m_pSceneViewTexture;
    }
    Texture* GetGameViewTexture() {
        return m_pGameViewTexture;
    }

};

