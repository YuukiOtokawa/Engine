#include "Editor.h"

#include "MainEngine.h"

#include "Component_Camera.h"
#include "Component_Transform.h"
#include "Component_MeshRenderer.h"
#include "Component_CubeMesh.h"
#include "AssimpMeshRenderer.h"

#include "ModelLoader.h"

Editor* Editor::m_pInstance;

void Editor::Initialize() {
	m_pGUI = new GUI();
	
	
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
		cube->GetComponent<MeshRenderer>()->SetMaterial(material);
		cube->GetComponent<MeshRenderer>()->SetLight(light);

		AddObject(cube);
	}

	{
		auto torus = new Object();
		torus->SetName("Torus1");
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

		//torus->GetComponent<Transform>()->SetRotation(Vector4O(2.0f, 0.0f, 2.0f));
		torus->GetComponent<MeshRenderer>()->SetMaterial(material);
		torus->GetComponent<MeshRenderer>()->SetLight(light);

	}
	
}

void Editor::Update() {
	Vector4O rot = GetObject("Cube1")->GetComponent<Transform>()->GetRotation();
	GetObject("Cube1")->GetComponent<Transform>()->SetRotation(Vector4O(rot.x + 0.01f, rot.y + 0.01f, rot.z + 0.01f));
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
