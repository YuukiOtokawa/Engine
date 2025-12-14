// ==========================================================================
//
// エディター管理クラス[Editor.cpp]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ==========================================================================

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#define NOMINMAX



#include "Editor.h"

#include "MainEngine.h"

//以下コンポーネントのヘッダー
//省略できるようにしたい
#include "EditorCamera.h"
#include "Component_Transform.h"
#include "Component_MeshRenderer.h"
#include "Component_CubeMesh.h"
#include "PlaneMesh.h"

#include "Component_InputSystem.h"

#include "OBJLoader.h"

#include "Player.h"
#include "PlayerCamera.h"

//#include "Bullet.h"
#include "Billboard.h"
#include "Animation.h"

#include "imgui.h"
#include "ImGuizmo.h"

#include "Pack.h"

#include "Component_CameraPostProcess.h"

#include "EditorViewWindow.h"
#include "GameViewWindow.h"
#include "SystemLog.h"
#include "TimeSystem.h"
#include "EngineConsole.h"
#include "SystemMonitor.h"

#include <numeric>

#include "SceneImporter.h"
#include "ScriptComponent.h"
#include "Prefab.h"
#include "RenderQueueManager.h"

Editor* Editor::m_pInstance;

//==========================================================================
// privateメンバ関数
//==========================================================================

void Editor::UpdateAllObjects()
{
	// エディタカメラは常に更新（エディタモードでもカメラ操作可能にする）
	if (m_pEditorCamera) {
		m_pEditorCamera->Update();
	}
	// InputSystemを更新
	auto inputSystem = GetObject("InputSystem");
	if (inputSystem) {
		inputSystem->Update();
	}

	// プレイモード中のみゲームオブジェクトのUpdate()を呼ぶ
	if (m_isPlaying) {
		for (auto& object : m_Objects) {
			// エディタカメラは既に更新済みなのでスキップ
			if (object->GetComponent<InputSystem>())
				continue;
			if (object != m_pEditorCamera) {
				object->Update();
			}
		}
	}
}

void Editor::DeleteMarkedObjects()
{
	// オブジェクト削除処理（モードに関係なく実行）
	for (auto& object : m_DeleteObjects) {
		auto it = std::find(m_Objects.begin(), m_Objects.end(), object);
		if (it != m_Objects.end()) {
			if (*it == m_pSelectedObject)
				m_pSelectedObject = nullptr; // 選択中のオブジェクトをクリア
			delete* it; // オブジェクトを削除
			m_Objects.erase(it); // リストから削除
		}
	}
	m_DeleteObjects.clear();
}

void Editor::DrawGameViewRTV()
{
	RenderQueueManager::BeginFrame();
	auto renderCore = MainEngine::GetInstance()->GetRenderCore();

	//使用するカメラを設定
	Object* currentCam = nullptr;

	for (auto& object : m_Objects) {
		auto cameraComp = object->GetComponent<Camera>();
		if (cameraComp && cameraComp->IsActiveCamera()) {
			currentCam = object;
		}
	}

	// GameView用の描画（activeCameraを使用）
	if (currentCam) {
		// カメラにポストプロセスコンポーネントがあるかチェック
		auto postProcess = currentCam->GetComponent<CameraPostProcess>();
		bool usePostProcess = postProcess && postProcess->IsEnabled();

		// デファードレンダリングを使用する場合
		if (m_useDeferredRendering) {
			// ジオメトリパス: GBufferに描画
			renderCore->BeginDeferredGeometryPass();
			renderCore->SetPixelShader("DeferredGeometry");
			RenderQueueManager::SetDeferredRendering(true); // デファードモード有効化
			DrawGame(currentCam);
			RenderQueueManager::SetDeferredRendering(false); // デファードモード無効化

			// ライティングパス: GBufferから光源計算して結果を出力
			// ポストプロセスがある場合はPostProcess[0]に、ない場合はGameViewに直接描画
			if (usePostProcess) {
				renderCore->BeginPostProcess(0);
			}
			else {
				renderCore->BeginGameView();
			}

			renderCore->SetWorldViewProjection2D();
			renderCore->SetRasterizerState2D();
			renderCore->SetPixelShader("DeferredLighting");
			renderCore->BeginDeferredLightingPass();
			renderCore->DrawFullScreenQuad(usePostProcess ? renderCore->GetPostProcessRTV(0) : renderCore->GetGameViewRTV(), renderCore->GetGBufferSRV(0));
			// ポストプロセスがある場合は、この後ポストプロセスが適用されるので、ここではラスタライザーステートを戻さない
		}
		else {
			// フォワードレンダリング
			if (usePostProcess) {
				// ポストプロセス使用時: PostProcessバッファに描画
				renderCore->BeginPostProcess(0);
			}
			else {
				// 通常描画: GameViewバッファに直接描画
				renderCore->BeginGameView();
			}

			DrawGame(currentCam);
		}

		// ポストプロセス処理（デファードレンダリングの光源計算の後に適用）
		if (usePostProcess) {
			// ポストプロセスを適用
			renderCore->SetWeight(postProcess->GetWeight());
			renderCore->SetWorldViewProjection2D();
			renderCore->SetRasterizerState2D();

			// Pass 0: PostProcess[0] → PostProcess[1] (垂直ブラー)
			auto material0 = postProcess->GetPostProcessMaterial(0);
			if (material0) {
				material0->SetShader();
				renderCore->DrawFullScreenQuad(renderCore->GetPostProcessRTV(1), renderCore->GetPostProcessSRV(0));
			}

			// Pass 1: PostProcess[1] → GameView (水平ブラー)
			auto material1 = postProcess->GetPostProcessMaterial(1);
			if (material1) {
				material1->SetShader();
				renderCore->DrawFullScreenQuad(renderCore->GetGameViewRTV(), renderCore->GetPostProcessSRV(1));
			}

			renderCore->SetRasterizerState3D();
		}
		else if (m_useDeferredRendering) {
			// デファードレンダリングでポストプロセスがない場合は、ラスタライザーステートを3Dに戻す
			renderCore->SetRasterizerState3D();
		}

		renderCore->ResetRenderTarget();
		renderCore->ResetViewPort();
		renderCore->BufferClear();

	}
}

