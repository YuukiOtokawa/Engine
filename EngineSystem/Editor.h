// ========================================================
//
// エディター管理クラス[Editor.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include <list>
#include <map>

#include "Object.h"

#include "EditorWindow.h"

#include "GUI.h"
#include "Component_Camera.h"

#include "../ParticleManager.h"

#include "Material.h"

//==========================================================================
// クラス定義
//==========================================================================

class Editor
{
private:
	// オブジェクトのリスト
	std::list<Object*> m_Objects;
	std::list<Object*> m_DeleteObjects;

    std::list<Material*> m_Materials; // マテリアルのリスト
	// エディターウィンドウのリスト
	std::list<EditorWindow*> m_EditorWindows;

	// GUIのポインタ
	GUI* m_pGUI = nullptr;
    ParticleManager* m_pParticleManager = nullptr;

	/// @brief Editor クラスのシングルトンパターンデフォルトコンストラクタです。
	Editor() = default;
	Editor(const Editor&) = delete;
	Editor& operator=(const Editor&) = delete;

	// Editor クラスのシングルトンパターンインスタンス
	static Editor* m_pInstance;

	Object* m_pSelectedObject = nullptr;
	Object* m_pActiveCamera = nullptr;

    std::string m_NextSceneName = "";

public:
	/// @brief エディタークラスのシングルトンインスタンスを取得します。
	/// @return Editor クラスの唯一のインスタンスへのポインタ。
	static Editor* GetInstance()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new Editor();
		}
		return m_pInstance;
	}

    ~Editor();
	
	/// @brief エディターの初期化処理を実行します。
	void Initialize();
	/// @brief オブジェクトとシステムの状態を更新します。
	void Update();
	/// @brief オブジェクトの描画処理を実行します。
	void Draw();
	/// @brief オブジェクトやリソースの後処理を行います。
	void Finalize();

	/// @brief オブジェクトを追加します。
	/// @param object 追加するオブジェクトへのポインタ。
	void AddObject(Object* object);
	void DeleteObject(Object* object);
	/// @brief 指定された名前のオブジェクトを検索して返します。
	/// @param name 検索するオブジェクトの名前。
	/// @return 見つかった場合は対応するObjectポインタ、見つからない場合はnullptrを返します。
	Object* GetObject(const std::string& name)
	{
		for (auto object : m_Objects)
		{
			if (object->GetName() == name)
			{
				return object;
			}
		}
		return nullptr;
	}

    std::list<Object*>& GetObjects() { return m_Objects; }
    void SetObjects(const std::list<Object*>& objects) { m_Objects = objects; }

	void SetActiveCamera(Object* camera)
	{
		if (m_pActiveCamera != nullptr)
			m_pActiveCamera->GetComponent<Camera>()->SetActiveCamera(false);
		
		m_pActiveCamera = camera;
		m_pActiveCamera->GetComponent<Camera>()->SetActiveCamera(true);
	}

	Object* GetActiveCamera() const { return m_pActiveCamera; }


    int AddMaterial(Material* material);

    void ChangeScene(std::string sceneName)
    {
        m_NextSceneName = sceneName;
    }
};

void Main();
void SetSceneTitle();
void SetSceneGame();
void SetSceneResult();
