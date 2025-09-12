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

//#include "Bullet.h"
#include "Billboard.h"
#include "Animation.h"

#include "imgui.h"

#include "../Pack.h"

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
	if (m_pParticleManager) {
		delete m_pParticleManager;
		m_pParticleManager = nullptr;
	}
	if (m_pAudioManager) {
		delete m_pAudioManager;
		m_pAudioManager = nullptr;
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
	m_pAudioManager = AudioManager::GetInstance();

	// シェーダーの読み込み
	{
		//光源計算無し
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/unlitTextureVS.cso", "unlit");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/unlitTexturePS.cso", "unlit");

		//頂点ライティング
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/vertexDirectionalLightingVS.cso", "vertex");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/vertexDirectionalLightingPS.cso", "vertex");

		//ピクセルライティング
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/pixelLightingVS.cso", "pixel");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/pixelLightingPS.cso", "pixel");

		//BlinnPhongライティング
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/blinnPhongVS.cso", "BlinnPhong");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/blinnPhongPS.cso", "BlinnPhong");

		//半球ライティング
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/hemisphereLightingVS.cso", "hemisphere");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/hemisphereLightingPS.cso", "hemisphere");

		//点光源ライティング
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/blinnPhongVS.cso", "pointLight");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/pointLightingBlinnPhongPS.cso", "pointLight");

		//スポットライトライティング
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/spotLightingVS.cso", "spotLight");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/spotLightingPS.cso", "spotLight");

		//リムライトライティング
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/blinnPhongVS.cso", "limLight");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/limLightingPS.cso", "limLight");

		//法線マップ
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/bumpVS.cso", "normal");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/bumpPS.cso", "normal");

		//Cook-Torranceライティング
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/blinnPhongVS.cso", "CookTorrance");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/cookPS.cso", "CookTorrance");

		//Cook-Torranceライティング
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/PBRVS.cso", "PBR");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/PBRPS.cso", "PBR");

		//トゥーンシェーダー
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/blinnPhongVS.cso", "toon1");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/toon1PS.cso", "toon1");
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/blinnPhongVS.cso", "toon2");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/toon2PS.cso", "toon2");

		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/blinnPhongVS.cso", "mosaic");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/mosaicPS.cso", "mosaic");
	}

	MainEngine::GetInstance()->GetRenderCore()->TextureLoad(L"asset/texture/Default_White.png");
	

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
	MainEngine::GetInstance()->GetRenderCore()->BufferClear();

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

	for (auto & object : m_Objects) {
		if (object->GetTag() == GameObjectTagLayer::LightTag) {
			object->Draw(); // ライトオブジェクトの描画
		}
	}
	Light::DrawGeneralLight(); // 全体のライト情報を描画

	// オブジェクトをカメラに遠い順にソート
	auto objects = m_Objects;
	objects.sort([activeCamera](Object* a, Object* b) {
		if (!activeCamera) return false; // activeCameraがnullptrの場合、ソートしない
		if (a->GetTag() == GameObjectTagLayer::CameraTag || b->GetTag() == GameObjectTagLayer::CameraTag)
			return false; // カメラオブジェクトはソートしない
		if (a->GetTag() == GameObjectTagLayer::LightTag || b->GetTag() == GameObjectTagLayer::LightTag)
			return false; // ライトオブジェクトはソートしない
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
		if (object->GetLayer() == GameObjectLayer::ObjectLayer) {
			object->Draw();
		}
	}
	MainEngine::GetInstance()->GetRenderCore()->SetRasterizerState2D();
	for (auto& object : objects) {
		if (object->GetTag() == GameObjectLayer::BillBoardLayer) {
			object->Draw();
		}
	}
	//パーティクルの描画
	m_pParticleManager->DrawParticles();

	// 2D描画
	MainEngine::GetInstance()->GetRenderCore()->SetWorldViewProjection2D();
	for (auto& object : objects) {
		if (object->GetTag() == GameObjectLayer::SpriteLayer) {
			object->Draw();
		}
	}
	MainEngine::GetInstance()->GetRenderCore()->SetRasterizerState3D();

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

		if (object->HasChild()) {
			if (object->IsOpened()) {
				object->SetOpened(ImGui::Button("-")); // 開いている場合は▼を表示
				// 子オブジェクトの描画
				ImGui::Indent(); // 子オブジェクトのインデント
				for (auto& child : object->GetChildren()) {
					if (child) { // nullptrチェック
						if (child == m_pSelectedObject)
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.8f, 1.0f)); // 選択中のオブジェクトの色を変更
						else
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // 通常のオブジェクトの色
						if (ImGui::Button(child->GetName().c_str())) {
							m_pSelectedObject = child; // 選択されたオブジェクトを更新
						}
						ImGui::PopStyleColor();
					}
				}
			}
			else {
				object->SetOpened(ImGui::Button("+")); // 閉じている場合は►を表示
			}
		}

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
	MainEngine::GetInstance()->GetRenderCore()->BufferPresent();

}