void Editor::DrawSceneViewRTV()
{
	RenderQueueManager::BeginFrame();
	auto renderCore = MainEngine::GetInstance()->GetRenderCore();

	//使用するカメラの行列情報を登録
	Object* currentCam = m_pEditorCamera;
	if (currentCam)
		currentCam->GetComponent<Camera>()->SetCamera();

	// デファードレンダリングを使用する場合
	if (m_useDeferredRendering) {
		// ジオメトリパス: GBufferに描画
		renderCore->BeginDeferredGeometryPass();
		renderCore->SetPixelShader("DeferredGeometry");
		RenderQueueManager::SetDeferredRendering(true); // デファードモード有効化
	}
	else {
		// フォワードレンダリング
		renderCore->BeginSceneView();
	}

	for (auto& object : m_Objects) {
		if (object->GetTag() == GameObjectTagLayer::LightTag) {
			object->GetComponent<Light>()->SetLight();
		}
	}
	Light::DrawGeneralLight(); // 全体のライト情報を描画


	auto objects = m_Objects;


	// Zソート
	if (currentCam) {
		auto camPos = currentCam->GetComponent<Transform>()->GetPosition().XYZ();
		auto camForward = currentCam->GetComponent<Transform>()->GetForward();

		objects.sort([&](Object* obj1, Object* obj2) {
			if (!currentCam) return false; // activeCameraがnullptrの場合、ソートしない
			if (obj1->GetTag() == GameObjectTagLayer::CameraTag || obj2->GetTag() == GameObjectTagLayer::CameraTag)
				return false; // カメラオブジェクトはソートしない
			if (obj1->GetTag() == GameObjectTagLayer::LightTag || obj2->GetTag() == GameObjectTagLayer::LightTag)
				return false; // ライトオブジェクトはソートしない
			if (!obj1->GetComponent<Transform>() || !obj2->GetComponent<Transform>())
				return false; // Transformコンポーネントがない場合はソートしない

			return obj1->GetComponent<Transform>()->GetZ(camPos, camForward)
			> obj2->GetComponent<Transform>()->GetZ(camPos, camForward);
					 });
	}

	//オブジェクトの描画
	for (auto& object : objects) {
		if (object->GetLayer() == GameObjectLayer::ObjectLayer) {
			if (!object->GetComponent<Transform>())
				continue;
			if (currentCam->GetComponent<Camera>()->IsInView(object->GetComponent<Transform>()->GetPosition().XYZ())) {
				float dist = (currentCam->GetComponent<Transform>()->GetPosition().XYZ() - object->GetComponent<Transform>()->GetPosition().XYZ()).Length();
				auto renderable = object->GetComponent<IRenderable>();
				if (!renderable) continue;
				RenderQueue queue = renderable->GetRenderQueue();
				RenderQueueManager::RegisterRenderQueue(renderable, dist, queue);
			}
		}
	}
	MainEngine::GetInstance()->GetRenderCore()->SetRasterizerState2D();
	for (auto& object : objects) {
		if (object->GetTag() == GameObjectLayer::BillBoardLayer) {
			if (!object->GetComponent<Transform>())
				continue;
			if (currentCam->GetComponent<Camera>()->IsInView(object->GetComponent<Transform>()->GetPosition().XYZ())) {
				float dist = (currentCam->GetComponent<Transform>()->GetPosition().XYZ() - object->GetComponent<Transform>()->GetPosition().XYZ()).Length();
				auto renderable = object->GetComponent<IRenderable>();
				if (!renderable) continue;
				RenderQueue queue = renderable->GetRenderQueue();
				RenderQueueManager::RegisterRenderQueue(renderable, dist, queue);
			}
		}
	}
	//パーティクルの描画
	m_pParticleManager->DrawParticles();

	// 2D描画
	MainEngine::GetInstance()->GetRenderCore()->SetWorldViewProjection2D();
	for (auto& object : objects) {
		if (object->GetLayer() == GameObjectLayer::SpriteLayer) {
			if (object->GetTag() == GameObjectTag::SystemTag)
				continue;
			if (!object->GetComponent<Transform>())
				continue;
			if (currentCam->GetComponent<Camera>()->IsInView(object->GetComponent<Transform>()->GetPosition().XYZ())) {
				float dist = (currentCam->GetComponent<Transform>()->GetPosition().XYZ() - object->GetComponent<Transform>()->GetPosition().XYZ()).Length();
				auto renderable = object->GetComponent<IRenderable>();
				if (!renderable) continue;
				RenderQueue queue = renderable->GetRenderQueue();
				RenderQueueManager::RegisterRenderQueue(renderable, dist, queue);
			}
		}
	}

	// UI描画（最前面）
	MainEngine::GetInstance()->GetRenderCore()->SetWorldViewProjection2D();
	MainEngine::GetInstance()->GetRenderCore()->SetRasterizerState2D();
	for (auto& object : objects) {
		if (object->GetLayer() == GameObjectLayer::UILayer) {
			float dist = (currentCam->GetComponent<Transform>()->GetPosition().XYZ() - object->GetComponent<Transform>()->GetPosition().XYZ()).Length();
			auto renderable = object->GetComponent<IRenderable>();
			if (!renderable) continue;
			RenderQueue queue = renderable->GetRenderQueue();
			RenderQueueManager::RegisterRenderQueue(renderable, dist, queue);
		}
	}

	RenderQueueManager::Render(currentCam->GetComponent<Camera>());

	// デファードレンダリングのライティングパス（ジオメトリパスの後に実行）
	if (m_useDeferredRendering) {
		RenderQueueManager::SetDeferredRendering(false); // デファードモード無効化

		// GBufferから光源計算してSceneViewに描画
		renderCore->BeginSceneView();
		renderCore->SetWorldViewProjection2D();
		renderCore->SetRasterizerState2D();
		renderCore->SetPixelShader("DeferredLighting");
		renderCore->BeginDeferredLightingPass();
		renderCore->DrawFullScreenQuad(renderCore->GetSceneViewRTV(), renderCore->GetGBufferSRV(0));
		// ライティングパス完了後、ラスタライザーステートを3Dに戻す
		renderCore->SetRasterizerState3D();
	}
}

