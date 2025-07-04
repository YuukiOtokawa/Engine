#include "Editor.h"

#include "MainEngine.h"

#include "Component_Transform.h"
#include "SpriteMesh.h"
#include "Component_MeshRenderer.h"
#include "PlayerCamera.h"
#include "EditorCamera.h"
#include "OBJLoader.h"
#include "ModelLoader.h"
#include "Component_InputSystem.h"
#include "Component_CubeMesh.h"
#include "PlaneMesh.h"
#include "Player.h"	
#include "AssimpMeshRenderer.h"

#include "../Title.h"
#include "../Game.h"
#include "../Result.h"


void Main() {
	SetSceneTitle();
}

void SetSceneTitle()
{
	//エディターカメラ作成
	{
		auto camera = new Object();
		camera->SetName("MainCamera");
		camera->AddComponent<Transform>();
		camera->AddComponent<EditorCamera>();

		camera->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, -2.5f));
		camera->GetComponent<EditorCamera>()->SetTarget(Vector4O(0.0f, 5.0f, -1.5f));
		Editor::GetInstance()->SetActiveCamera(camera);

		Editor::GetInstance()->AddObject(camera);
	}
	//入力システム作成
	{
		auto input = new Object();
		input->SetName("InputSystem");
		input->AddComponent<InputSystem>();

		Editor::GetInstance()->AddObject(input);
	}
	//UIスプライト作成
	{
		Object* sprite = new Object();
		sprite->SetName("UISprite");
		sprite->AddComponent<Transform>();
		sprite->AddComponent<SpriteMesh>();
		sprite->AddComponent<MeshRenderer>();
		sprite->AddComponent<Title>();
		MATERIAL material;
		material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
		material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
		LIGHT light;
		light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
		light.Ambient = Vector4O(0.1f, 0.1f, 0.1f, 1.0f);
		light.Direction = Vector4O(0.2f, -1.0f, -1.0f, 0.0f);
		sprite->GetComponent<Transform>()->SetScale(Vector4O(1920.0f, 1080.0f, 1.0f));
		sprite->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 0.0f, 0.0f));
		sprite->GetComponent<MeshRenderer>()->SetMaterial(material);
		sprite->GetComponent<MeshRenderer>()->SetLight(light);
		light.Enable = false; // UIでは光源を無効にする

		ID3D11ShaderResourceView* texture3 = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/title.png");
		sprite->GetComponent<MeshRenderer>()->SetTexture(texture3);
		sprite->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
		sprite->GetComponent<MeshRenderer>()->SetPixelShader("unlit");

		Editor::GetInstance()->AddObject(sprite);
	}
}