void Editor::Finalize() {

}

void Editor::CreateComponent(Component* component) {
	m_Components[component->GetClassID()].push_back(component);
}

void Editor::DeleteComponent(Component* component) {
	auto& compList = m_Components[component->GetClassID()];
	auto it = std::find(compList.begin(), compList.end(), component);
	if (it != compList.end()) {
		compList.erase(it);
		delete component;
	}
}

Component* Editor::GetComponentByFileID(int fileID) {
	for (auto& componentPair : m_Components)
	{
		for (auto& component : componentPair.second)
		{
			if (component->GetFileID() == fileID) {
				return component;
			}
		}
	}
	return nullptr;
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

void Editor::CreateObject()
{
	Object* newObject = new Object();
}

Object* Editor::GetObject(const std::string& name)
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

void Editor::ResetScene()
{
	m_Objects.clear();
	m_Components.clear();
	m_Materials.clear();
	m_VertexIndices.clear();
	MainEngine::GetInstance()->GetRenderCore()->ResetTexture();
}

void Editor::SetActiveCamera(Object* camera)
{
	if (m_pActiveCamera != nullptr)
		m_pActiveCamera->GetComponent<Camera>()->SetActiveCamera(false);

	m_pActiveCamera = camera;
	m_pActiveCamera->GetComponent<Camera>()->SetActiveCamera(true);
}

int Editor::AddMaterial(Material* material)
{
	m_Materials.push_back(material);
	return static_cast<int>(m_Materials.size() - 1); // 追加したマテリアルのインデックスを返す
}

int Editor::AddVertexIndex(VertexIndex* vertexIndex)
{
	for (auto& vi : m_VertexIndices) {
		if (vi->GetFileID() == vertexIndex->GetFileID()) {
			// 同じ名前の頂点とインデックスが存在する場合は、追加しない
			return vi->GetFileID();
		}
	}
	m_VertexIndices.push_back(vertexIndex);
	return vertexIndex->GetFileID(); // 追加した頂点とインデックスのインデックスを返す
}

Material* Editor::GetMaterialByFileID(int fileID) {
	for (auto& material : m_Materials) {
		if (material->GetFileID() == fileID)
			return material;
	}
	return nullptr;
}

VertexIndex* Editor::GetVertexIndexByFileID(int fileID)
{
	for (auto& vertexIndex : m_VertexIndices) {
		if (vertexIndex->GetFileID() == fileID)
			return vertexIndex;
	}
	return nullptr;
}

void Editor::CheckCollision(Object* object)
{
	static float beforeLength = 0.0f;
	float length;
	// ObjectTagがPackTagの物に対して衝突判定
	if (object->GetTag() == GameObjectTagLayer::PackTag) {
		for (auto& obj : m_Objects) {
			if (obj->GetTag() == GameObjectTagLayer::PackTag) {
				// プレイヤーとの衝突判定
				length = (object->GetComponent<Transform>()->GetPosition() - obj->GetComponent<Transform>()->GetPosition()).Length();
				if ((length) < beforeLength && length < 2.0f) {
					// 衝突した場合の処理
					float e = BOUND_CONST;

					// はねかえりの法則
					Vector4O v0_new = (object->GetComponent<Pack>()->GetVelocity() * (200.0f - e * 200.0f) + obj->GetComponent<Pack>()->GetVelocity() * (1.0f + e) * 200.0f) / (200.0f + 200.0f);
					Vector4O v1_new = (obj->GetComponent<Pack>()->GetVelocity() * (200.0f - e * 200.0f) + object->GetComponent<Pack>()->GetVelocity() * (1.0f + e) * 200.0f) / (200.0f + 200.0f);

					object->GetComponent<Pack>()->SetVelocity(v0_new);		// ボール０の速度を更新
					obj->GetComponent<Pack>()->SetVelocity(v1_new);		// ボール1の速度を更新

				}
				beforeLength = length;
			}
		}
	}
}

#include "CSVImporter.h"

void Editor::ChangeScene(std::string sceneName)
{
	std::list<Object*> objects;
	Editor::GetInstance()->ResetScene();
	objects = CSVImporter::Import("Scenes\\" + sceneName);
	if (objects.empty()) {
		MessageBoxA(NULL, "読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
	}
	else {
		Editor::GetInstance()->SetObjects(objects); // 読み込んだオブジェクトをエディターに設定
	}
}

void Editor::OpenScene(std::string sceneFilePath)
{
	std::list<Object*> objects;
	Editor::GetInstance()->ResetScene();
	objects = CSVImporter::Import(sceneFilePath);
	if (objects.empty()) {
		MessageBoxA(NULL, "読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
	}
	else {
		Editor::GetInstance()->SetObjects(objects); // 読み込んだオブジェクトをエディターに設定
	}
}

