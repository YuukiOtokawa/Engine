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
#include "../RenderTexture.h"

#include "../Particle.h"

#include "../Title.h"
#include "../Game.h"
#include "../Result.h"

#include "../TornadoParticle.h"

#include "../Pack.h"

#include "../Audio.h"

#include "../Score.h"


void Main() {
	SetSceneGame();
}


void SetSceneGame()
{
	//入力システム作成
	{
		auto input = new Object();
		input->SetName("InputSystem");
		input->AddComponent<InputSystem>();

	}
	//エディターカメラ作成
	{
		auto camera = new Object();
		camera->SetName("EditorCamera");
		camera->AddComponent<Transform>();
		camera->AddComponent<EditorCamera>();

		camera->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, -7.5f));
		camera->GetComponent<EditorCamera>()->SetTarget(Vector4O(0.0f, 0.0f, 0.0f));
		Editor::GetInstance()->SetActiveCamera(camera);

	}

	//立方体オブジェクト作成
	//{
	//	auto cube = new Object();
	//	cube->SetName("Cube");
	//	cube->AddComponent<Transform>();
	//	cube->AddComponent<CubeMesh>();
	//	cube->AddComponent<MeshRenderer>();

	//	MATERIAL material;
	//	material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
	//	material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);


	//	LIGHT light;

	//	light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
	//	light.Ambient = Vector4O(0.1f, 0.1f, 0.1f, 1.0f);
	//	light.Direction = Vector3O(0.2f, -1.0f, -1.0f);

	//	cube->GetComponent<Transform>()->SetRotation(Vector4O(0.0f, 0.0f, 0.0f));
	//	cube->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 0.0f, 0.0f));
	//	cube->GetComponent<MeshRenderer>()->SetMaterial(material);
	//	cube->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
	//	cube->GetComponent<MeshRenderer>()->SetPixelShader("unlit");

	//}

	auto texture = MainEngine::GetInstance()->GetRenderCore()->TextureLoad(L"asset/texture/sura.jpg");
	auto bumpTexture = MainEngine::GetInstance()->GetRenderCore()->TextureLoad(L"asset/texture/Normal.bmp");
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
		material.SpecularPower = 0.5f;

		LIGHT light;
		light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
		light.Ambient = Vector4O(0.2f, 0.2f, 0.2f, 0.2f);
		light.Direction = Vector3O(0.2f, -1.0f, -1.0f);
		light.Position = Vector3O(0.0f, 2.0f, -0.5f);
		light.PointLightRange = 100.0f;

		plane->GetComponent<Transform>()->SetScale(Vector4O(20.0f, 1.0f, 20.0f));
		plane->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, -5.0f, 0.0f));
		plane->GetComponent<MeshRenderer>()->GetMaterial()->SetTexture(texture);
		plane->GetComponent<MeshRenderer>()->GetMaterial()->SetMaterial(material);
		plane->GetComponent<MeshRenderer>()->GetMaterial()->SetBumpTexture(bumpTexture);

		plane->GetComponent<MeshRenderer>()->GetMaterial()->SetMaterial(material);

		plane->GetComponent<MeshRenderer>()->GetMaterial()->SetVertexShaderKey("unlit");
		plane->GetComponent<MeshRenderer>()->GetMaterial()->SetPixelShaderKey("unlit");

	}

	//スプライトオブジェクト作成
	//{
	//	Object* sprite = new Object();
	//	sprite->SetName("Sprite1");
	//	sprite->AddComponent<Transform>();
	//	sprite->AddComponent<SpriteMesh>();
	//	sprite->AddComponent<MeshRenderer>();

	//	sprite->SetTag(GameObjectTag::ObjectTag);
	//	sprite->SetLayer(GameObjectLayer::ObjectLayer);

	//	MATERIAL material;
	//	material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 0);
	//	material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
	//	material.SpecularPower = 0.5f;

	//	LIGHT light;
	//	light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
	//	light.Ambient = Vector4O(0.2f, 0.2f, 0.2f, 0.2f);
	//	light.Direction = Vector3O(0.2f, -1.0f, -1.0f);
	//	light.Position = Vector3O(0.0f, 2.0f, -0.5f);
	//	light.PointLightRange = 100.0f;

	//	sprite->GetComponent<MeshRenderer>()->GetMaterial()->SetMaterial(material);

	//	sprite->GetComponent<MeshRenderer>()->GetMaterial()->SetVertexShaderKey("unlit");
	//	sprite->GetComponent<MeshRenderer>()->GetMaterial()->SetPixelShaderKey("unlit");

	//	sprite->GetComponent<Transform>()->SetScale(Vector4O(100.0f, 100.0f, 1.0f));

	//	sprite->AddComponent<RenderTexture>();
	//}

	/*
	//プレイヤー追尾カメラ作成
	{
		auto camera = new Object();
		camera->SetName("Camera2");
		camera->AddComponent<Transform>();
		camera->AddComponent<Camera>();
		camera->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, 2.5f));
		camera->GetComponent<Camera>()->SetTarget(Vector4O(0.0f, 5.0f, 1.5f));

		camera->AddComponent<PlayerCamera>();

		//Editor::GetInstance()->AddObject(camera);
	}



	auto texture1 = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/gravel 1.jpg");
	auto toonTexture = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/toonGray.bmp");

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

		model->GetComponent<MeshRenderer>()->SetVertexShader("toon1");
		model->GetComponent<MeshRenderer>()->SetPixelShader("toon1");
		model->GetComponent<MeshRenderer>()->SetToonTexture(toonTexture);

		//Editor::GetInstance()->GetObject("Camera2")->GetComponent<PlayerCamera>()->SetPlayer(model);
		//Editor::GetInstance()->AddObject(model);
	}

	{
		Object* torus = new Object();
		torus->SetName("donut");
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
		torus->GetComponent<Transform>()->SetPosition(Vector4O(5.0f, 0.0f, 5.0f));
		torus->GetComponent<AssimpMeshRenderer>()->SetMaterial(material);
		torus->GetComponent<AssimpMeshRenderer>()->SetTexture(texture1);
		torus->GetComponent<MeshRenderer>()->SetToonTexture(toonTexture);

		torus->GetComponent<AssimpMeshRenderer>()->SetVertexShader("toon1");
		torus->GetComponent<AssimpMeshRenderer>()->SetPixelShader("toon1");
		Editor::GetInstance()->AddObject(torus);
	}
	{
		Object* torus1 = new Object();
		torus1->SetName("donut1");
		torus1->AddComponent<Transform>();
		torus1->AddComponent<MeshFilter>();
		torus1->AddComponent<AssimpMeshRenderer>();

		OBJLoader* loader = new OBJLoader();
		loader->Load("asset\\model\\torus.obj", torus1);

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

		torus1->GetComponent<Transform>()->SetRotation(Vector4O::Zero());
		torus1->GetComponent<Transform>()->SetPosition(Vector4O(-5.0f, 0.0f, 5.0f));
		torus1->GetComponent<AssimpMeshRenderer>()->SetMaterial(material);
		torus1->GetComponent<AssimpMeshRenderer>()->SetTexture(texture1);
		torus1->GetComponent<MeshRenderer>()->SetToonTexture(toonTexture);

		Editor::GetInstance()->AddObject(torus1);
		torus1->GetComponent<AssimpMeshRenderer>()->SetVertexShader("toon2");
		torus1->GetComponent<AssimpMeshRenderer>()->SetPixelShader("toon2");
	}


	int texture2 = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset\\texture\\explosion.png");

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
		sprite->GetComponent<MeshRenderer>()->SetTexture(texture2);

		sprite->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
		sprite->GetComponent<MeshRenderer>()->SetPixelShader("unlit");

		sprite->GetComponent<Animation>()->SetFramePerSecond(30);
		sprite->GetComponent<Billboard>()->SetUVRect(Vector4O(4, 4));

		sprite->SetTag(GameObjectTagLayer::BillBoardTag);

		//Editor::GetInstance()->AddObject(sprite);
	}
	int particleTexture = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/particle.png");
	// パーティクル作成
	{
		Object* particle = new Object();
		particle->SetName("Particle");
		particle->AddComponent<Transform>();
		particle->AddComponent<TornadoParticle>();
		particle->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 0.5f, 20.0f));
		particle->GetComponent<Particle>()->SetTexture(particleTexture);
		particle->GetComponent<Particle>()->SetLifeTime(1000);
		particle->GetComponent<Particle>()->SetAcceleration(Vector4O::Zero());
		particle->GetComponent<Particle>()->SetVelocity(Vector4O::Zero());
		particle->GetComponent<Particle>()->SetSpawnSpan(5);
		//Editor::GetInstance()->AddObject(particle);
	}

	// スポットライトオブジェクト作成
	{
		Object* lightObject = new Object();
		lightObject->SetName("SpotLight");
		lightObject->AddComponent<Transform>();
		lightObject->AddComponent<Light>();

		lightObject->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 10.0f, 0.0f));
		lightObject->GetComponent<Light>()->SetType(LightType::LIGHTTYPE_SPOT);
		lightObject->GetComponent<Light>()->SetDirection(Vector4O(0.0f, -1.0f, 0.0f, 0.0f));
		lightObject->GetComponent<Light>()->SetDiffuse(Vector4O(1.0f, 1.0f, 1.0f, 1.0f));
		lightObject->GetComponent<Light>()->SetAmbient(Vector4O(0.2f, 0.2f, 0.2f, 1.0f));

		lightObject->GetComponent<Light>()->SetEnable(true);
		Editor::GetInstance()->AddObject(lightObject);
	}
	// 点光源オブジェクト作成
	{
		Object* lightObject = new Object();
		lightObject->SetName("PointLight");
		lightObject->AddComponent<Transform>();
		lightObject->AddComponent<Light>();

		lightObject->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 10.0f, 0.0f));
		lightObject->GetComponent<Light>()->SetType(LightType::LIGHTTYPE_POINT);
		lightObject->GetComponent<Light>()->SetDiffuse(Vector4O(1.0f, 1.0f, 1.0f, 1.0f));
		lightObject->GetComponent<Light>()->SetAmbient(Vector4O(0.2f, 0.2f, 0.2f, 1.0f));
		lightObject->GetComponent<Light>()->SetRange(50.0f);
		lightObject->GetComponent<Light>()->SetEnable(true);

		Editor::GetInstance()->AddObject(lightObject);
	}
	// 平行光源オブジェクト作成
	{
		Object* lightObject = new Object();
		lightObject->SetName("DirectionalLight");
		lightObject->AddComponent<Transform>();
		lightObject->AddComponent<Light>();

		lightObject->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 10.0f, 0.0f));
		lightObject->GetComponent<Light>()->SetType(LightType::LIGHTTYPE_DIRECTIONAL);
		lightObject->GetComponent<Light>()->SetDirection(Vector4O(0.0f, -1.0f, 0.0f, 0.0f));
		lightObject->GetComponent<Light>()->SetDiffuse(Vector4O(1.0f, 1.0f, 1.0f, 1.0f));
		lightObject->GetComponent<Light>()->SetAmbient(Vector4O(0.2f, 0.2f, 0.2f, 1.0f));
		lightObject->GetComponent<Light>()->SetEnable(true);
		Editor::GetInstance()->AddObject(lightObject);
	}

	//パン作成
	//for (int i = 0; i < 3; i++)
	{
		Object* donut = new Object();
		donut->SetName("Bread");
		donut->AddComponent<Transform>();
		donut->AddComponent<MeshFilter>();
		donut->AddComponent<AssimpMeshRenderer>();
		ModelLoader* loader = new ModelLoader();
		loader->LoadModel( donut, "asset\\model\\bread.fbx");

		donut->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, 0.0f));

		donut->GetComponent<MeshRenderer>()->SetMaterial(new Material());

		donut->GetComponent<MeshRenderer>()->SetVertexShader("toon2");	
		donut->GetComponent<MeshRenderer>()->SetPixelShader("toon2");
		int mainTex = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset\\texture\\PlainBread_Diffuse_4K.png");
		int bump = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset\\texture\\PlainBread_Nomal.png");

		donut->GetComponent<MeshRenderer>()->SetTexture(mainTex);
		donut->GetComponent<MeshRenderer>()->SetBumpTexture(bump);
		donut->GetComponent<MeshRenderer>()->SetToonTexture(toonTexture);

		//Editor::GetInstance()->AddObject(donut);
	}

	// スカイボックスオブジェクト作成
	{
		Object* skyBox = new Object();
		skyBox->SetName("SkyBox");
		skyBox->AddComponent<Transform>();
		skyBox->AddComponent<MeshFilter>();
		skyBox->AddComponent<MeshRenderer>();
		OBJLoader* loader = new OBJLoader();
		loader->Load("asset\\model\\sky.obj", skyBox);

		skyBox->GetComponent<Transform>()->SetScale(Vector4O(100.0f, 100.0f, 100.0f));
		skyBox->GetComponent<MeshRenderer>()->SetPixelShader("unlit");
		skyBox->GetComponent<MeshRenderer>()->SetVertexShader("unlit");

		int skyTexture = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/sky.jpg");

		skyBox->GetComponent<MeshRenderer>()->SetTexture(skyTexture);

		Editor::GetInstance()->AddObject(skyBox);
	}

	// オーディオシステム作成
	{
		auto audio = new Object();
		audio->SetName("BGM");
		audio->AddComponent<Audio>();

		audio->GetComponent<Audio>()->LoadAudio("asset/sound/BGM/maou_bgm_orchestra24_1.wav");
		audio->GetComponent<Audio>()->SetVolume(0.1f);
		//audio->GetComponent<Audio>()->Play();

		Editor::GetInstance()->AddObject(audio);
	}

	auto scoreTexture = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/number.png");

	// スコア表示スプライト
	{
		auto scoreSprite = new Object();
		scoreSprite->SetName("ScoreSprite1p");
		scoreSprite->AddComponent<Transform>();
		scoreSprite->AddComponent<SpriteMesh>();
		scoreSprite->AddComponent<MeshRenderer>();

		scoreSprite->GetComponent<Transform>()->SetPosition(Vector4O(500.0f, -450.0f, 0.0f));
		scoreSprite->GetComponent<Transform>()->SetScale(Vector4O(100.0f, -100.0f, 1.0f));
		scoreSprite->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
		scoreSprite->GetComponent<MeshRenderer>()->SetPixelShader("unlit");
		scoreSprite->GetComponent<MeshRenderer>()->SetTexture(scoreTexture);
		scoreSprite->GetComponent<SpriteMesh>()->SetUVRect(Vector4O(5, 5));
		scoreSprite->GetComponent<SpriteMesh>()->SetSpriteByIndex(0);

		Editor::GetInstance()->AddObject(scoreSprite);
	}
	{
		auto scoreSprite = new Object();
		scoreSprite->SetName("ScoreSprite10p");
		scoreSprite->AddComponent<Transform>();
		scoreSprite->AddComponent<SpriteMesh>();
		scoreSprite->AddComponent<MeshRenderer>();

		scoreSprite->GetComponent<Transform>()->SetPosition(Vector4O(400.04, -450.0f, 0.0f));
		scoreSprite->GetComponent<Transform>()->SetScale(Vector4O(100.0f, -100.0f, 1.0f));
		scoreSprite->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
		scoreSprite->GetComponent<MeshRenderer>()->SetPixelShader("unlit");
		scoreSprite->GetComponent<MeshRenderer>()->SetTexture(scoreTexture);
		scoreSprite->GetComponent<SpriteMesh>()->SetUVRect(Vector4O(5, 5));
		scoreSprite->GetComponent<SpriteMesh>()->SetSpriteByIndex(0);

		Editor::GetInstance()->AddObject(scoreSprite);
	}
	{
		auto scoreSprite = new Object();
		scoreSprite->SetName("ScoreSprite100p");
		scoreSprite->AddComponent<Transform>();
		scoreSprite->AddComponent<SpriteMesh>();
		scoreSprite->AddComponent<MeshRenderer>();

		scoreSprite->GetComponent<Transform>()->SetPosition(Vector4O(300.0f, -450.0f, 0.0f));
		scoreSprite->GetComponent<Transform>()->SetScale(Vector4O(100.0f, -100.0f, 1.0f));
		scoreSprite->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
		scoreSprite->GetComponent<MeshRenderer>()->SetPixelShader("unlit");
		scoreSprite->GetComponent<MeshRenderer>()->SetTexture(scoreTexture);
		scoreSprite->GetComponent<SpriteMesh>()->SetUVRect(Vector4O(5, 5));
		scoreSprite->GetComponent<SpriteMesh>()->SetSpriteByIndex(0);

		Editor::GetInstance()->AddObject(scoreSprite);
	}

	// スコア管理オブジェクト
	{
		auto scoreManager = new Object();
		scoreManager->SetName("ScoreManager");
		scoreManager->AddComponent<Score>();
		scoreManager->SetDrawable(false); // スコア管理オブジェクトは描画しない
		Editor::GetInstance()->AddObject(scoreManager);
	}

	
	{
		auto toonTextureSprite = new Object();
		toonTextureSprite->SetName("toonTextureSprite");
		toonTextureSprite->AddComponent<Transform>();
		toonTextureSprite->AddComponent<SpriteMesh>();
		toonTextureSprite->AddComponent<MeshRenderer>();

		toonTextureSprite->GetComponent<Transform>()->SetPosition(Vector4O(-500.0f, -450.0f, 0.0f));
		toonTextureSprite->GetComponent<Transform>()->SetScale(Vector4O(100.0f, -100.0f, 1.0f));
		toonTextureSprite->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
		toonTextureSprite->GetComponent<MeshRenderer>()->SetPixelShader("unlit");
		toonTextureSprite->GetComponent<MeshRenderer>()->SetTexture(toonTexture);
		toonTextureSprite->GetComponent<SpriteMesh>()->SetUVRect(Vector4O(1, 1));
		toonTextureSprite->GetComponent<SpriteMesh>()->SetSpriteByIndex(0);

		Editor::GetInstance()->AddObject(toonTextureSprite);
	}
	*/
}

