// ========================================================
//
// マテリアルクラス[Material.h]
// 
//									Date:	20250520
//									Author:	Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include <d3d11.h>

#include "FrameWork.h"

//==========================================================================
// クラス
//==========================================================================

class Material
{
private:
	// シェーダーキー
	std::string m_VertexShader;
	std::string m_PixelShader;

	// マテリアル
	MATERIAL m_Material;
	LIGHT m_Light;
	// テクスチャ
	ID3D11ShaderResourceView* m_Texture;

	unsigned int	StartIndex;
	unsigned int	IndexNum;

public:
	/// @brief Material クラスのデフォルトコンストラクタです。
	Material() = default;
	/// @brief Material クラスのデストラクタです。
	~Material() = default;

	/// @brief マテリアルを描画します。
	void DrawMaterial();

	void DrawGUI();

	/// @brief 頂点シェーダーのキーを設定します。
	/// @param key 設定する頂点シェーダーのキー。
	void SetVertexShaderKey(std::string key);
	/// @brief ピクセルシェーダーのキーを設定します。
	/// @param key 設定するピクセルシェーダーのキー。
	void SetPixelShaderKey(std::string key);

	void SetShader();

	/// @brief マテリアルを設定します。
	/// @param material 設定するMATERIAL型のマテリアル。
	void SetMaterial(MATERIAL material) { m_Material = material; }

	/// @brief LIGHT型の値を設定します。
	/// @param light 設定するLIGHT型の値。
	void SetLight(LIGHT light) { m_Light = light; }

	/// @brief テクスチャを設定します。
	/// @param texture 設定するID3D11ShaderResourceView型のテクスチャ。
	void SetTexture(ID3D11ShaderResourceView* texture) { m_Texture = texture; }

	/// @brief 頂点シェーダーの文字列を取得します。
	/// @return 格納されている頂点シェーダーの文字列。
	std::string GetVertexShader() const { return m_VertexShader; }
	/// @brief ピクセルシェーダーの文字列を取得します。
	/// @return ピクセルシェーダーを表す std::string 型の値。
	std::string GetPixelShader() const { return m_PixelShader; }

	void SetStartIndex(unsigned int startIndex) { StartIndex = startIndex; }
	void SetIndexNum(unsigned int indexNum) { IndexNum = indexNum; }

	unsigned int GetStartIndex() const { return StartIndex; }
	unsigned int GetIndexNum() const { return IndexNum; }

	void SetTextureEnable(bool enable) { m_Material.textureEnable = enable; }

	/// @brief 現在のオブジェクトのマテリアルを取得します。
	/// @return このオブジェクトに関連付けられているMATERIAL型のマテリアル。
	MATERIAL GetMaterial() const { return m_Material; }
	/// @brief ライト情報を取得します。
	/// @return メンバー変数 m_Light の値を返します。
	LIGHT GetLight() const { return m_Light; }
	/// @brief テクスチャリソースビューを取得します。
	/// @return テクスチャを表す ID3D11ShaderResourceView ポインタ。
	ID3D11ShaderResourceView* GetTexture() const { return m_Texture; }
};

