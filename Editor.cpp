#include "Editor.h"

#include "MainEngine.h"

#include "EditorCamera.h"
#include "Component_Transform.h"
#include "Component_MeshRenderer.h"
#include "Component_CubeMesh.h"
#include "PlaneMesh.h"
#include "AssimpMeshRenderer.h"

#include "Component_InputSystem.h"

#include "ModelLoader.h"
#include "OBJLoader.h"

#include "imgui.h"

Editor* Editor::m_pInstance;

void Editor::Initialize() {
	m_pGUI = new GUI();
	
	{
		//光源計算無し
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("unlitTextureVS.cso", "unlit");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("unlitTexturePS.cso", "unlit");

		//頂点ライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("vertexDirectionalLightingVS.cso", "vertex");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("vertexDirectionalLightingPS.cso", "vertex");

		//ピクセルライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("pixelLightingVS.cso", "pixel");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("pixelLightingPS.cso", "pixel");

		//BlinnPhongライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("blinnPhongVS.cso", "BlinnPhong");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("blinnPhongPS.cso", "BlinnPhong");

		//半球ライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("hemisphereLightingVS.cso", "hemisphere");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("hemisphereLightingPS.cso", "hemisphere");

		//点光源ライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("blinnPhongVS.cso", "pointLight");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("pointLightingBlinnPhongPS.cso", "pointLight");

		//スポットライトライティング
		MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("spotLightingVS.cso", "spotLight");
		MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("spotLightingPS.cso", "spotLight");
	}

	//カメラ作成
	{
		auto camera = new Object();
		camera->SetName("MainCamera");
		camera->AddComponent<Transform>();
		camera->AddComponent<EditorCamera>();

		camera->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, -2.5f));
		camera->GetComponent<EditorCamera>()->SetTarget(Vector4O(0.0f, 5.0f, -1.5f));

		camera->GetComponent<EditorCamera>()->SetActiveCamera(true);
		AddObject(camera);
	}

	//カメラ2作成
	{
		auto camera = new Object();
		camera->SetName("Camera2");
		camera->AddComponent<Transform>();
		camera->AddComponent<Camera>();
		camera->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, 2.5f));
		camera->GetComponent<Camera>()->SetTarget(Vector4O(0.0f, 5.0f, 1.5f));
		AddObject(camera);
	}

	//入力システム作成
	{
		auto input = new Object();
		input->SetName("InputSystem");
		input->AddComponent<InputSystem>();

		AddObject(input);
	}

	//立方体オブジェクト作成
	{
		auto cube = new Object();
		cube->SetName("Cube1");
		cube->AddComponent<Transform>();
		cube->AddComponent<CubeMesh>();
		cube->AddComponent<MeshRenderer>();

		MATERIAL material;
		material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


		LIGHT light;

		light.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		light.Direction = XMFLOAT4(0.2f, -1.0f, -1.0f, 0.0f);

		cube->GetComponent<Transform>()->SetRotation(Vector4O(2.0f, 0.0f, 2.0f));
		cube->GetComponent<Transform>()->SetPosition(Vector4O(5.0f, 0.0f, 0.0f));
		cube->GetComponent<MeshRenderer>()->SetMaterial(material);
		cube->GetComponent<MeshRenderer>()->SetLight(light);

		//AddObject(cube);
	}

	auto texture = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/sura.jpg");

	//平面オブジェクト作成
	{
		Object* plane = new Object();
		plane->SetName("Plane1");
		plane->AddComponent<Transform>();
		plane->AddComponent<PlaneMesh>();
		plane->AddComponent<MeshRenderer>();

		MATERIAL material;
		material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


		LIGHT light;

		light.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		light.Direction = XMFLOAT4(0.2f, -1.0f, -1.0f, 0.0f);

		plane->GetComponent<Transform>()->SetScale(Vector4O(20.0f, 1.0f, 20.0f));
		plane->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, -5.0f, 0.0f));
		plane->GetComponent<MeshRenderer>()->SetMaterial(material);
		plane->GetComponent<MeshRenderer>()->SetLight(light);
		plane->GetComponent<MeshRenderer>()->SetTexture(texture);

		plane->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
		plane->GetComponent<MeshRenderer>()->SetPixelShader("unlit");

		AddObject(plane);
	}

	auto texture1 = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/gravel 1.jpg");

	//プレイヤーモデルオブジェクト作成
	{
		Object* model = new Object();
		model->SetName("Model0");
		model->AddComponent<Transform>();
		model->AddComponent<MeshFilter>();
		model->AddComponent<MeshRenderer>();

		OBJLoader* loader = new OBJLoader();
		loader->Load("asset\\model\\player.obj", model);

		MATERIAL material;
		material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


		LIGHT light;

		light.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		light.Direction = XMFLOAT4(0.2f, -1.0f, -1.0f, 0.0f);

		model->GetComponent<MeshRenderer>()->SetMaterial(material);
		model->GetComponent<MeshRenderer>()->SetLight(light);

		model->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
		model->GetComponent<MeshRenderer>()->SetPixelShader("unlit");
		model->GetComponent<MeshRenderer>()->SetTexture(texture1);


		AddObject(model);
	}

	//トーラスオブジェクト作成
	{
		Object* torus;

		for (int i = 0; i < 5; i++) {
			torus = new Object();
			torus->SetName("Torus" + std::to_string(i));
			torus->AddComponent<Transform>();
			torus->AddComponent<MeshFilter>();
			torus->AddComponent<AssimpMeshRenderer>();

			ModelLoader* loader = new ModelLoader();
			loader->LoadModel(torus, "asset\\model\\torus.obj");

			MATERIAL material;
			material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			LIGHT light;
			light.Direction = XMFLOAT4(0.2f, -1.0f, -1.0f, 0.0f);
			light.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
			light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
			light.Position = Vector4O(0.0f, 2.0f, -0.5f, 1.0f);
			light.PointLightRange = Vector4O(10.0f, 0.0f, 0.0f, 0.0f);

			torus->GetComponent<Transform>()->SetRotation(Vector4O(2.0f * i, 2.0f * i, 2.0f * i));
			torus->GetComponent<Transform>()->SetPosition(Vector4O(-5.0f + i * 2.5f, -5.0f + i * 2.f, -5.f + i * 2.f));
			torus->GetComponent<AssimpMeshRenderer>()->SetMaterial(material);
			torus->GetComponent<AssimpMeshRenderer>()->SetLight(light);
			torus->GetComponent<AssimpMeshRenderer>()->SetTexture(texture1);

			torus->GetComponent<AssimpMeshRenderer>()->SetVertexShader("BlinnPhong");
			torus->GetComponent<AssimpMeshRenderer>()->SetPixelShader("BlinnPhong");
			AddObject(torus);
		}

		//スポットライト用トーラス
		{
			Object* torus = new Object();
			torus->SetName("SpotLightTorus");
			torus->AddComponent<Transform>();
			torus->AddComponent<MeshFilter>();
			torus->AddComponent<AssimpMeshRenderer>();

			ModelLoader* loader = new ModelLoader();
			loader->LoadModel(torus, "asset\\model\\torus.obj");

			MATERIAL material;
			material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			LIGHT light;
			light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
			light.Ambient = Vector4O(0.2f, 0.2f, 0.2f, 1.0f);
			light.Direction = Vector4O(0.0f, -1.0f, 0.0f, 0.0f); // スポットライトの方向
			light.Position = Vector4O(0.0f, 1.0f, 0.0f, 1.0f); // スポットライトの位置
			light.PointLightRange = Vector4O(2000.0f, 1.5f, 0.0f, 0.0f); // スポットライトの範囲
			light.SpotLightAngle = Vector4O((Vector4O::PI / 180.0f) * 30.0f, 0.0f, 0.0f, 0.0f); // スポットライトの角度

			torus->GetComponent<Transform>()->SetRotation(Vector4O::Zero());
			torus->GetComponent<Transform>()->SetPosition(Vector4O::Zero());
			torus->GetComponent<AssimpMeshRenderer>()->SetMaterial(material);
			torus->GetComponent<AssimpMeshRenderer>()->SetLight(light);
			torus->GetComponent<AssimpMeshRenderer>()->SetTexture(texture1);

			torus->GetComponent<AssimpMeshRenderer>()->SetVertexShader("spotLight");
			torus->GetComponent<AssimpMeshRenderer>()->SetPixelShader("spotLight");
			//AddObject(torus);
		}
	}

	Main();
	m_pGUI->Initialize();

}

void Editor::Update() {
	for (auto& object : m_Objects) 
		object->Update();
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
	activeCamera->Draw();

	//オブジェクトの描画
	for (auto& object : m_Objects) {
		if (object->GetTag() == GameObjectTagLayer::ObjectTag) {
			object->Draw();
		}
	}

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
}

void Editor::Finalize() {

}

void Editor::AddObject(Object* object)
{
	m_Objects.push_back(object);
}

