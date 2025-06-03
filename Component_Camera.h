// ========================================================
//
// �J�����R���|�[�l���g[Component_Camera.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// �w�b�_�[�C���N���[�h
//==========================================================================

#include "Component.h"
#include "Vector4O.h"
#include "FrameWork.h"

//==========================================================================
// �N���X��`
//==========================================================================

class Camera : public Component
{
private:
	// �C�[�W���O�p �O�t���[���̈ʒu��ۑ�
    Vector4O m_LastPosition = Vector4O::Zero();

	// �J�����̒����_
    Vector4O m_Target;
	// �J�����̊p�x�i�s�b�`�A���[�A���[���j
    Vector4O m_Angle;

	// �J�����̎���p�iFOV�j
    float m_Fov = 90.0f;

	// ���e�s��
    XMMATRIX m_Projection;
	// �r���[�s��
	XMMATRIX m_View;

	// �ߐڃN���b�s���O����
	float m_Near = 0.1f;
	// �����N���b�s���O����
    float m_Far = 1000.0f;

	// �J�����̏�����x�N�g��
	Vector4O m_Up = Vector4O::Up();

public:
	// �R���X�g���N�^
    Camera();

	/// @brief ���e�s����擾���܂��B
	/// @return ���݂̓��e�s��iXMMATRIX�^�j�B
	XMMATRIX GetProjection() const { return m_Projection; }

	/// @brief �r���[�s����擾���܂��B
	/// @return ���݂̃r���[�s��iXMMATRIX�^�j�B
	XMMATRIX GetView() const { return m_View; }

	/// @brief �p�x��\���x�N�g�����擾���܂��B
	/// @return �p�x��\�� Vector4O �^�̒l�B
	Vector4O GetAngle() const { return m_Angle; }

    /// @brief �I�u�W�F�N�g�̏�Ԃ��X�V���܂��i�I�[�o�[���C�h���ꂽ���\�b�h�j�B
    void Update() override;
	/// @brief �`�揈�������s���܂��B
	void Draw() override;
  
	void DrawGUI() override;
	/// @brief �R���|�[�l���g�ɑΉ������^�O�����L�҂̃I�u�W�F�N�g�ɐݒ肵�܂��i�I�[�o�[���C�h���ꂽ���\�b�h�j�B
  void InitializeTag() override;

	/// @brief �����_��ݒ肵�܂��B
	/// @param target �ݒ肷��Vector4O�^�̒����_�B
	void SetTarget(Vector4O target) { m_Target = target; }
	/// @brief �p�x��ݒ肵�܂��B
	/// @param angle �ݒ肷��p�x��\�� Vector4O �^�̒l�B
	void SetAngle(Vector4O angle) { m_Angle = angle; }
	/// @brief ����p�iFOV�j��ݒ肵�܂��B
	/// @param fov �ݒ肷�鎋��p�iFOV�j�̒l�B
	void SetFov(float fov) { m_Fov = fov; }

	Vector4O GetTarget() const { return m_Target; }
};