void SetSceneGame()
{
	//エディターカメラ作成
	{
		auto camera = new Object();
		camera->SetName("MainCamera");
		camera->AddComponent<Transform>();
		camera->AddComponent<EditorCamera>();

		camera->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, -2.5f));
		camera->GetComponent<EditorCamera>()->SetTarget(Vector4O(0.0f, 5.0f, -1.5f));
		Editor::GetInstance()->SetActiveCamera(camera);

		Editor::GetInstance()->AddObject(camera);
	}

	//プレイヤー追尾カメラ作成
	{
		auto camera = new Object();
		camera->SetName("Camera2");
		camera->AddComponent<Transform>();
		camera->AddComponent<Camera>();
		camera->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, 2.5f));
		camera->GetComponent<Camera>()->SetTarget(Vector4O(0.0f, 5.0f, 1.5f));

		camera->AddComponent<PlayerCamera>();

		Editor::GetInstance()->AddObject(camera);
	}

	//入力システム作成
	{
		auto input = new Object();
		input->SetName("InputSystem");
		input->AddComponent<InputSystem>();

		Editor::GetInstance()->AddObject(input);
	}

	//立方体オブジェクト作成
	{
		auto cube = new Object();
		cube->SetName("Cube1");
		cube->AddComponent<Transform>();
		cube->AddComponent<CubeMesh>();
		cube->AddComponent<MeshRenderer>();

		MATERIAL material;
		material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
		material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);


		LIGHT light;

		light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
		light.Ambient = Vector4O(0.1f, 0.1f, 0.1f, 1.0f);
		light.Direction = Vector4O(0.2f, -1.0f, -1.0f, 0.0f);

		cube->GetComponent<Transform>()->SetRotation(Vector4O(2.0f, 0.0f, 2.0f));
		cube->GetComponent<Transform>()->SetPosition(Vector4O(5.0f, 0.0f, 0.0f));
		cube->GetComponent<MeshRenderer>()->SetMaterial(material);
		cube->GetComponent<MeshRenderer>()->SetLight(light);

		//AddObject(cube);
	}

	auto texture = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/sura.jpg");
	auto bumpTexture = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/Normal.bmp");
	//平面オブジェクト作成
	{
		Object* plane = new Object();
		plane->SetName("Plane1");
		plane->AddComponent<Transform>();
		plane->AddComponent<PlaneMesh>();
		plane->AddComponent<MeshRenderer>();

		MATERIAL material;
		material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
		material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);

		LIGHT light;
		light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
		light.Ambient = Vector4O(0.2f, 0.2f, 0.2f, 0.2f);
		light.Direction = Vector4O(0.2f, -1.0f, -1.0f, 0.0f);
		light.Position = Vector4O(0.0f, 2.0f, -0.5f, 1.0f);
		light.PointLightRange = Vector4O(100.0f, 0.0f, 0.0f, 0.0f);

		plane->GetComponent<Transform>()->SetScale(Vector4O(20.0f, 1.0f, 20.0f));
		plane->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, -5.0f, 0.0f));
		plane->GetComponent<MeshRenderer>()->SetMaterial(material);
		plane->GetComponent<MeshRenderer>()->SetLight(light);
		plane->GetComponent<MeshRenderer>()->SetTexture(texture);
		plane->GetComponent<MeshRenderer>()->SetBumpTexture(bumpTexture);

		plane->GetComponent<MeshRenderer>()->SetVertexShader("normal");
		plane->GetComponent<MeshRenderer>()->SetPixelShader("normal");

		Editor::GetInstance()->AddObject(plane);
	}

	auto texture1 = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/gravel 1.jpg");

	//プレイヤーモデルオブジェクト作成
	{
		Object* model = new Object();
		model->SetName("Model0");
		model->AddComponent<Transform>();
		model->AddComponent<MeshFilter>();
		model->AddComponent<MeshRenderer>();
		model->AddComponent<Player>();
		model->AddComponent<Game>();

		OBJLoader* loader = new OBJLoader();
		loader->Load("asset\\model\\player.obj", model);

		MATERIAL material;
		material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
		material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);


		LIGHT light;

		light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
		light.Ambient = Vector4O(0.1f, 0.1f, 0.1f, 1.0f);
		light.Direction = Vector4O(0.2f, -1.0f, -1.0f, 0.0f);

		model->GetComponent<MeshRenderer>()->SetMaterial(material);
		model->GetComponent<MeshRenderer>()->SetLight(light);

		model->GetComponent<MeshRenderer>()->SetVertexShader("limLight");
		model->GetComponent<MeshRenderer>()->SetPixelShader("limLight");
		//model->GetComponent<MeshRenderer>()->SetTexture(texture1);

		Editor::GetInstance()->GetObject("Camera2")->GetComponent<PlayerCamera>()->SetPlayer(model);
		Editor::GetInstance()->AddObject(model);
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
			material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
			material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);

			LIGHT light;
			light.Direction = Vector4O(0.2f, -1.0f, -1.0f, 0.0f);
			light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
			light.Ambient = Vector4O(0.1f, 0.1f, 0.1f, 1.0f);
			light.Position = Vector4O(0.0f, 2.0f, -0.5f, 1.0f);
			light.PointLightRange = Vector4O(10.0f, 0.0f, 0.0f, 0.0f);

			torus->GetComponent<Transform>()->SetRotation(Vector4O(2.0f * i, 2.0f * i, 2.0f * i));
			torus->GetComponent<Transform>()->SetPosition(Vector4O(-5.0f + i * 2.5f, -5.0f + i * 2.f, -5.f + i * 2.f));
			torus->GetComponent<AssimpMeshRenderer>()->SetMaterial(material);
			torus->GetComponent<AssimpMeshRenderer>()->SetLight(light);
			torus->GetComponent<AssimpMeshRenderer>()->SetTexture(texture1);

			torus->GetComponent<AssimpMeshRenderer>()->SetVertexShader("BlinnPhong");
			torus->GetComponent<AssimpMeshRenderer>()->SetPixelShader("BlinnPhong");
			//AddObject(torus);
		}

		{
			torus = new Object();
			torus->SetName("LimTorus");
			torus->AddComponent<Transform>();
			torus->AddComponent<MeshFilter>();
			torus->AddComponent<AssimpMeshRenderer>();

			ModelLoader* loader = new ModelLoader();
			loader->LoadModel(torus, "asset\\model\\torus.obj");

			MATERIAL material;
			material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
			material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);

			LIGHT light;
			light.Direction = Vector4O(0.2f, -1.0f, -1.0f, 0.0f);
			light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
			light.Ambient = Vector4O(0.1f, 0.1f, 0.1f, 1.0f);
			light.Position = Vector4O(0.0f, 2.0f, -0.5f, 1.0f);
			light.PointLightRange = Vector4O(10.0f, 0.0f, 0.0f, 0.0f);

			torus->GetComponent<Transform>()->SetPosition(Vector4O(5.0f, 0.0f, 5.0f));
			torus->GetComponent<AssimpMeshRenderer>()->SetMaterial(material);
			torus->GetComponent<AssimpMeshRenderer>()->SetLight(light);
			torus->GetComponent<AssimpMeshRenderer>()->SetTexture(texture1);

			torus->GetComponent<AssimpMeshRenderer>()->SetVertexShader("limLight");
			torus->GetComponent<AssimpMeshRenderer>()->SetPixelShader("limLight");
			Editor::GetInstance()->AddObject(torus);

		}

		//スポットライト用トーラス
		{
			Object* torus = new Object();
			torus->SetName("SpotLightTorus");
			torus->AddComponent<Transform>();
			torus->AddComponent<MeshFilter>();
			torus->AddComponent<AssimpMeshRenderer>();

			OBJLoader* loader = new OBJLoader();
			loader->Load("asset\\model\\torus.obj", torus);

			MATERIAL material;
			material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
			material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);

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
			Editor::GetInstance()->AddObject(torus);
		}
		//ポイントライト用トーラス
		{
			Object* torus = new Object();
			torus->SetName("PointTorus");
			torus->AddComponent<Transform>();
			torus->AddComponent<MeshFilter>();
			torus->AddComponent<AssimpMeshRenderer>();

			OBJLoader* loader = new OBJLoader();
			loader->Load("asset\\model\\torus.obj", torus);

			MATERIAL material;
			material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
			material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);

			LIGHT light;
			light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
			light.Ambient = Vector4O(0.2f, 0.2f, 0.2f, 1.0f);
			light.Direction = Vector4O(0.0f, -1.0f, 0.0f, 0.0f); // スポットライトの方向
			light.Position = Vector4O(0.0f, 1.0f, 0.0f, 1.0f); // スポットライトの位置
			light.PointLightRange = Vector4O(2000.0f, 1.5f, 0.0f, 0.0f); // スポットライトの範囲
			light.SpotLightAngle = Vector4O((Vector4O::PI / 180.0f) * 30.0f, 0.0f, 0.0f, 0.0f); // スポットライトの角度

			torus->GetComponent<Transform>()->SetRotation(Vector4O::Zero());
			torus->GetComponent<Transform>()->SetPosition(Vector4O(-5.0f, 0.0f, 5.0f));
			torus->GetComponent<AssimpMeshRenderer>()->SetMaterial(material);
			torus->GetComponent<AssimpMeshRenderer>()->SetLight(light);
			torus->GetComponent<AssimpMeshRenderer>()->SetTexture(texture1);

			torus->GetComponent<AssimpMeshRenderer>()->SetVertexShader("pointLight");
			torus->GetComponent<AssimpMeshRenderer>()->SetPixelShader("pointLight");
			//AddObject(torus);
		}
		//クックトランス用トーラス
		{
			Object* torus = new Object();
			torus->SetName("CookTorus");
			torus->AddComponent<Transform>();
			torus->AddComponent<MeshFilter>();
			torus->AddComponent<AssimpMeshRenderer>();

			OBJLoader* loader = new OBJLoader();
			loader->Load("asset\\model\\torus.obj", torus);

			MATERIAL material;
			material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
			material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);

			LIGHT light;
			light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
			light.Ambient = Vector4O(0.2f, 0.2f, 0.2f, 1.0f);
			light.Direction = Vector4O(0.0f, -1.0f, 0.0f, 0.0f); // スポットライトの方向
			light.Position = Vector4O(0.0f, 1.0f, 0.0f, 1.0f); // スポットライトの位置
			light.PointLightRange = Vector4O(2000.0f, 1.5f, 0.0f, 0.0f); // スポットライトの範囲
			light.SpotLightAngle = Vector4O((Vector4O::PI / 180.0f) * 30.0f, 0.0f, 0.0f, 0.0f); // スポットライトの角度

			torus->GetComponent<Transform>()->SetRotation(Vector4O::Zero());
			torus->GetComponent<Transform>()->SetPosition(Vector4O(-5.0f, 0.0f, 5.0f));
			torus->GetComponent<AssimpMeshRenderer>()->SetMaterial(material);
			torus->GetComponent<AssimpMeshRenderer>()->SetLight(light);
			torus->GetComponent<AssimpMeshRenderer>()->SetTexture(texture1);

			torus->GetComponent<AssimpMeshRenderer>()->SetVertexShader("CookTorrance");
			torus->GetComponent<AssimpMeshRenderer>()->SetPixelShader("CookTorrance");
			Editor::GetInstance()->AddObject(torus);
		}
	}

	ID3D11ShaderResourceView* texture2 = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset\\texture\\explosion.png");

	//アニメーションスプライト作成
	{
		Object* sprite = new Object();
		sprite->SetName("sprite");
		sprite->AddComponent<Transform>();
		sprite->AddComponent<Billboard>();
		sprite->AddComponent<MeshRenderer>();
		sprite->AddComponent<Animation>();

		MATERIAL material;
		material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
		material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);

		LIGHT light;

		light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
		light.Ambient = Vector4O(0.1f, 0.1f, 0.1f, 1.0f);
		light.Direction = Vector4O(0.2f, -1.0f, -1.0f, 0.0f);

		sprite->GetComponent<Transform>()->SetScale(Vector4O(1.0f, 1.0f, 1.0f));
		sprite->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 1.0f, 5.0f));
		sprite->GetComponent<MeshRenderer>()->SetMaterial(material);
		sprite->GetComponent<MeshRenderer>()->SetLight(light);
		sprite->GetComponent<MeshRenderer>()->SetTexture(texture2);

		sprite->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
		sprite->GetComponent<MeshRenderer>()->SetPixelShader("unlit");

		sprite->GetComponent<Animation>()->SetFramePerSecond(30);
		sprite->GetComponent<Billboard>()->SetUVRect(Vector4O(4, 4));

		sprite->SetTag(GameObjectTagLayer::BillBoardTag);

		Editor::GetInstance()->AddObject(sprite);
	}
}

