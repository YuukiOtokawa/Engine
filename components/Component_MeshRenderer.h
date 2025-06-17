// ========================================================
//
// メッシュレンダラーコンポーネントクラス[Component_MeshRenderer.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component.h"

#include "../EngineSystem/Material.h"

//==========================================================================
// クラス定義
//==========================================================================

class MeshRenderer :
    public Component
{
private:
	// マテリアルデータ
	Material* m_pMaterial = nullptr;
	int m_MaterialNum = 1;

public:
	/// @brief MeshRenderer クラスの新しいインスタンスを作成します。
	MeshRenderer();
	/// @brief MeshRenderer オブジェクトのデストラクタです。
	~MeshRenderer() = default;

    /// @brief コンポーネントの状態を更新します。
    void UpdateComponent() override;
	/// @brief 描画処理を実行します。
	void Draw() override;
	void DrawGUI() override;

	/// @brief マテリアルを設定します。
	/// @param material 設定するマテリアル。
	void SetMaterial(MATERIAL material) { this->m_pMaterial->SetMaterial(material); }
	/// @brief LIGHT オブジェクトを設定します。
	/// @param light 設定する LIGHT オブジェクト。
	void SetLight(LIGHT light) { 
		this->m_pMaterial->SetLight(light);
	}

	void SetMaterial(Material* material) { this->m_pMaterial = material; }
	void SetMaterialNum(int num) { this->m_MaterialNum = num; }

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
	void SetBumpTexture(ID3D11ShaderResourceView* texture) { m_pMaterial->SetBumpTexture(texture); }

	/// @brief タグを初期化します。
	void InitializeTag() override;
};

