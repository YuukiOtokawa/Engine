#include "PlayerTest.h"

#include "ScriptFactory.h"

#include "Component_Transform.h"
#include "Component_InputSystem.h"

REGISTER_SCRIPT(PlayerTest)

// ���ꂪ�X�N���v�g��CPP
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
		cameraTransform->SetPosition(cameraTransform->GetPosition() + Vector3O(0, 0, speed) * 2);
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

REGISTERCLASS(PlayerTest);
