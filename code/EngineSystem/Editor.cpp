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

#include "PostProcessTexture.h"
#include "PostProcessRenderer.h"

#include "SystemLog.h"
#include "TimeSystem.h"
#include "EngineConsole.h"
#include "SystemMonitor.h"

#include <numeric>


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
	m_pGUI = GUI::GetInstance();
	m_pParticleManager = ParticleManager::GetInstance();
	m_pAudioManager = AudioManager::GetInstance();
	m_pCollisionManager = CollisionManager::GetInstance();

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

		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/blinnPhongVS.cso", "RGBShift");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/RGBShiftPS.cso", "RGBShift");

		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/blinnPhongVS.cso", "Posterise");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/PosterisePS.cso", "Posterise");

		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/blinnPhongVS.cso", "GaussianVS_V");
		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/blinnPhongVS.cso", "GaussianVS_H");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/GaussianPS_V.cso", "GaussianPS_V");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/GaussianPS_H.cso", "GaussianPS_H");

		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/WaveVS.cso", "Wave");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/WavePS.cso", "Wave");

		MainEngine::GetInstance()->GetRenderCore()->CreateVertexShader("cso/EnvMapVS.cso", "Enviroment");
		MainEngine::GetInstance()->GetRenderCore()->CreatePixelShader("cso/EnvMapPS.cso", "Enviroment");
		
	}

	Main();
	m_pGUI->Initialize();

	m_pEditorCamera = GetObject("EditorCamera");


	m_pPostProcessSprite = new Object(false);
	m_pPostProcessSprite->SetName("PostProcess");

	auto transform = m_pPostProcessSprite->AddComponent<Transform>(false);
	transform->SetPosition({ 0.0f, 0.0f, 0.0f });
	//transform->SetScale({ 100.0f, 100.0f, 1.0f });
	transform->SetScale({ 960.0f, 540.0f, 1.0f });
	auto mesh = m_pPostProcessSprite->AddComponent<SpriteMesh>(false);
	auto renderer = m_pPostProcessSprite->AddComponent<PostProcessRenderer>(false);

	Material* material = new Material();
	material->SetVertexShaderKey("unlit");
	material->SetPixelShaderKey("unlit");
	material->SetTextureEnable(true);
	material->SetTexture(0); // デフォルトの白テクスチャを設定
	renderer->SetMaterial(material);

	auto renderTextureComp = m_pPostProcessSprite->AddComponent<PostProcessTexture>(false);

	m_pPostProcessSprite->SetTag(GameObjectTag::SystemTag);
	m_pPostProcessSprite->SetLayer(GameObjectLayer::SpriteLayer);
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

	m_pCollisionManager->CheckCollisions();
	for (auto& object : m_Objects) {
		object->OnCollision();
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

	//使用するカメラを設定
	Object* activeCamera = nullptr;
	for (auto& object : m_Objects) {
		auto cameraComp = object->GetComponent<Camera>();
		if (cameraComp && cameraComp->IsActiveCamera()) {
			activeCamera = object;
		}
	}

	// GameView用の描画（activeCameraを使用）
	if (activeCamera) {
		renderCore->BeginGameView();
		DrawGame(activeCamera);
		renderCore->ResetRenderTarget();
		renderCore->ResetViewPort();
		renderCore->BufferClear();

	}


	//SceneView用の描画（m_pEditorCameraを使用）
	renderCore->BeginSceneView();
	//==========================================================================
// オブジェクト描画処理
//==========================================================================

	//使用するカメラの行列情報を登録
	if (m_pEditorCamera)
		m_pEditorCamera->Draw();

	for (auto & object : m_Objects) {
		if (object->GetTag() == GameObjectTagLayer::LightTag) {
			object->Draw(); // ライトオブジェクトの描画
		}
	}
	Light::DrawGeneralLight(); // 全体のライト情報を描画

	auto editCam = m_pEditorCamera;

	// オブジェクトをカメラに遠い順にソート
	auto objects = m_Objects;
	objects.sort([editCam](Object* a, Object* b) {
		if (!editCam) return false; // activeCameraがnullptrの場合、ソートしない
		if (a->GetTag() == GameObjectTagLayer::CameraTag || b->GetTag() == GameObjectTagLayer::CameraTag)
			return false; // カメラオブジェクトはソートしない
		if (a->GetTag() == GameObjectTagLayer::LightTag || b->GetTag() == GameObjectTagLayer::LightTag)
			return false; // ライトオブジェクトはソートしない
		if (!a->GetComponent<Transform>() || !b->GetComponent<Transform>())
			return false; // Transformコンポーネントがない場合はソートしない
		auto aPos = a->GetComponent<Transform>()->GetPosition();
		auto bPos = b->GetComponent<Transform>()->GetPosition();
		auto aLength = (aPos - editCam->GetComponent<Transform>()->GetPosition()).Length();
		auto bLength = (bPos - editCam->GetComponent<Transform>()->GetPosition()).Length();
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
		if (object->GetLayer() == GameObjectLayer::SpriteLayer) {
			if (object->GetTag() == GameObjectTag::SystemTag)
				continue;
			object->Draw();
		}
	}



//==========================================================================
// GUI描画処理
//==========================================================================

	//renderCore->BeginPE(1);
	renderCore->BufferClear();
	MainEngine::GetInstance()->GetRenderCore()->SetWorldViewProjection2D();

	//ImGuiの初期化
	m_pGUI->StartImGui();

	m_pGUI->StartGameView();

	{ // GameView
		// GameViewのテクスチャを取得して表示
		auto tex = renderCore->GetGameViewTexture();

		// テクスチャサイズ取得（例: tex->width, tex->height で取得できる場合）
		ImVec2 texSize = ImVec2((float)tex->width, (float)tex->height);
		ImVec2 avail = ImGui::GetWindowSize();

		// 拡大・縮小倍率を計算
		float scaleX = texSize.x / avail.x;
		float scaleY = texSize.y / avail.y;
		float scale = 1.0f;

		if (scaleX < 1.0f) {
			if (scaleY >= 1.0f) {
				scale = 1.0f / scaleX;
			}
			else if (scaleY < 1.0f) {
				scale = std::max(1.0f / scaleX, 1.0f / scaleY);
			}

		}
		else if (scaleY < 1.0f) {
			if (scaleX >= 1.0f) {
				scale = 1.0f / scaleY;
			}
		}
		else {
			scale = 1.0f / std::min(scaleX, scaleY);
		}

		ImVec2 drawSize = ImVec2(texSize.x * scale, texSize.y * scale);

		// テクスチャを中央に配置するためのオフセット計算
		ImVec2 offset = ImVec2(
			(avail.x - drawSize.x) * 0.5f,
			(avail.y - drawSize.y) * 0.5f
		);

		// カーソル位置を中央に移動
		ImGui::SetCursorPos(offset);

		ImGui::Image((ImTextureID)tex->shaderResourceView, drawSize, ImVec2(0, 0), ImVec2(1, 1));
		m_isGameViewHovered = ImGui::IsItemHovered();
	}

	m_pGUI->EndWindow();

	//シーンビューウィンドウの描画開始
	m_pGUI->StartSceneView();

	{ // SceneView
		auto tex = renderCore->GetSceneViewTexture();

		// テクスチャサイズ取得（例: tex->width, tex->height で取得できる場合）
		ImVec2 texSize = ImVec2((float)tex->width, (float)tex->height);
		ImVec2 avail = ImGui::GetWindowSize();

		// 拡大・縮小倍率を計算
		float scaleX = texSize.x / avail.x;
		float scaleY = texSize.y / avail.y;
		float scale = 1.0f;

		if (scaleX < 1.0f) {
			if (scaleY >= 1.0f) {
				scale = 1.0f / scaleX;
			}
			else if (scaleY < 1.0f) {
				scale = std::max(1.0f / scaleX, 1.0f / scaleY);
			}

		}
		else if (scaleY < 1.0f) {
			if (scaleX >= 1.0f) {
				scale = 1.0f / scaleY;
			}
		}
		else {
			scale = 1.0f / std::min(scaleX, scaleY);
		}

		ImVec2 drawSize = ImVec2(texSize.x * scale, texSize.y * scale);

		// テクスチャを中央に配置するためのオフセット計算
		ImVec2 offset = ImVec2(
			(avail.x - drawSize.x) * 0.5f,
			(avail.y - drawSize.y) * 0.5f
		);

		// カーソル位置を中央に移動
		ImGui::SetCursorPos(offset);

		ImGui::Image((ImTextureID)tex->shaderResourceView, drawSize, ImVec2(0, 0), ImVec2(1, 1));
		m_isSceneViewHovered = ImGui::IsItemHovered();

		// ImGuizmo: Scene Viewにギズモとグリッドを表示
		auto camera = m_pEditorCamera->GetComponent<Camera>();

		if (camera) {
			// ImGuizmoの設定
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			// ウィンドウの位置とサイズを取得
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

			// カメラのビュー行列とプロジェクション行列を取得
			XMMATRIX viewMatrix = camera->GetView();
			XMMATRIX projectionMatrix = camera->GetProjection();

			// グリッド表示（XYZ軸の線を表示）
			//XMMATRIX identityMatrix = XMMatrixIdentity();
			//ImGuizmo::DrawGrid(
			//	(float*)&viewMatrix,
			//	(float*)&projectionMatrix,
			//	(float*)&identityMatrix,
			//	100.0f  // グリッドサイズ
			//);

			// ViewManipulate（右上にXYZ軸のビューキューブを表示）
			float viewManipulateSize = 128.0f;
			ImVec2 viewManipulatePos = ImVec2(
				windowPos.x + windowSize.x - viewManipulateSize - 10.0f,
				windowPos.y + 10.0f
			);

			// ViewManipulateのために行列を反転（カメラの向きを正しく表示するため）
			XMMATRIX viewMatrixInverse = XMMatrixInverse(nullptr, viewMatrix);
			ImGuizmo::ViewManipulate(
				(float*)&viewMatrixInverse,
				8.0f,  // カメラからの距離
				viewManipulatePos,
				ImVec2(viewManipulateSize, viewManipulateSize),
				0x10101010  // 背景色
			);

			// 選択中のオブジェクトにギズモを表示
			if (m_pSelectedObject) {
				auto transform = m_pSelectedObject->GetComponent<Transform>();

				if (transform) {
					// キーボード入力で操作モードを切り替え (Scene Viewがホバーされている場合のみ)
					if (m_isSceneViewHovered && !ImGuizmo::IsUsing() && !ImGui::IsKeyDown(ImGuiKey_MouseRight)) {
						if (ImGui::IsKeyPressed(ImGuiKey_W)) {
							m_GizmoOperation = 0; // 移動モード
						}
						else if (ImGui::IsKeyPressed(ImGuiKey_E)) {
							m_GizmoOperation = 1; // 回転モード
						}
						else if (ImGui::IsKeyPressed(ImGuiKey_R)) {
							m_GizmoOperation = 2; // スケールモード
						}
					}

					// Transformからワールド行列を作成
					XMMATRIX translationMatrix = XMMatrixTranslation(
						transform->GetPosition().x,
						transform->GetPosition().y,
						transform->GetPosition().z
					);
					XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
						XMConvertToRadians(transform->GetRotation().x),
						XMConvertToRadians(transform->GetRotation().y),
						XMConvertToRadians(transform->GetRotation().z)
					);
					XMMATRIX scaleMatrix = XMMatrixScaling(
						transform->GetScale().x,
						transform->GetScale().y,
						transform->GetScale().z
					);
					XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

					// 操作モードの選択
					ImGuizmo::OPERATION operation;
					switch (m_GizmoOperation) {
					case 0:
						operation = ImGuizmo::OPERATION::TRANSLATE;
						break;
					case 1:
						operation = ImGuizmo::OPERATION::ROTATE;
						break;
					case 2:
						operation = ImGuizmo::OPERATION::SCALE;
						break;
					default:
						operation = ImGuizmo::OPERATION::TRANSLATE;
						break;
					}

					// ImGuizmoでギズモを操作
					// 回転時はLOCALモードを使用してジンバルロックを軽減
					ImGuizmo::MODE mode = (operation == ImGuizmo::OPERATION::ROTATE)
						? ImGuizmo::MODE::LOCAL
						: ImGuizmo::MODE::WORLD;

					ImGuizmo::Manipulate(
						(float*)&viewMatrix,
						(float*)&projectionMatrix,
						operation,
						mode,
						(float*)&worldMatrix,
						nullptr,
						nullptr
					);

					// ギズモで変更された行列から位置、回転、スケールを抽出
					if (ImGuizmo::IsUsing()) {
						float position[3], rotation[3], scale[3];
						ImGuizmo::DecomposeMatrixToComponents(
							(float*)&worldMatrix,
							position,
							rotation,
							scale
						);

						// Transformに反映
						transform->SetPosition(Vector4O(position[0], position[1], position[2]));
						transform->SetRotation(Vector4O(rotation[0], rotation[1], rotation[2]));
						transform->SetScale(Vector4O(scale[0], scale[1], scale[2]));
					}
				}
			}
		}
	}

	m_pGUI->EndWindow();


	//システムモニターウィンドウの描画開始
	m_pGUI->StartSystemMonitor();

	Time::DrawFPSGraph();
	SystemMonitor::Draw();

	m_pGUI->EndWindow();

	//コンソールウィンドウの描画開始
	m_pGUI->StartConsole();

	EngineConsole::Draw();

	m_pGUI->EndWindow();


	//インスペクタウィンドウの描画開始
	m_pGUI->StartInspector();

	//選択されたオブジェクトの情報を表示
	if (m_pSelectedObject)
		m_pSelectedObject->DrawGUI();

	ImGui::Image(renderCore->GetGameViewTexture()->shaderResourceView, ImVec2(300, 300));
	ImGui::Image(renderCore->GetSceneViewTexture()->shaderResourceView, ImVec2(300, 300));

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
						object->RightClickMenu();
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
		object->RightClickMenu();

		ImGui::PopStyleColor();
	}

	// --- ヒエラルキーの空きスペースの右クリック処理 ---
	// この関数が右クリックを検知してくれます
	if (ImGui::BeginPopupContextWindow("HierarchyWindowContextMenu", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
	{
		if (ImGui::MenuItem("Create Empty Object"))
		{
			new Object();
		}
		ImGui::EndPopup();
	}
		
	m_pGUI->EndWindow();

	// ImGui描画の終了
	m_pGUI->EndImGui();

	// レンダリングバッファの内容を画面に表示
	MainEngine::GetInstance()->GetRenderCore()->BufferPresent();

	auto guiCursor = ImGui::GetIO().MousePos;
	std::string cursorPos = std::to_string(guiCursor.x) + ", " + std::to_string(guiCursor.y);
	SystemLog::Log(0, cursorPos);
	cursorPos = std::to_string(InputSystem::GetMouse()->GetPosition().x) + ", " + std::to_string(InputSystem::GetMouse()->GetPosition().y);
	SystemLog::Log(1, cursorPos);
}

void Editor::DrawGame(Object* camera, Object* renderTexture)
{
//==========================================================================
// オブジェクト描画処理
//==========================================================================

	camera->Draw();

	for (auto& object : m_Objects) {
		if (object->GetTag() == GameObjectTagLayer::LightTag) {
			object->Draw(); // ライトオブジェクトの描画
		}
	}
	Light::DrawGeneralLight(); // 全体のライト情報を描画

	// オブジェクトをカメラに遠い順にソート
	auto objects = m_Objects;
	objects.sort([camera](Object* a, Object* b) {
		if (!camera) return false; // activeCameraがnullptrの場合、ソートしない
		if (a->GetTag() == GameObjectTagLayer::CameraTag || b->GetTag() == GameObjectTagLayer::CameraTag)
			return false; // カメラオブジェクトはソートしない
		if (a->GetTag() == GameObjectTagLayer::LightTag || b->GetTag() == GameObjectTagLayer::LightTag)
			return false; // ライトオブジェクトはソートしない
		if (!a->GetComponent<Transform>() || !b->GetComponent<Transform>())
			return false; // Transformコンポーネントがない場合はソートしない
		auto aPos = a->GetComponent<Transform>()->GetPosition();
		auto bPos = b->GetComponent<Transform>()->GetPosition();
		auto aLength = (aPos - camera->GetComponent<Transform>()->GetPosition()).Length();
		auto bLength = (bPos - camera->GetComponent<Transform>()->GetPosition()).Length();
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
		if (renderTexture && object == renderTexture) continue;
		if (object->GetTag() == GameObjectLayer::SpriteLayer) {
			object->Draw();
		}
	}
	MainEngine::GetInstance()->GetRenderCore()->SetRasterizerState3D();

}

void Editor::Finalize() {
	m_pGUI->Finalize();
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

