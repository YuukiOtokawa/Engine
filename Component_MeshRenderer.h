// ========================================================
//
// ���b�V�������_���[�R���|�[�l���g���N���X[Component_MeshRenderer.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// �w�b�_�[�C���N���[�h
//==========================================================================

#include "Component.h"

#include "Material.h"

//==========================================================================
// �N���X��`
//==========================================================================

class MeshRenderer :
    public Component
{
private:
	// �}�e���A���|�C���^
	Material* m_pMaterial = nullptr;
	int m_MaterialNum = 1;

public:
	/// @brief MeshRenderer �N���X�̐V�����C���X�^���X���쐬���܂��B
	MeshRenderer();
	/// @brief MeshRenderer �̃f�X�g���N�^�ł��B
	~MeshRenderer() = default;

    /// @brief �I�u�W�F�N�g�̏�Ԃ��X�V���܂��B
    void Update() override;
	/// @brief �`�揈�������s���܂��B
	void Draw() override;
	void DrawGUI() override;

	/// @brief �}�e���A����ݒ肵�܂��B
	/// @param material �ݒ肷��}�e���A���B
	void SetMaterial(MATERIAL material) { this->m_pMaterial->SetMaterial(material); }
	/// @brief LIGHT�^�̌�������ݒ肵�܂��B
	/// @param light �ݒ肷��LIGHT�^�̌������B
	void SetLight(LIGHT light) { 
		this->m_pMaterial->SetLight(light);
	}

	void SetMaterial(Material* material) { this->m_pMaterial = material; }
	void SetMaterialNum(int num) { this->m_MaterialNum = num; }

	/// @brief ���_�V�F�[�_�[���w�肳�ꂽ�L�[�Őݒ肵�܂��B
	/// @param key �ݒ肷�钸�_�V�F�[�_�[�̃L�[�B
	void SetVertexShader(std::string key) {
		this->m_pMaterial->SetVertexShaderKey(key);
	}
	/// @brief �s�N�Z���V�F�[�_�[���w�肳�ꂽ�L�[�Őݒ肵�܂��B
	/// @param key �ݒ肷��s�N�Z���V�F�[�_�[�̃L�[�B
	void SetPixelShader(std::string key) {
		this->m_pMaterial->SetPixelShaderKey(key);
	}

	/// @brief �e�N�X�`����ݒ肵�܂��B
	/// @param texture �ݒ肷��ID3D11ShaderResourceView�^�̃e�N�X�`���B
	void SetTexture(ID3D11ShaderResourceView* texture) { m_pMaterial->SetTexture(texture); }

	/// @brief �R���|�[�l���g�ɑΉ������^�O�����L�҂̃I�u�W�F�N�g�ɐݒ肵�܂��B
	void InitializeTag() override;
};

