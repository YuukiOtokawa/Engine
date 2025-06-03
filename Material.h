// ========================================================
//
// �}�e���A���N���X[Material.h]
// 
//									Date:	20250520
//									Author:	Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// �w�b�_�[�C���N���[�h
//==========================================================================

#include <d3d11.h>

#include "FrameWork.h"

//==========================================================================
// �N���X��`
//==========================================================================

class Material
{
private:
	// �V�F�[�_�[�̃L�[
	std::string m_VertexShader;
	std::string m_PixelShader;

	// �}�e���A���ƌ����̏��
	MATERIAL m_Material;
	LIGHT m_Light;
	// �e�N�X�`���̃��\�[�X�r���[
	ID3D11ShaderResourceView* m_Texture;

	unsigned int	StartIndex;
	unsigned int	IndexNum;

public:
	/// @brief Material �N���X�̃f�t�H���g�R���X�g���N�^�ł��B
	Material() = default;
	/// @brief Material �N���X�̃f�X�g���N�^�ł��B
	~Material() = default;

	/// @brief �}�e���A�������V�F�[�_�[�ɑ��M
	void DrawMaterial();

	/// @brief ���̃}�e���A���ɒ��_�V�F�[�_�[�̃L�[��ݒ肵�܂��B
	/// @param key �ݒ肷�钸�_�V�F�[�_�[�̃L�[�B
	void SetVertexShaderKey(std::string key);
	/// @brief ���̃}�e���A���Ƀs�N�Z���V�F�[�_�[�̃L�[��ݒ肵�܂��B
	/// @param key �ݒ肷��s�N�Z���V�F�[�_�[�̃L�[�B
	void SetPixelShaderKey(std::string key);

	void SetShader();

  	/// @brief �}�e���A����ݒ肵�܂��B
	/// @param material �ݒ肷��}�e���A���B
	void SetMaterial(MATERIAL material) { m_Material = material; }

	/// @brief ��������ݒ肵�܂��B
	void SetLight(LIGHT light) { m_Light = light; }

	/// @brief �e�N�X�`����ݒ肵�܂��B
	/// @param texture �ݒ肷��ID3D11ShaderResourceView�^�̃e�N�X�`���B
	void SetTexture(ID3D11ShaderResourceView* texture) { m_Texture = texture; }

	/// @brief ���̃}�e���A���̒��_�V�F�[�_�[�̃L�[���擾���܂��B
	/// @return ���̃}�e���A���Ɋi�[����Ă��钸�_�V�F�[�_�[�̃L�[�B
	std::string GetVertexShader() const { return m_VertexShader; }
	/// @brief ���̃}�e���A���̃s�N�Z���V�F�[�_�[�̃L�[���擾���܂��B
	/// @return ���̃}�e���A���Ɋi�[����Ă���s�N�Z���V�F�[�_�[�̃L�[�B
	std::string GetPixelShader() const { return m_PixelShader; }

	void SetStartIndex(unsigned int startIndex) { StartIndex = startIndex; }
	void SetIndexNum(unsigned int indexNum) { IndexNum = indexNum; }

	unsigned int GetStartIndex() const { return StartIndex; }
	unsigned int GetIndexNum() const { return IndexNum; }

	void SetTextureEnable(bool enable) { m_Material.textureEnable = enable; }

  	/// @brief ���݂̃I�u�W�F�N�g�̃}�e���A�����擾���܂��B
	/// @return ���̃I�u�W�F�N�g�Ɋ֘A�t�����Ă���MATERIAL�^�̃}�e���A���B
	MATERIAL GetMaterial() const { return m_Material; }
	/// @brief ���C�g�����擾���܂��B
	/// @return ���݂̃��C�g���iLIGHT�^�j�B
	LIGHT GetLight() const { return m_Light; }
	/// @brief �e�N�X�`�����\�[�X�r���[���擾���܂��B
	/// @return �e�N�X�`����\�� ID3D11ShaderResourceView �|�C���^�B
	ID3D11ShaderResourceView* GetTexture() const { return m_Texture; }
};