//==========================================================================
// publicメンバ関数
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
	if (m_pProjectWindow) {
		m_pProjectWindow->Finalize();
		delete m_pProjectWindow;
		m_pProjectWindow = nullptr;
	}

	// すべてのオブジェクトを削除
	for (auto& object : m_Objects) {
		delete object;
	}
	m_Objects.clear();
	for (auto& material : m_Materials) {
		if (material)
			delete material;
	}
	m_Materials.clear();
	if (m_pInstance) {
		m_pInstance = nullptr;
	}
    // NodeManagerの解放
    if (m_pNodeManager) {
        delete m_pNodeManager;
        m_pNodeManager = nullptr;
    }
}

void Editor::Initialize() {
	m_pGUI = GUI::GetInstance();
	m_pParticleManager = ParticleManager::GetInstance();
	m_pAudioManager = AudioManager::GetInstance();
	m_pCollisionManager = CollisionManager::GetInstance();

	// シェーダーの読み込み
	{
		// TODO [otokawa]:csoファイルまとめてロードしたいね
		//光源計算無し
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/unlitTexturePS.cso", "unlit");

		//頂点ライティング
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/vertexDirectionalLightingPS.cso", "directional");

		//ピクセルライティング
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/pixelLightingPS.cso", "pixel");

		//BlinnPhongライティング
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/blinnPhongPS.cso", "BlinnPhong");

		//半球ライティング
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/hemisphereLightingPS.cso", "hemisphere");

		//点光源ライティング
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/pointLightingBlinnPhongPS.cso", "pointLight");

		//スポットライトライティング
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/spotLightingPS.cso", "spotLight");

		//リムライトライティング
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/limLightingPS.cso", "limLight");

		//法線マップ
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/bumpPS.cso", "normal");

		//Cook-Torranceライティング
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/cookPS.cso", "CookTorrance");

		//Cook-Torranceライティング
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/PBRPS.cso", "PBR");

		//トゥーンシェーダー
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/toon1PS.cso", "toon1");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/toon2PS.cso", "toon2");

		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/mosaicPS.cso", "mosaic");

		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/RGBShiftPS.cso", "RGBShift");

		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/PosterisePS.cso", "Posterise");

		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/GaussianPS_V.cso", "GaussianPS_V");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/GaussianPS_H.cso", "GaussianPS_H");

		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/WavePS.cso", "Wave");

		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/EnvMapPS.cso", "Enviroment");

		// デファードレンダリング用シェーダー
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/DeferredGeometry.cso", "DeferredGeometry");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/DeferredLighting.cso", "DeferredLighting");

	}

	Main();
	m_pGUI->Initialize();
    m_pNodeManager = new NodeManager(); // NodeManagerのインスタンスを生成
    m_pNodeManager->Initialize(); // NodeManagerを初期化

	// プロジェクトウィンドウの初期化

	m_pEditorCamera = GetObject("EditorCamera");

	SetUseDeferredRendering(true);
}

