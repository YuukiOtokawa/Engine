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

Editor* Editor::m_pInstance;

void Editor::Initialize() {
	m_pGUI = new GUI();
	
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


	{
		auto camera = new Object();
		camera->SetName("MainCamera");
		camera->AddComponent<Transform>();
		camera->AddComponent<EditorCamera>();

		camera->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, -2.5f));
		camera->GetComponent<EditorCamera>()->SetTarget(Vector4O(0.0f, 5.0f, -1.5f));

		AddObject(camera);
	}

	{
		auto input = new Object();
		input->SetName("InputSystem");
		input->AddComponent<InputSystem>();

		AddObject(input);
	}

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

	Main();
}

void Editor::Update() {
	//Vector4O rot = GetObject("Cube1")->GetComponent<Transform>()->GetRotation();
	//GetObject("Cube1")->GetComponent<Transform>()->SetRotation(Vector4O(rot.x + 0.01f, rot.y + 0.01f, rot.z + 0.01f));
	//for (int i = 0; i < 5; i++) {
	//	Vector4O torusRot = GetObject("Torus" + std::to_string(i))->GetComponent<Transform>()->GetRotation();
	//	GetObject("Torus" + std::to_string(i))->GetComponent<Transform>()->SetRotation(Vector4O(torusRot.x + 0.01f, torusRot.y + 0.03f, torusRot.z));

	//}
	for (auto& object : m_Objects) {
		object->Update();
	}
}

void Editor::Draw() {
	
	MainEngine::GetInstance()->GetRenderer()->BufferClear();

	std::list<Object*> cameraList;
	for (auto& object : m_Objects) {
		if (object->GetTag() == GameObjectTagLayer::CameraTag) {
			cameraList.push_back(object);
		}
	}

	for (auto& camera : cameraList) {
		camera->Draw();

		for (auto& object : m_Objects) {
			if (object->GetTag() == GameObjectTagLayer::ObjectTag) {
				object->Draw();
			}
		}
	}

	m_pGUI->Start();
	m_pGUI->Draw();
	m_pGUI->End();

	MainEngine::GetInstance()->GetRenderer()->BufferPresent();


}

void Editor::Finalize() {

}

void Editor::AddObject(Object* object)
{
	m_Objects.push_back(object);
}