void SetSceneResult()
{
	//エディターカメラ作成
	{
		auto camera = new Object();
		camera->SetName("MainCamera");
		camera->AddComponent<Transform>();
		camera->AddComponent<EditorCamera>();

		camera->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, -2.5f));
		camera->GetComponent<EditorCamera>()->SetTarget(Vector4O(0.0f, 5.0f, -1.5f));
		Editor::GetInstance()->SetActiveCamera(camera);

		Editor::GetInstance()->AddObject(camera);
	}
	//入力システム作成
	{
		auto input = new Object();
		input->SetName("InputSystem");
		input->AddComponent<InputSystem>();

		Editor::GetInstance()->AddObject(input);
	}
	//UIスプライト作成
	{
		Object* sprite = new Object();
		sprite->SetName("UISprite");
		sprite->AddComponent<Transform>();
		sprite->AddComponent<SpriteMesh>();
		sprite->AddComponent<MeshRenderer>();
		sprite->AddComponent<Result>();
		MATERIAL material;
		material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
		material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
		LIGHT light;
		light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
		light.Ambient = Vector4O(0.1f, 0.1f, 0.1f, 1.0f);
		light.Direction = Vector4O(0.2f, -1.0f, -1.0f, 0.0f);
		sprite->GetComponent<Transform>()->SetScale(Vector4O(1920.0f, 1080.0f, 1.0f));
		sprite->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 0.0f, 0.0f));
		sprite->GetComponent<MeshRenderer>()->SetMaterial(material);
		sprite->GetComponent<MeshRenderer>()->SetLight(light);
		light.Enable = false; // UIでは光源を無効にする

		ID3D11ShaderResourceView* texture3 = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/result.png");
		sprite->GetComponent<MeshRenderer>()->SetTexture(texture3);
		sprite->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
		sprite->GetComponent<MeshRenderer>()->SetPixelShader("unlit");

		Editor::GetInstance()->AddObject(sprite);
	}
}