void Editor::Update() {
	UpdateAllObjects();

    // NodeManagerのUpdateを呼び出す
    if (m_pNodeManager) {
        m_pNodeManager->Update();
    }

	DeleteMarkedObjects();

	// 再生モード中のみパーティクルとコリジョンを更新
	if (m_isPlaying) {
		m_pParticleManager->UpdateParticles();

		m_pCollisionManager->CheckCollisions();
		for (auto& object : m_Objects) {
			object->OnCollision();
		}
	}
}

#include "RenderTexture.h"
void Editor::Draw() {

	auto renderCore = MainEngine::GetInstance()->GetRenderCore();

	for (auto renderTextureComponent : m_Components[CID_Component_RenderTexture]) {
		static_cast<RenderTexture*>(renderTextureComponent)->DrawRenderTexture();
	}

	renderCore->ResetRenderTarget();
	renderCore->ResetViewPort();

	DrawGameViewRTV();
	DrawSceneViewRTV();

	//==========================================================================
	// GUI描画処理
	//==========================================================================

	renderCore->BufferClear();
	MainEngine::GetInstance()->GetRenderCore()->SetWorldViewProjection2D();

	//ImGuiの初期化
	m_pGUI->StartImGui();

	if (m_pNodeManager) {
		m_pNodeManager->Draw();
	}


	GUI::DrawGUI();


	// ImGui描画の終了
	m_pGUI->EndImGui();

	// レンダリングバッファの内容を画面に表示
	MainEngine::GetInstance()->GetRenderCore()->BufferPresent();

}

