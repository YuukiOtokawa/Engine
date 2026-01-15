#include "Editor.h"

#include "MainEngine.h"

#include "Component_Transform.h"
#include "SpriteMesh.h"
#include "Component_MeshRenderer.h"
#include "PlayerCamera.h"
#include "EditorCamera.h"
#include "OBJLoader.h"
#include "Component_InputSystem.h"
#include "Component_CubeMesh.h"
#include "PlaneMesh.h"
#include "Player.h"	
#include "RenderTexture.h"

#include "Particle.h"

#include "Title.h"
#include "Game.h"
#include "Result.h"

#include "TornadoParticle.h"

#include "Pack.h"

#include "Audio.h"

#include "Score.h"


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
		input->SetLayer(GameObjectLayer::InputSystemLayer);
	}
	//エディターカメラ作成
	{
		auto camera = new Object();
		camera->SetName("EditorCamera");
		camera->AddComponent<Transform>();
		camera->AddComponent<EditorCamera>();

		camera->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, -7.5f));
		camera->GetComponent<EditorCamera>()->SetTarget(Vector4O(0.0f, 0.0f, 0.0f));

	}

	//ゲームカメラ作成
	{
		auto camera = new Object();
		camera->SetName("GameCamera");
		camera->AddComponent<Transform>();
		camera->AddComponent<Camera>();

		camera->GetComponent<Transform>()->SetPosition(Vector4O(0.0f, 5.0f, -7.5f));
		camera->GetComponent<Camera>()->SetTarget(Vector4O(0.0f, 0.0f, 0.0f));
		Editor::GetInstance()->SetActiveCamera(camera);

	}

	{
		auto dirctionalLight = new Object();
		dirctionalLight->SetName("DirectionalLight");
		auto transform = dirctionalLight->AddComponent<Transform>();
		auto light = dirctionalLight->AddComponent<Light>();

		transform->SetPosition(Vector4O(0.0f, 10.0f, 0.0f));
		light->SetType(LIGHTTYPE_DIRECTIONAL);
		light->SetDirection(Vector4O(-0.5f, -1.0f, -0.5f));

		dirctionalLight->SetTag(GameObjectTagLayer::LightTag);
	}

	Editor::GetInstance()->OpenScene("Scenes/GY31_Title.yml");
}

