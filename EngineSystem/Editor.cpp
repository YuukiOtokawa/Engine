// ========================================================
//
// エディター管理クラス[Editor.cpp]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

//==========================================================================
// ヘッダーインクルード
//==========================================================================




#include "Editor.h"

#include "MainEngine.h"

//以下コンポーネントのヘッダー
//省略できるようにしたい
#include "EditorCamera.h"
#include "Component_Transform.h"
#include "Component_MeshRenderer.h"
#include "Component_CubeMesh.h"
#include "PlaneMesh.h"
#include "AssimpMeshRenderer.h"

#include "Component_InputSystem.h"

#include "ModelLoader.h"
#include "OBJLoader.h"

#include "Player.h"
#include "PlayerCamera.h"

#include "Bullet.h"
#include "Billboard.h"
#include "Animation.h"

#include "imgui.h"

Editor* Editor::m_pInstance;

//==========================================================================
// メンバ関数
//==========================================================================

Editor::~Editor()
{
	if (m_pGUI) {
		delete m_pGUI;
		m_pGUI = nullptr;
	}
	// すべてのオブジェクトを削除
	for (auto& object : m_Objects) {
		delete object;
	}
	m_Objects.clear();
	for (auto& material : m_Materials) {
		delete material;
	}
	m_Materials.clear();
	if (m_pInstance) {
		m_pInstance = nullptr;
	}
}

void Editor::Initialize() {
	m_pGUI = new GUI();
	m_pParticleManager = ParticleManager::GetInstance();

	// シェーダーの読み込み
	{
		//光源計算無し
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("cso/unlitTextureVS.cso", "unlit");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("cso/unlitTexturePS.cso", "unlit");

		//頂点ライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("cso/vertexDirectionalLightingVS.cso", "vertex");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("cso/vertexDirectionalLightingPS.cso", "vertex");

		//ピクセルライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("cso/pixelLightingVS.cso", "pixel");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("cso/pixelLightingPS.cso", "pixel");

		//BlinnPhongライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("cso/blinnPhongVS.cso", "BlinnPhong");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("cso/blinnPhongPS.cso", "BlinnPhong");

		//半球ライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("cso/hemisphereLightingVS.cso", "hemisphere");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("cso/hemisphereLightingPS.cso", "hemisphere");

		//点光源ライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("cso/blinnPhongVS.cso", "pointLight");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("cso/pointLightingBlinnPhongPS.cso", "pointLight");

		//スポットライトライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("cso/spotLightingVS.cso", "spotLight");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("cso/spotLightingPS.cso", "spotLight");

		//リムライトライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("cso/blinnPhongVS.cso", "limLight");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("cso/limLightingPS.cso", "limLight");

		//法線マップ
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("cso/bumpVS.cso", "normal");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("cso/bumpPS.cso", "normal");

		//Cook-Torranceライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("cso/blinnPhongVS.cso", "CookTorrance");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("cso/cookPS.cso", "CookTorrance");
	}


	

	Main();
	m_pGUI->Initialize();
}

void Editor::Update() {
	for (auto& object : m_Objects) 
		object->Update();
	for (auto& object : m_DeleteObjects) {
		auto it = std::find(m_Objects.begin(), m_Objects.end(), object);
		if (it != m_Objects.end()) {
			if (*it == m_pSelectedObject)
				m_pSelectedObject = nullptr; // 選択中のオブジェクトをクリア
			delete *it; // オブジェクトを削除
			m_Objects.erase(it); // リストから削除
		}
	}
	m_DeleteObjects.clear();
	m_pParticleManager->UpdateParticles();
}