void Editor::DrawGame(Object* camera, Object* renderTexture)
{
//==========================================================================
// オブジェクト描画処理
//==========================================================================

	camera->GetComponent<Camera>()->SetCamera();

	for (auto& object : m_Objects) {
		if (object->GetTag() == GameObjectTagLayer::LightTag) {
			object->GetComponent<Light>()->SetLight();
		}
	}
	Light::DrawGeneralLight(); // 全体のライト情報を描画

	// オブジェクトをカメラに遠い順にソート
	auto objects = m_Objects;
	// Zソート
	if (camera) {
		auto camPos = camera->GetComponent<Transform>()->GetPosition().XYZ();
		auto camForward = camera->GetComponent<Transform>()->GetForward();

		objects.sort([&](Object* obj1, Object* obj2) {
			if (!camera) return false; // activeCameraがnullptrの場合、ソートしない
			if (obj1->GetTag() == GameObjectTagLayer::CameraTag || obj2->GetTag() == GameObjectTagLayer::CameraTag)
				return false; // カメラオブジェクトはソートしない
			if (obj1->GetTag() == GameObjectTagLayer::LightTag || obj2->GetTag() == GameObjectTagLayer::LightTag)
				return false; // ライトオブジェクトはソートしない
			if (!obj1->GetComponent<Transform>() || !obj2->GetComponent<Transform>())
				return false; // Transformコンポーネントがない場合はソートしない

			return obj1->GetComponent<Transform>()->GetZ(camPos, camForward)
			> obj2->GetComponent<Transform>()->GetZ(camPos, camForward);
					 });
	}

	//オブジェクトの描画
	for (auto& object : objects) {
		if (object->GetLayer() == GameObjectLayer::ObjectLayer) {
			if (!object->GetComponent<Transform>())
				continue;
			if (camera->GetComponent<Camera>()->IsInView(object->GetComponent<Transform>()->GetPosition().XYZ())) {
				float dist = (camera->GetComponent<Transform>()->GetPosition().XYZ() - object->GetComponent<Transform>()->GetPosition().XYZ()).Length();
				auto renderable = object->GetComponent<IRenderable>();
				if (!renderable) continue;
				RenderQueue queue = renderable->GetRenderQueue();
				RenderQueueManager::RegisterRenderQueue(renderable, dist, queue);
			}
				//object->Draw();
		}
	}
	MainEngine::GetInstance()->GetRenderCore()->SetRasterizerState2D();
	for (auto& object : objects) {
		if (object->GetTag() == GameObjectLayer::BillBoardLayer) {
			if (!object->GetComponent<Transform>())
				continue;
			if (camera->GetComponent<Camera>()->IsInView(object->GetComponent<Transform>()->GetPosition().XYZ())) {
				float dist = (camera->GetComponent<Transform>()->GetPosition().XYZ() - object->GetComponent<Transform>()->GetPosition().XYZ()).Length();
				auto renderable = object->GetComponent<IRenderable>();
				if (!renderable) continue;
				RenderQueue queue = renderable->GetRenderQueue();
				RenderQueueManager::RegisterRenderQueue(renderable, dist, queue);
			}
		}
	}
	//パーティクルの描画
	m_pParticleManager->DrawParticles();

	// 2D描画
	MainEngine::GetInstance()->GetRenderCore()->SetWorldViewProjection2D();
	for (auto& object : objects) {
		if (renderTexture && object == renderTexture) continue;
		if (object->GetTag() == GameObjectLayer::SpriteLayer) {
			if (!object->GetComponent<Transform>())
				continue;
			if (camera->GetComponent<Camera>()->IsInView(object->GetComponent<Transform>()->GetPosition().XYZ())) {
				float dist = (camera->GetComponent<Transform>()->GetPosition().XYZ() - object->GetComponent<Transform>()->GetPosition().XYZ()).Length();
				auto renderable = object->GetComponent<IRenderable>();
				if (!renderable) continue;
				RenderQueue queue = renderable->GetRenderQueue();
				RenderQueueManager::RegisterRenderQueue(renderable, dist, queue);
			}
		}
	}

	// UI描画（最前面）
	MainEngine::GetInstance()->GetRenderCore()->SetWorldViewProjection2D();
	MainEngine::GetInstance()->GetRenderCore()->SetRasterizerState2D();
	for (auto& object : objects) {
		if (object->GetLayer() == GameObjectLayer::UILayer) {
			auto renderable = object->GetComponent<IRenderable>();
			if (!renderable) continue;
			RenderQueue queue = renderable->GetRenderQueue();
			RenderQueueManager::RegisterRenderQueue(renderable, 0.0f, queue);
		}
	}

	MainEngine::GetInstance()->GetRenderCore()->SetRasterizerState3D();

	RenderQueueManager::Render(camera->GetComponent<Camera>());

}

void Editor::Finalize() {
	m_pGUI->Finalize();
    // NodeManagerのFinalizeを呼び出す
    if (m_pNodeManager) {
        m_pNodeManager->Finalize();
    }
}

void Editor::Play()
{
	if (m_isPlaying) {
		EngineConsole::LogWarning("Editor::Play: 既に再生中です");
		return;
	}

	// 再生開始前にシーンを一時ファイルに保存
	m_playModeSceneBackup = "temp_playmode_backup.scene";

	EngineConsole::Log("Editor::Play: シーンを保存中...");
	SceneExporter::ExportToFile(m_Objects, m_playModeSceneBackup);
	EngineConsole::Log("Editor::Play: シーンを保存しました");

	m_isPlaying = true;

	EngineConsole::Log("Editor::Play: 再生モードを開始しました");
}

