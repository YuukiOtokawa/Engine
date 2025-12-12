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

#include "ParticleManager.h"

#include "Material.h"

#include "AudioManager.h"

#include "FrameWork.h"

#include "VertexIndex.h"

#include "CollisionManager.h"

#include "EngineAPI.h"

#include "ProjectWindow.h"

#include "Node/NodeManager.h" // NodeManagerをインクルード

//==========================================================================
// クラス定義
//==========================================================================

class OTOKAWA_API Editor
{
private:
	// オブジェクトのリスト
    std::map<ClassID, std::list<Component*>> m_Components;
	std::list<Object*> m_Objects;
	std::list<Object*> m_DeleteObjects;

    std::list<VertexIndex*> m_VertexIndices; // 頂点とインデックスのリスト
    std::list<Material*> m_Materials; // マテリアルのリスト

	// GUIのポインタ
	GUI* m_pGUI = nullptr;
    ParticleManager* m_pParticleManager = nullptr;

    AudioManager* m_pAudioManager = nullptr;

    CollisionManager* m_pCollisionManager = nullptr;

    ProjectWindow* m_pProjectWindow = nullptr;

    NodeManager* m_pNodeManager = nullptr; // NodeManagerのポインタを追加

    // 再生モード関連
    bool m_isPlaying = false;                    // 再生中かどうか
    std::string m_playModeSceneBackup = "";      // 再生開始時のシーン保存先

    // Prefab編集モード関連
    bool m_isEditingPrefab = false;              // Prefab編集中かどうか
    Object* m_pEditingPrefabObject = nullptr;    // 編集中のPrefabオブジェクト
    std::string m_editingPrefabPath = "";        // 編集中のPrefabファイルパス

	/// @brief Editor クラスのシングルトンパターンデフォルトコンストラクタです。
	Editor() = default;
	Editor(const Editor&) = delete;
	Editor& operator=(const Editor&) = delete;

	// Editor クラスのシングルトンパターンインスタンス
	static Editor* m_pInstance;

	Object* m_pSelectedObject = nullptr;
	Object* m_pActiveCamera = nullptr;

    Object* m_pEditorCamera = nullptr;

    std::string m_NextSceneName = "";

    bool m_isSceneViewHovered = false;

	// ImGuizmo操作モード (0=移動, 1=回転, 2=スケール)
	int m_GizmoOperation = 0;

    void UpdateAllObjects();
    void DeleteMarkedObjects();

    void DrawGameViewRTV();
    void DrawSceneViewRTV();

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

    void DrawGame(Object* camera, Object* renderTexture = nullptr);
	/// @brief オブジェクトやリソースの後処理を行います。
	void Finalize();

    void CreateComponent(Component* component);
    void DeleteComponent(Component* component);

    Component* GetComponentByFileID(int fileID);

    std::list<Component*> GetComponentsByClassID(ClassID classID) {
        return m_Components[classID];
    }


	/// @brief オブジェクトを追加します。
	/// @param object 追加するオブジェクトへのポインタ。
	void AddObject(Object* object);
	void DeleteObject(Object* object);
    void CreateObject();

    void CheckObjectNameConflict(Object* object);
	/// @brief 指定された名前のオブジェクトを検索して返します。
	/// @param name 検索するオブジェクトの名前。
	/// @return 見つかった場合は対応するObjectポインタ、見つからない場合はnullptrを返します。
    Object* GetObject(const std::string& name);

    std::list<Object*>* GetObjects() { return &m_Objects; }
    void SetObjects(const std::list<Object*>& objects) { m_Objects = objects; }

    Object* GetObjectByFileID(int fileID);

    void ResetScene();

    void SetActiveCamera(Object* camera);

	Object* GetActiveCamera() const { return m_pActiveCamera; }
    Object* GetEditorCamera() const { return m_pEditorCamera; }

    Object* GetSelectedObject() const { return m_pSelectedObject; }
    void SetSelectedObject(Object* object) { m_pSelectedObject = object; }

    int AddMaterial(Material* material);
    int AddVertexIndex(VertexIndex* vertexIndex);

    Material* GetMaterialByFileID(int fileID);
    VertexIndex* GetVertexIndexByFileID(int fileID);

    void ChangeScene(std::string sceneName);

    void OpenScene(std::string sceneFilePath);

    // 再生モード関連
    /// @brief 再生モードを開始（シーンを保存して実行開始）
    void Play();

    /// @brief 再生モードを停止（シーンを復元）
    void Stop();

    /// @brief 再生中かどうかを取得
    /// @return 再生中の場合true
    bool IsPlaying() const { return m_isPlaying; }

    // Prefab編集モード関連
    /// @brief Prefabファイルを編集用に読み込む
    /// @param filePath Prefabファイルパス
    /// @return 成功した場合true
    bool LoadPrefabForEdit(const std::string& filePath);

    /// @brief 編集中のPrefabを保存
    /// @return 成功した場合true
    bool SaveEditingPrefab();

    /// @brief Prefab編集モードを終了
    void CloseEditingPrefab();

    /// @brief Prefab編集中かどうかを取得
    /// @return Prefab編集中の場合true
    bool IsEditingPrefab() const { return m_isEditingPrefab; }

    /// @brief 編集中のPrefabオブジェクトを取得
    /// @return 編集中のPrefabオブジェクト
    Object* GetEditingPrefabObject() const { return m_pEditingPrefabObject; }
    std::string GetEditingPrefabPath() const { return m_editingPrefabPath; }

    bool GetIsSceneViewHovered() const { return m_isSceneViewHovered; }

    NodeManager* GetNodeManager() const { return m_pNodeManager; } // NodeManagerのゲッターを追加
};

void Main();
void SetSceneGame();