void Editor::Draw() {

	//レンダリングバッファクリア
	MainEngine::GetInstance()->GetRenderer()->BufferClear();

//==========================================================================
// オブジェクト描画処理
//==========================================================================

	//使用するカメラを設定
	Object* activeCamera = nullptr;
	for (auto& object : m_Objects) {
		if (object->GetTag() == GameObjectTagLayer::CameraTag &&
			object->GetComponent<Camera>()->IsActiveCamera()) {
			activeCamera = object;
		}
	}

	//使用するカメラの行列情報を登録
	if (activeCamera)
		activeCamera->Draw();

	// オブジェクトをカメラに遠い順にソート
	auto objects = m_Objects;
	objects.sort([activeCamera](Object* a, Object* b) {
		if (!activeCamera) return false; // activeCameraがnullptrの場合、ソートしない
		if (a->GetTag() == GameObjectTagLayer::CameraTag || b->GetTag() == GameObjectTagLayer::CameraTag)
			return false; // カメラオブジェクトはソートしない
		if (!a->GetComponent<Transform>() || !b->GetComponent<Transform>())
			return false; // Transformコンポーネントがない場合はソートしない
		auto aPos = a->GetComponent<Transform>()->GetPosition();
		auto bPos = b->GetComponent<Transform>()->GetPosition();
		auto aLength = (aPos - activeCamera->GetComponent<Transform>()->GetPosition()).Length();
		auto bLength = (bPos - activeCamera->GetComponent<Transform>()->GetPosition()).Length();
		return aLength > bLength; // 遠い順にソート
	});

	//オブジェクトの描画
	for (auto& object : objects) {
		if (object->GetTag() == GameObjectTagLayer::ObjectTag) {
			object->Draw();
		}
	}
	MainEngine::GetInstance()->GetRenderer()->SetRasterizerState2D();
	for (auto& object : objects) {
		if (object->GetTag() == GameObjectTagLayer::BillBoardTag) {
			object->Draw();
		}
	}
	//パーティクルの描画
	m_pParticleManager->DrawParticles();
	MainEngine::GetInstance()->GetRenderer()->SetWorldViewProjection2D();
	for (auto& object : objects) {
		if (object->GetTag() == GameObjectTagLayer::SpriteTag) {
			object->Draw();
		}
	}
	MainEngine::GetInstance()->GetRenderer()->SetRasterizerState3D();

//==========================================================================
// GUI描画処理
//==========================================================================

	//ImGuiの初期化
	m_pGUI->StartImGui();


	//インスペクタウィンドウの描画開始
	m_pGUI->StartInspector();

	//選択されたオブジェクトの情報を表示
	if (m_pSelectedObject)
		m_pSelectedObject->DrawGUI();

	m_pGUI->EndWindow();


	//ヒエラルキーウィンドウの描画開始
	m_pGUI->StartHierarchy();

	//全オブジェクトの名前ボタンを描画
	for (auto& object : m_Objects) {
		if (!object) continue; // nullptrチェック
		if (object == m_pSelectedObject)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.8f, 1.0f)); // 選択中のオブジェクトの色を変更
		else
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // 通常のオブジェクトの色

		if (ImGui::Button(object->GetName().c_str())){
			m_pSelectedObject = object;// 選択されたオブジェクトを更新
		}

		ImGui::PopStyleColor();
	}
		
	m_pGUI->EndWindow();

	// ImGui描画の終了
	m_pGUI->EndImGui();

	// レンダリングバッファの内容を画面に表示
	MainEngine::GetInstance()->GetRenderer()->BufferPresent();

	if (m_NextSceneName != "") {
		for (auto& object : m_Objects) {
			delete object; // すべてのオブジェクトを削除
		}
		m_Objects.clear(); // オブジェクトリストをクリア
		m_pActiveCamera = nullptr;
		m_pSelectedObject = nullptr;

		if (m_NextSceneName == "Title") {
			SetSceneTitle(); // タイトルシーンをセット
		}
		else if (m_NextSceneName == "Result") {
			SetSceneResult(); // リザルトシーンをセット
		}
		else if (m_NextSceneName == "Game") {
			SetSceneGame(); // ゲームシーンをセット
		}
		m_NextSceneName = ""; // シーン名をリセット
	}
}

void Editor::Finalize() {

}

void Editor::AddObject(Object* object)
{
	int copyCount = 0;
	for (auto& obj : m_Objects) {
		if (obj->GetName() == object->GetName() + std::to_string(copyCount)||
			obj->GetName() == object->GetName()) {
			// 同じ名前のオブジェクトが存在する場合は、名前を変更
			copyCount++;
		}
	}
	if (copyCount > 0)
		object->SetName(object->GetName() + std::to_string(copyCount));
	m_Objects.push_back(object);
}

void Editor::DeleteObject(Object* object)
{
	
	m_DeleteObjects.push_back(object);
}

int Editor::AddMaterial(Material* material)
{
	m_Materials.push_back(material);
	return static_cast<int>(m_Materials.size() - 1); // 追加したマテリアルのインデックスを返す
}

