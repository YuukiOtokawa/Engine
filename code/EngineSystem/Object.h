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

#include "EngineMetaFile.h"
#include "EngineAPI.h"

// 名前空間定義

using namespace GameObjectTagLayer;

class ObjectNode;

class Component; // Forward declaration of Component class
class OTOKAWA_API Object : public EngineMetaFile
{
private:
	// コンポーネントのリスト
	std::list<Component*> m_Components;

    std::list<Component*> m_DeleteComponents;
	// オブジェクトの名前とタグ
	std::string m_Name;
	GameObjectTag m_Tag = NoObject;
    GameObjectLayer m_Layer = GameObjectLayer::ObjectLayer; // Layer for the object

    ObjectNode* m_ObjectNode = nullptr;
	
	bool m_IsActive = true; // Flag to indicate if the object is active
	bool m_IsDrawable = true; // Flag to indicate if the object is drawable
    bool m_IsInEditName = false;
    char m_EditNameBuffer[20];

	// 使用する頂点とインデックスの数
	int m_iVertexCount = 0;
	int m_iIndexCount = 0;

    Object* m_pParent = nullptr; // Pointer to the parent object, if any
    std::list<Object*> m_Children; // List of child objects

    bool m_IsOpened = false; // Flag to indicate if the object is opened in the editor
    bool m_IsChild = false; // Flag to indicate if the object is a child of another object

public:
	// @brief コンストラクタ
    Object(bool editable = true);
	Object(const Object&) = delete; // Disable copy constructor
	Object(Object&&) = delete; // Disable move constructor
	Object& operator=(const Object&) = delete; // Disable copy assignment
	Object& operator=(Object&&) = delete; // Disable move assignment

	~Object();

	/// @brief 初期化
	virtual void Initialize();
	/// @brief コンポーネントの更新
	void Update();

    void OnCollision();
	/// @brief 描画処理を実行します。
	void Draw();

	void DrawGUI();

    void RightClickMenu() override;

	/// @brief 終了処理
	void Finalize();

	void Destroy();

	/// @brief コンポーネントの追加
	/// @tparam T コンポーネントのクラステンプレート
	/// @tparam ...Args コンポーネントのコンストラクタ引数テンプレート
	/// @param ...args コンポーネントのコンストラクタ引数
	template<typename T, typename... Args>
    T* AddComponent(bool isInGame = true) {
		static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
		T* component = new T();
    	AddComponentClass(component, isInGame);
        return component;
	}

	/// @brief コンポーネントクラスを追加します。
	/// @param component 追加する Component 型のポインタ。
	void AddComponentClass(Component* component, bool isInGame = true);

	/// @brief 指定された型の最初のコンポーネントを取得します。
	/// @tparam T 取得したいコンポーネントの型。
	/// @return 指定された型にキャスト可能な最初のコンポーネントへのポインタ。該当するコンポーネントが存在しない場合はnullptrを返します。
	template<typename T>
	T* GetComponent() {
		for (Component* component : m_Components) {
			if (T* castedComponent = dynamic_cast<T*>(component)) {
				return castedComponent;
			}
		}
		return nullptr; // Return nullptr if no matching component is found
	}

    void DeleteComponent(Component* component);

	/// @brief コンポーネントのデータをYAML形式でエクスポートします。
    void ExportFile(YAML::Emitter& out) override;


    void AddExportList() override;

    void ImportFile(YAML::Node& node) override;

	/// @brief 名前を設定します。
	/// @param name 設定する名前。
    void SetName(const std::string& name);

    void ResolveNameConflict(const std::string& name) {
        m_Name = name;
    }

	/// @brief オブジェクトの名前を取得します。
	/// @return オブジェクトの名前を表す std::string 型の値。
	std::string GetName() const {
		return m_Name;
	}

	/// @brief ゲームオブジェクトのタグを設定します。
	/// @param tag 設定するGameObjectTag型のタグ。
	void SetTag(GameObjectTag tag) {
		m_Tag = tag;
	}

	/// @brief 
	/// @return 
	GameObjectTag GetTag() const {
		return m_Tag;
	}

    void SetLayer(GameObjectLayer layer) {
        m_Layer = layer;
    }

    GameObjectLayer GetLayer() const {
        return m_Layer;
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

    // parent == nullptr で親子関係解消
    void SetParent(Object* parent) {
        if (m_pParent) {
            m_pParent->DeleteChild(this);
        }
        m_pParent = parent;
        m_IsChild = (parent != nullptr);
    }

    void AddChild(Object* child) {
        if (child) {
            child->SetParent(this);
            m_Children.push_back(child);
        }
    }
    void DeleteChild(Object* child) {
        if (child) {
            m_Children.remove(child);
        }
    }

    bool HasChild() {
        return !m_Children.empty();
    }

    Object* GetParent() const {
        return m_pParent;
    }
    std::list<Object*>* GetChildren() {
        return &m_Children;
    }

    void SetOpened(bool isOpened) {
        if (isOpened)
            m_IsOpened ^= true; // Toggle the opened state
    }

    bool IsOpened() const {
        return m_IsOpened;
    }

    bool IsChild() const {
        return m_IsChild;
    }

    /// @brief コンポーネントリストを取得
    const std::list<Component*>& GetComponents() const {
        return m_Components;
    }

    void SetObjectNode(ObjectNode* node) {
        m_ObjectNode = node;
    }
    ObjectNode* GetObjectNode() const {
        return m_ObjectNode;
    }
};

