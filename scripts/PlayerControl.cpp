#include "PlayerControl.h"

#include "ScriptFactory.h"

#include "CSVExporter.h"

#include "Editor.h"
#include "Component_InputSystem.h"
#include "Component_Transform.h"

#include "EngineConsole.h"

#include "yaml.h"

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
	if (keyboard->GetKeyRepeat(KK_A)) {
		gameobject->GetComponent<Transform>()->SetPosition(
			gameobject->GetComponent<Transform>()->GetPosition() +
			Vector4O(1.0f, 0.0f, 0.0f) * m_Speed
		);
	}
	if (keyboard->GetKeyRepeat(KK_D)) {
		gameobject->GetComponent<Transform>()->SetPosition(
			gameobject->GetComponent<Transform>()->GetPosition() +
			Vector4O(-1.0f, 0.0f, 0.0f) * m_Speed
		);
	}


	if (keyboard->GetKeyDown(KK_ENTER)) {
		EngineConsole::Log("Enter Key Pressed!!!");
	}
}

void PlayerControl::Import(YAML::Node& node) {
    if (node["speed"]) {
        m_Speed = node["speed"].as<float>();
    }
    if (node["jumpForce"]) {
        m_JumpForce = node["jumpForce"].as<float>();
    }
}

void PlayerControl::Export(YAML::Emitter& node) {
    node << YAML::Key << "speed" << YAML::Value << m_Speed;
    node << YAML::Key << "jumpForce" << YAML::Value << m_JumpForce;
}

REGISTERCLASS(PlayerControl);