void Editor::Stop()
{
	if (!m_isPlaying) {
		EngineConsole::LogWarning("Editor::Stop: 再生中ではありません");
		return;
	}

	EngineConsole::Log("Editor::Stop: シーンを復元中...");

	// 現在のシーンをクリア
	ResetScene();

	// バックアップからシーンを復元
	if (!m_playModeSceneBackup.empty() && std::filesystem::exists(m_playModeSceneBackup)) {
		std::list<Object*> objects = SceneImporter::Import(m_playModeSceneBackup);
		SetObjects(objects);
		EngineConsole::Log("Editor::Stop: シーンを復元しました");

		// 一時ファイルを削除
		std::filesystem::remove(m_playModeSceneBackup);
		m_playModeSceneBackup = "";
	}

	m_isPlaying = false;
	EngineConsole::Log("Editor::Stop: 再生モードを停止しました");
}

bool Editor::LoadPrefabForEdit(const std::string& filePath)
{
	// 既にPrefab編集中の場合は閉じる
	if (m_isEditingPrefab) {
		CloseEditingPrefab();
	}

	// Prefabをインポート
	Object* prefabObject = Prefab::Import(filePath);
	if (!prefabObject) {
		EngineConsole::LogError("Editor::LoadPrefabForEdit: Prefabの読み込みに失敗しました");
		return false;
	}

	// Prefab編集モードを開始
	m_isEditingPrefab = true;
	m_pEditingPrefabObject = prefabObject;
	m_editingPrefabPath = filePath;
	m_pSelectedObject = prefabObject;

	EngineConsole::Log("Editor::LoadPrefabForEdit: Prefab編集モードを開始しました: %s", filePath.c_str());
	return true;
}

bool Editor::SaveEditingPrefab()
{
	if (!m_isEditingPrefab || !m_pEditingPrefabObject) {
		EngineConsole::LogWarning("Editor::SaveEditingPrefab: Prefab編集中ではありません");
		return false;
	}

	// Prefabを保存
	bool result = Prefab::Export(m_pEditingPrefabObject, m_editingPrefabPath);
	if (result) {
		EngineConsole::Log("Editor::SaveEditingPrefab: Prefabを保存しました: %s", m_editingPrefabPath.c_str());
	}
	else {
		EngineConsole::LogError("Editor::SaveEditingPrefab: Prefabの保存に失敗しました");
	}

	return result;
}

void Editor::CloseEditingPrefab()
{
	if (!m_isEditingPrefab) {
		return;
	}

	// 編集中のPrefabオブジェクトを削除
	if (m_pEditingPrefabObject) {
		DeleteObject(m_pEditingPrefabObject);
		m_pEditingPrefabObject = nullptr;
	}

	// 選択解除
	if (m_pSelectedObject == m_pEditingPrefabObject) {
		m_pSelectedObject = nullptr;
	}

	m_isEditingPrefab = false;
	m_editingPrefabPath = "";

	EngineConsole::Log("Editor::CloseEditingPrefab: Prefab編集モードを終了しました");
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

void Editor::CheckObjectNameConflict(Object* object)
{
	int copyCount = 0;
	for (auto& obj : m_Objects) {
		if (obj->GetName() == object->GetName() + std::to_string(copyCount) ||
			obj->GetName() == object->GetName()) {
			// 同じ名前のオブジェクトが存在する場合は、名前を変更
			copyCount++;
		}
	}
	if (copyCount > 1)
		object->ResolveNameConflict(object->GetName() + std::to_string(copyCount - 1));
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

Object* Editor::GetObjectByFileID(int fileID)
{
	for (auto object : m_Objects)
	{
		if (object->GetFileID() == fileID)
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
	EngineMetaFile::ResetFileIDCounter();
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

#include "CSVImporter.h"
#include "SceneImporter.h"

void Editor::ChangeScene(std::string sceneName)
{
	std::list<Object*> objects;
	Editor::GetInstance()->ResetScene();
	objects = SceneImporter::Import("Scenes\\" + sceneName);
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
	objects = SceneImporter::Import(sceneFilePath);
	if (objects.empty()) {
		MessageBoxA(NULL, "読み込みに失敗しました。", "エラー", MB_OK | MB_ICONERROR);
	}
	else {
		Editor::GetInstance()->SetObjects(objects); // 読み込んだオブジェクトをエディターに設定
	}
}

