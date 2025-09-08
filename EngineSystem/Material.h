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

#include <string>

#include "../EngineMetaFile.h"

#include <vector>

struct SUB_MATERIAL
{
    std::string name;
    MATERIAL material;
    int textureID = -1;
    int bumpTextureID = -1;
    int toonTextureID = -1;
};

//==========================================================================
// クラス定義
//==========================================================================

class Material : public EngineMetaFile
{
private:
	// シェーダーキー
    std::string m_VertexShader{};
    std::string m_PixelShader{};

	// マテリアルとテクスチャ番号

    std::vector<SUB_MATERIAL*> m_SubMaterial;

	unsigned int	StartIndex = 0;
	unsigned int	IndexNum = 0;

public:
	/// @brief Material クラスのデフォルトコンストラクタです。
	Material() {
        m_ClassID = CID_Material;
    }
	/// @brief Material クラスのデストラクタです。
	~Material() = default;

	/// @brief マテリアル情報をGPUに送ります。
    /// @note 描画前に呼び出す必要があります。
	void DrawMaterial();

	void DrawGUI();

    void ImportFile(std::vector<std::string>& tokens) override;
    void ExportFile() override;

	/// @brief 頂点シェーダーのキーを設定します。
	/// @param key 設定する頂点シェーダーのキー。
	void SetVertexShaderKey(std::string key);
	/// @brief ピクセルシェーダーのキーを設定します。
	/// @param key 設定するピクセルシェーダーのキー。
	void SetPixelShaderKey(std::string key);

	void SetShader();

    void AddSubMaterial(SUB_MATERIAL* subMaterial) { m_SubMaterial.push_back(subMaterial); }
    void SetSubMaterials(std::vector<SUB_MATERIAL*> subMaterials) { m_SubMaterial = subMaterials; }
    /*
	/// @brief 繝�繧ｯ繧ｹ繝√Ε繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param texture 險ｭ螳壹☆繧紀D3D11ShaderResourceView蝙九�ｮ繝�繧ｯ繧ｹ繝√Ε縲�
	void SetTexture(int texture) { m_TextureFileID = texture; }
	/// @brief 繝舌Φ繝励�槭ャ繝励ユ繧ｯ繧ｹ繝√Ε繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param bumpTexture 險ｭ螳壹☆繧紀D3D11ShaderResourceView蝙九�ｮ繝舌Φ繝励�槭ャ繝励ユ繧ｯ繧ｹ繝√Ε縲�
	void SetBumpTexture(int bumpTexture) { m_BumpTextureFileID = bumpTexture; }
    void SetToonTexture(int toonTexture) { m_ToonTextureFileID = toonTexture; }
    */

	/// @brief 鬆らせ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｮ譁�蟄怜�励ｒ蜿門ｾ励＠縺ｾ縺吶�
	/// @return 譬ｼ邏阪＆繧後※縺�繧矩�らせ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｮ譁�蟄怜�励�
	std::string GetVertexShader() const { return m_VertexShader; }
	/// @brief 繝斐け繧ｻ繝ｫ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｮ譁�蟄怜�励ｒ蜿門ｾ励＠縺ｾ縺吶�
	/// @return 繝斐け繧ｻ繝ｫ繧ｷ繧ｧ繝ｼ繝繝ｼ繧定｡ｨ縺� std::string 蝙九�ｮ蛟､縲�
	std::string GetPixelShader() const { return m_PixelShader; }

	void SetStartIndex(unsigned int startIndex) { StartIndex = startIndex; }
	void SetIndexNum(unsigned int indexNum) { IndexNum = indexNum; }

	unsigned int GetStartIndex() const { return StartIndex; }
	unsigned int GetIndexNum() const { return IndexNum; }

    /*
	void SetTextureEnable(bool enable) { m_Material.textureEnable = enable; }

    void SetMaterial(MATERIAL material) { m_Material = material; }

	/// @brief 繝�繧ｯ繧ｹ繝√Ε繝ｪ繧ｽ繝ｼ繧ｹ繝薙Η繝ｼ繧貞叙蠕励＠縺ｾ縺吶�
	/// @return 繝�繧ｯ繧ｹ繝√Ε繧定｡ｨ縺� ID3D11ShaderResourceView 繝昴う繝ｳ繧ｿ縲�
	int GetTexture() const { return m_TextureFileID; }
    */
};

