// ========================================================
//
// オブジェクトクラス[Object.h]
// 
//									Date:	20250520
//									Author:	Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include <list>

#include "FrameWork.h"

// 名前空間

using namespace GameObjectTagLayer;

class Component; // Forward declaration of Component class
class Object
{
private:
	// コンポーネントリスト
	std::list<Component*> m_Components;
	// オブジェクトの名前
	std::string m_Name;
	GameObjectTag m_Tag;

	
	bool m_IsActive = true; // Flag to indicate if the object is active
	bool m_IsDrawable = true; // Flag to indicate if the object is drawable

	// 頂点数とインデックス数
	int m_iVertexCount = 0;
	int m_iIndexCount = 0;

public:
	// @brief コンストラクタ
	Object() = default;
	Object(const Object&) = delete; // Disable copy constructor
	Object(Object&&) = delete; // Disable move constructor
	Object& operator=(const Object&) = delete; // Disable copy assignment
	Object& operator=(Object&&) = delete; // Disable move assignment

	~Object() {
		for (Component* component : m_Components) {
			delete component; // Clean up dynamically allocated components
			component = nullptr;
		}
	}

	/// @brief 初期化処理を行います。
	virtual void Initialize();
	/// @brief 状態を更新します。
	void Update();
	/// @brief 描画処理を実行します。
	void Draw();

	void DrawGUI();
	/// @brief ファイナライズ処理を実行します。
	void Finalize();

	void Destroy();

	/// @brief コンポーネントの追加
	/// @tparam T 追加したいコンポーネントの型。
	/// @tparam ...Args 
	/// @param ...args 
	template<typename T, typename... Args>
	void AddComponent(Args&&... args) {
		static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
		T* component = new T(std::forward<Args>(args)...);
		AddComponentClass(component);
	}

	/// @brief コンポーネントクラスを追加します。
	/// @param component 追加するコンポーネントへのポインタ。
	void AddComponentClass(Component* component);

	/// @brief 指定した型のコンポーネントを取得します。
	/// @tparam T 取得したいコンポーネントの型。
	/// @return 指定した型に一致する最初のコンポーネントへのポインタ。一致するコンポーネントが存在しない場合はnullptrを返します。
	template<typename T>
	T* GetComponent() {
		for (Component* component : m_Components) {
			if (T* castedComponent = dynamic_cast<T*>(component)) {
				return castedComponent;
			}
		}
		return nullptr; // Return nullptr if no matching component is found
	}

	/// @brief 名前を設定します。
	/// @param name 設定する名前。
	void SetName(const std::string& name) {
		m_Name = name;
	}

	/// @brief オブジェクトの名前を取得します。
	/// @return オブジェクトの名前を表す std::string。
	std::string GetName() const {
		return m_Name;
	}

	/// @brief ゲームオブジェクトのタグを設定します。
	/// @param tag 設定するGameObjectTag型のタグ。
	void SetTag(GameObjectTag tag) {
		m_Tag = tag;
	}

	/// @brief ゲームオブジェクトのタグを取得します。
	/// @return このゲームオブジェクトに関連付けられているタグ。
	GameObjectTag GetTag() const {
		return m_Tag;
	}

	/// @brief オブジェクトのアクティブ状態を設定します。
	/// @param isActive アクティブ状態に設定する場合は true、非アクティブにする場合は false を指定します。
	void SetActive(bool isActive) {
		m_IsActive = isActive;
	}

	/// @brief 描画可能かどうかを設定します。
	/// @param isDrawable 描画可能にする場合は true、そうでない場合は false を指定します。
	void SetDrawable(bool isDrawable) {
		m_IsDrawable = isDrawable;
	}

	/// @brief 頂点数を設定します。
	/// @param count 設定する頂点数。
	void SetVertexCount(int count) {
		m_iVertexCount = count;
	}

	/// @brief インデックスの数を設定します。
	/// @param count 設定するインデックスの数。
	void SetIndexCount(int count) {
		m_iIndexCount = count;
	}
};

