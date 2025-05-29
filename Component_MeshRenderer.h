// ========================================================
//
// メッシュレンダラーコンポーネント基底クラス[Component_MeshRenderer.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component.h"

#include "Material.h"

//==========================================================================
// クラス定義
//==========================================================================

class MeshRenderer :
    public Component
{
private:
	// マテリアルポインタ
	Material* m_pMaterial = nullptr;

public:
	/// @brief MeshRenderer クラスの新しいインスタンスを作成します。
	MeshRenderer();
	/// @brief MeshRenderer のデストラクタです。
	~MeshRenderer() = default;

    /// @brief オブジェクトの状態を更新します。
    void Update() override;
	/// @brief 描画処理を実行します。
	void Draw() override;

	/// @brief マテリアルを設定します。
	/// @param material 設定するマテリアル。
	void SetMaterial(MATERIAL material) { this->m_pMaterial->SetMaterial(material); }
	/// @brief LIGHT型の光源情報を設定します。
	/// @param light 設定するLIGHT型の光源情報。
	void SetLight(LIGHT light) { 
		this->m_pMaterial->SetLight(light);
	}

	/// @brief 頂点シェーダーを指定されたキーで設定します。
	/// @param key 設定する頂点シェーダーのキー。
	void SetVertexShader(std::string key) {
		this->m_pMaterial->SetVertexShaderKey(key);
	}
	/// @brief ピクセルシェーダーを指定されたキーで設定します。
	/// @param key 設定するピクセルシェーダーのキー。
	void SetPixelShader(std::string key) {
		this->m_pMaterial->SetPixelShaderKey(key);
	}

	/// @brief テクスチャを設定します。
	/// @param texture 設定するID3D11ShaderResourceView型のテクスチャ。
	void SetTexture(ID3D11ShaderResourceView* texture) { m_pMaterial->SetTexture(texture); }

	/// @brief コンポーネントに対応したタグを所有者のオブジェクトに設定します。
	void InitializeTag() override;
};

