#include "PlayerTest.h"

#include "ScriptFactory.h"

#include "Component_Transform.h"
#include "Component_InputSystem.h"

REGISTER_SCRIPT(PlayerTest)

// これがスクリプトのCPP
void PlayerTest::Start()
{
	auto camera = Editor::GetInstance()->GetObject("GameCamera");
	cameraTransform = camera->GetComponent<Transform>();
	playerTransform = gameobject->GetComponent<Transform>();
}

void PlayerTest::Update()
{
	auto keyboard = Editor::GetInstance()->GetObject("InputSystem")->GetComponent<InputSystem>()->GetKeyboard();
	if (keyboard->GetKeyRepeat(KK_W)) {
		cameraTransform->SetPosition(cameraTransform->GetPosition() + Vector3O(0, 0, speed));
		playerTransform->SetPosition(playerTransform->GetPosition() + Vector3O(0, 0, speed));
	}
	if (keyboard->GetKeyRepeat(KK_S)) {
		cameraTransform->SetPosition(cameraTransform->GetPosition() + Vector3O(0, 0, -speed));
		playerTransform->SetPosition(playerTransform->GetPosition() + Vector3O(0, 0, -speed));
	}
	if (keyboard->GetKeyRepeat(KK_A)) {
		cameraTransform->SetPosition(cameraTransform->GetPosition() + Vector3O(-speed, 0, 0));
		playerTransform->SetPosition(playerTransform->GetPosition() + Vector3O(-speed, 0, 0));
	}
	if (keyboard->GetKeyRepeat(KK_D)) {
		cameraTransform->SetPosition(cameraTransform->GetPosition() + Vector3O(speed, 0, 0));
		playerTransform->SetPosition(playerTransform->GetPosition() + Vector3O(speed, 0, 0));
	}
}

void PlayerTest::Import(YAML::Node& node) {
	if (node["speed"]) {
		speed = node["speed"].as<float>();
	}
}

void PlayerTest::Export(YAML::Emitter& node) {
	node << YAML::Key << "speed" << YAML::Value << speed;
}

REGISTERCLASS(PlayerTest);
