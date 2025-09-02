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
	// マテリアル
	Material* m_pMaterial = nullptr;
	int m_MaterialNum = 1;

public:
	/// @brief MeshRenderer クラスの新しいインスタンスを作成します。
	MeshRenderer();

    /// @brief コンポーネントの状態を更新します。
    void UpdateComponent() override;
	/// @brief 描画処理を実行します。
	void Draw() override;
	void DrawGUI() override;

    void ImportFile(std::vector<std::string>& tokens) override;

    void ExportComponent() override;

    void AddExportList() override;

	/// @brief マテリアルを設定します。
	/// @param material 設定するマテリアル。
	void SetMaterial(MATERIAL material) { this->m_pMaterial->SetMaterial(material); }
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

	/// @brief テクスチャをマテリアルに設定します。
	/// @param texture 設定するID3D11ShaderResourceView型のテクスチャ。
	void SetTexture(int texture) {
        m_pMaterial->SetTexture(texture);
        if (owner->HasChild()) {
            for (auto& child : owner->GetChildren()) {
                if (child->GetComponent<MeshRenderer>()) {
                    child->GetComponent<MeshRenderer>()->SetTexture(texture);
                }
            }
        }
    }
	void SetBumpTexture(int texture) {
        m_pMaterial->SetBumpTexture(texture);
        if (owner->HasChild()) {
            for (auto& child : owner->GetChildren()) {
                if (child->GetComponent<MeshRenderer>()) {
                    child->GetComponent<MeshRenderer>()->SetBumpTexture(texture);
                }
            }
        }
    }
	void SetToonTexture(int texture) {
        m_pMaterial->SetToonTexture(texture);
        if (owner->HasChild()) {
            for (auto& child : owner->GetChildren()) {
                if (child->GetComponent<MeshRenderer>()) {
                    child->GetComponent<MeshRenderer>()->SetToonTexture(texture);
                }
            }
        }
    }

	/// @brief タグを初期化します。
	void InitializeTag() override;
};

