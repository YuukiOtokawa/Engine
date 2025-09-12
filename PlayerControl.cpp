#include "PlayerControl.h"

#include "ScriptFactory.h"

#include "CSVExporter.h"

#include "Editor.h"
#include "Component_InputSystem.h"
#include "Component_Transform.h"

REGISTER_SCRIPT(PlayerControl)



void PlayerControl::Start()
{
}

void PlayerControl::Update()
{
	auto keyboard = Editor::GetInstance()->GetObject("InputSystem")->GetComponent<InputSystem>()->GetKeyboard();

	Vector3O direction = Vector3O::Zero();

	if (keyboard->GetKeyRepeat(KK_W)) {
		gameobject->GetComponent<Transform>()->SetPosition(
			gameobject->GetComponent<Transform>()->GetPosition() +
			Vector4O(0.0f, 0.0f, 1.0f) * m_Speed
		);
	}
	if (keyboard->GetKeyRepeat(KK_S)) {
		gameobject->GetComponent<Transform>()->SetPosition(
			gameobject->GetComponent<Transform>()->GetPosition() +
			Vector4O(0.0f, 0.0f, -1.0f) * m_Speed
		);
	}
}

void PlayerControl::Export() {
    CSVExporter::ExportFloat(m_Speed);
}
