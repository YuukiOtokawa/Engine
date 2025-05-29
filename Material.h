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
// クラス定義
//==========================================================================

class Material
{
private:
	// シェーダーのキー
	std::string m_VertexShader;
	std::string m_PixelShader;

	// マテリアルと光源の情報
	MATERIAL m_Material;
	LIGHT m_Light;
	// テクスチャのリソースビュー
	ID3D11ShaderResourceView* m_Texture;

public:
	/// @brief Material クラスのデフォルトコンストラクタです。
	Material() = default;
	/// @brief Material クラスのデストラクタです。
	~Material() = default;

	/// @brief マテリアル情報をシェーダーに送信
	void DrawMaterial();

	/// @brief このマテリアルに頂点シェーダーのキーを設定します。
	/// @param key 設定する頂点シェーダーのキー。
	void SetVertexShaderKey(std::string key);
	/// @brief このマテリアルにピクセルシェーダーのキーを設定します。
	/// @param key 設定するピクセルシェーダーのキー。
	void SetPixelShaderKey(std::string key);

	/// @brief マテリアルを設定します。
	/// @param material 設定するマテリアル。
	void SetMaterial(MATERIAL material) { m_Material = material; }

	/// @brief 光源情報を設定します。
	void SetLight(LIGHT light) { m_Light = light; }

	/// @brief テクスチャを設定します。
	/// @param texture 設定するID3D11ShaderResourceView型のテクスチャ。
	void SetTexture(ID3D11ShaderResourceView* texture) { m_Texture = texture; }

	/// @brief このマテリアルの頂点シェーダーのキーを取得します。
	/// @return このマテリアルに格納されている頂点シェーダーのキー。
	std::string GetVertexShader() const { return m_VertexShader; }
	/// @brief このマテリアルのピクセルシェーダーのキーを取得します。
	/// @return このマテリアルに格納されているピクセルシェーダーのキー。
	std::string GetPixelShader() const { return m_PixelShader; }

	/// @brief 現在のオブジェクトのマテリアルを取得します。
	/// @return このオブジェクトに関連付けられているMATERIAL型のマテリアル。
	MATERIAL GetMaterial() const { return m_Material; }
	/// @brief ライト情報を取得します。
	/// @return 現在のライト情報（LIGHT型）。
	LIGHT GetLight() const { return m_Light; }
	/// @brief テクスチャリソースビューを取得します。
	/// @return テクスチャを表す ID3D11ShaderResourceView ポインタ。
	ID3D11ShaderResourceView* GetTexture() const { return m_Texture; }
};

