#include "Editor.h"

#include "MainEngine.h"

#include "Component_Camera.h"
#include "Component_Transform.h"
#include "Component_MeshRenderer.h"
#include "Component_CubeMesh.h"
#include "PlaneMesh.h"
#include "AssimpMeshRenderer.h"

#include "ModelLoader.h"

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
		camera->AddComponent<Camera>();

		camera->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, -2.5f));

		AddObject(camera);
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

	auto texture = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/sura.jpg");
	for (int i=0;i<5;i++)
	{
		auto torus = new Object();
		torus->SetName("Torus" + std::to_string(i));
		torus->AddComponent<Transform>();
		torus->AddComponent<MeshFilter>();
		torus->AddComponent<AssimpMeshRenderer>();

		auto loader = new ModelLoader();
		loader->LoadModel(torus, "asset/model/model.fbx");
		
		MATERIAL material;
		material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		


		LIGHT light;

		light.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		light.Direction = XMFLOAT4(0.2f, -1.0f, -1.0f, 0.0f);

		torus->GetComponent<Transform>()->SetPosition(Vector4O(-6.0f + 3.0f * i, 0.0f, 0.0f));
		torus->GetComponent<MeshRenderer>()->SetMaterial(material);
		torus->GetComponent<MeshRenderer>()->SetLight(light);
		torus->GetComponent<MeshRenderer>()->SetTexture(texture);

		switch (i)
		{
		case 0:
			torus->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
			torus->GetComponent<MeshRenderer>()->SetPixelShader("unlit");
			break;
		case 1:
			torus->GetComponent<MeshRenderer>()->SetVertexShader("vertex");
			torus->GetComponent<MeshRenderer>()->SetPixelShader("vertex");
			break;
		case 2:
			torus->GetComponent<MeshRenderer>()->SetVertexShader("pixel");
			torus->GetComponent<MeshRenderer>()->SetPixelShader("pixel");
			break;
		case 3:
			torus->GetComponent<MeshRenderer>()->SetVertexShader("BlinnPhong");
			torus->GetComponent<MeshRenderer>()->SetPixelShader("BlinnPhong");
			break;
		case 4:
			torus->GetComponent<MeshRenderer>()->SetVertexShader("hemisphere");
			torus->GetComponent<MeshRenderer>()->SetPixelShader("hemisphere");
			light.Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f); // 半球ライティングは上方向を向く
			light.SkyColor = XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f);
			light.GroundColor = XMFLOAT4(0.0f, 0.6f, 0.0f, 1.0f); // 半球ライティングの空と地面の色を設定
			light.GroundNormal = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f); // 半球ライティングの地面の法線を設定
			XMVECTOR temp = XMLoadFloat4(&light.GroundNormal);
			temp = XMVector4Normalize(temp);
			XMStoreFloat4(&light.GroundNormal, temp);
			torus->GetComponent<MeshRenderer>()->SetLight(light);

			break;
		default:
			break;
		}

		AddObject(torus);
	}

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

	Main();
}

void Editor::Update() {
	//Vector4O rot = GetObject("Cube1")->GetComponent<Transform>()->GetRotation();
	//GetObject("Cube1")->GetComponent<Transform>()->SetRotation(Vector4O(rot.x + 0.01f, rot.y + 0.01f, rot.z + 0.01f));
	for (int i = 0; i < 5; i++) {
		Vector4O torusRot = GetObject("Torus" + std::to_string(i))->GetComponent<Transform>()->GetRotation();
		GetObject("Torus" + std::to_string(i))->GetComponent<Transform>()->SetRotation(Vector4O(torusRot.x + 0.01f, torusRot.y + 0.03f, torusRot.z));

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

