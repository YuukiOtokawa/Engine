// ========================================================
//
// 繝槭ユ繝ｪ繧｢繝ｫ繧ｯ繝ｩ繧ｹ[Material.h]
// 
//									Date:	20250520
//									Author:	Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// 繝倥ャ繝繝ｼ繧､繝ｳ繧ｯ繝ｫ繝ｼ繝�
//==========================================================================

#include <d3d11.h>

#include "FrameWork.h"

#include <string>

#include "../EngineMetaFile.h"

//==========================================================================
// 繧ｯ繝ｩ繧ｹ
//==========================================================================

class Material : public EngineMetaFile
{
private:
	// 繧ｷ繧ｧ繝ｼ繝繝ｼ繧ｭ繝ｼ
	std::string m_VertexShader;
	std::string m_PixelShader;

	// 繝槭ユ繝ｪ繧｢繝ｫ
	MATERIAL m_Material;
	ID3D11ShaderResourceView* m_Texture = nullptr;
	ID3D11ShaderResourceView* m_BumpTexture = nullptr;

	unsigned int	StartIndex = 0;
	unsigned int	IndexNum = 0;

public:
	/// @brief Material 繧ｯ繝ｩ繧ｹ縺ｮ繝�繝輔か繝ｫ繝医さ繝ｳ繧ｹ繝医Λ繧ｯ繧ｿ縺ｧ縺吶�
	Material() {
        m_ClassID = CID_Material;
    }
	/// @brief Material 繧ｯ繝ｩ繧ｹ縺ｮ繝�繧ｹ繝医Λ繧ｯ繧ｿ縺ｧ縺吶�
	~Material() = default;

	/// @brief 繝槭ユ繝ｪ繧｢繝ｫ繧呈緒逕ｻ縺励∪縺吶�
	void DrawMaterial();

	void DrawGUI();

	/// @brief 鬆らせ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｮ繧ｭ繝ｼ繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param key 險ｭ螳壹☆繧矩�らせ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｮ繧ｭ繝ｼ縲�
	void SetVertexShaderKey(std::string key);
	/// @brief 繝斐け繧ｻ繝ｫ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｮ繧ｭ繝ｼ繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param key 險ｭ螳壹☆繧九ヴ繧ｯ繧ｻ繝ｫ繧ｷ繧ｧ繝ｼ繝繝ｼ縺ｮ繧ｭ繝ｼ縲�
	void SetPixelShaderKey(std::string key);

	void SetShader();

	/// @brief 繝�繧ｯ繧ｹ繝√Ε繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param texture 險ｭ螳壹☆繧紀D3D11ShaderResourceView蝙九�ｮ繝�繧ｯ繧ｹ繝√Ε縲�
	void SetTexture(ID3D11ShaderResourceView* texture) { m_Texture = texture; }
	/// @brief 繝舌Φ繝励�槭ャ繝励ユ繧ｯ繧ｹ繝√Ε繧定ｨｭ螳壹＠縺ｾ縺吶�
	/// @param bumpTexture 險ｭ螳壹☆繧紀D3D11ShaderResourceView蝙九�ｮ繝舌Φ繝励�槭ャ繝励ユ繧ｯ繧ｹ繝√Ε縲�
	void SetBumpTexture(ID3D11ShaderResourceView* bumpTexture) { m_BumpTexture = bumpTexture; }

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

	void SetTextureEnable(bool enable) { m_Material.textureEnable = enable; }

    void SetMaterial(MATERIAL material) { m_Material = material; }

	/// @brief 繝�繧ｯ繧ｹ繝√Ε繝ｪ繧ｽ繝ｼ繧ｹ繝薙Η繝ｼ繧貞叙蠕励＠縺ｾ縺吶�
	/// @return 繝�繧ｯ繧ｹ繝√Ε繧定｡ｨ縺� ID3D11ShaderResourceView 繝昴う繝ｳ繧ｿ縲�
	ID3D11ShaderResourceView* GetTexture() const { return m_Texture; }
};

