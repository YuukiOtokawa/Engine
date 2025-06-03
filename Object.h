// ========================================================
//
// �I�u�W�F�N�g�N���X[Object.h]
// 
//									Date:	20250520
//									Author:	Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// �w�b�_�[�C���N���[�h
//==========================================================================

#include <list>

#include "FrameWork.h"

// ���O��Ԃ̎w��

using namespace GameObjectTagLayer;

class Component; // Forward declaration of Component class
class Object
{
private:
	// �R���|�[�l���g�̃��X�g
	std::list<Component*> m_Components;
	// �I�u�W�F�N�g�̖��O�ƃ^�O
	std::string m_Name;
	GameObjectTag m_Tag;

	// �I�u�W�F�N�g�̏��
	bool m_IsActive = true; // Flag to indicate if the object is active
	bool m_IsDrawable = true; // Flag to indicate if the object is drawable

	// ���_���ƃC���f�b�N�X��
	int m_iVertexCount = 0;
	int m_iIndexCount = 0;

public:
	// �f�t�H���g�R���X�g���N�^�ƃf�X�g���N�^
	Object() = default;
	Object(const Object&) = delete; // Disable copy constructor
	Object(Object&&) = delete; // Disable move constructor
	Object& operator=(const Object&) = delete; // Disable copy assignment
	Object& operator=(Object&&) = delete; // Disable move assignment

	~Object() {
		for (Component* component : m_Components) {
			delete component; // Clean up dynamically allocated components
		}
	}

	/// @brief �������������s���܂��B
	virtual void Initialize();
	/// @brief �I�u�W�F�N�g�܂��̓R���|�[�l���g�̏�Ԃ��X�V���܂��B
	void Update();
	/// @brief �`�揈�������s���܂��B
	void Draw();

	void DrawGUI();
  	/// @brief ���\�[�X�̉����㏈�����s���܂��B
	void Finalize();

	/// @brief �I�u�W�F�N�g�ɃR���|�[�l���g��ǉ����܂��B
	/// @tparam T �ǉ�����R���|�[�l���g�̌^�BT��Component�N���X���p�����Ă���K�v������܂��B
	/// @tparam ...Args 
	/// @param ...args 
	template<typename T, typename... Args>
	void AddComponent(Args&&... args) {
		static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
		T* component = new T(std::forward<Args>(args)...);
		AddComponentClass(component);
	}

	/// @brief �R���|�[�l���g�N���X��ǉ����܂��B
	/// @param component �ǉ�����R���|�[�l���g�ւ̃|�C���^�B
	void AddComponentClass(Component* component);

	/// @brief �w�肳�ꂽ�^�̍ŏ��̃R���|�[�l���g�ւ̃|�C���^���擾���܂��B
	/// @tparam T �擾�������R���|�[�l���g�̌^�B
	/// @return �w�肳�ꂽ�^�Ɉ�v����ŏ��̃R���|�[�l���g�ւ̃|�C���^�B��v����R���|�[�l���g�����݂��Ȃ��ꍇ��nullptr��Ԃ��܂��B
	template<typename T>
	T* GetComponent() {
		for (Component* component : m_Components) {
			if (T* castedComponent = dynamic_cast<T*>(component)) {
				return castedComponent;
			}
		}
		return nullptr; // Return nullptr if no matching component is found
	}

	/// @brief ���O��ݒ肵�܂��B
	/// @param name �ݒ肷�閼�O�B
	void SetName(const std::string& name) {
		m_Name = name;
	}

	/// @brief �I�u�W�F�N�g�̖��O���擾���܂��B
	/// @return �I�u�W�F�N�g�̖��O��\�� std::string�B
	std::string GetName() const {
		return m_Name;
	}

	/// @brief �Q�[���I�u�W�F�N�g�̃^�O��ݒ肵�܂��B
	/// @param tag �ݒ肷��GameObjectTag�l�B
	void SetTag(GameObjectTag tag) {
		m_Tag = tag;
	}

	/// @brief �I�u�W�F�N�g�̃^�O���擾���܂��B
	/// @return ���̃I�u�W�F�N�g�Ɋ֘A�t�����Ă��� GameObjectTag ��Ԃ��܂��B
	GameObjectTag GetTag() const {
		return m_Tag;
	}

	/// @brief �I�u�W�F�N�g�̃A�N�e�B�u��Ԃ�ݒ肵�܂��B
	/// @param isActive �A�N�e�B�u��Ԃɐݒ肷��ꍇ�� true�A��A�N�e�B�u�ɂ���ꍇ�� false ���w�肵�܂��B
	void SetActive(bool isActive) {
		m_IsActive = isActive;
	}

	/// @brief �`��\���ǂ�����ݒ肵�܂��B
	/// @param isDrawable �`��\�ɂ���ꍇ�� true�A�����łȂ��ꍇ�� false ���w�肵�܂��B
	void SetDrawable(bool isDrawable) {
		m_IsDrawable = isDrawable;
	}

	/// @brief ���_����ݒ肵�܂��B
	/// @param count �ݒ肷�钸�_���B
	void SetVertexCount(int count) {
		m_iVertexCount = count;
	}

	/// @brief �C���f�b�N�X�̐���ݒ肵�܂��B
	/// @param count �ݒ肷��C���f�b�N�X�̐��B
	void SetIndexCount(int count) {
		m_iIndexCount = count;
	}
};

