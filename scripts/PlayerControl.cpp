#include "PlayerControl.h"

#include "ScriptFactory.h"

#include "Editor.h"
#include "Component_InputSystem.h"


#include "GameManager.h" 
#include "TimeSystem.h"

REGISTER_SCRIPT(PlayerControl)

void PlayerControl::Start()
{
    gameManager = Editor::GetInstance()->GetObject("GameManager")->GetComponent<GameManager>();
}

void PlayerControl::Update()
{
	auto keyboard = Editor::GetInstance()->GetObject("InputSystem")->GetComponent<InputSystem>()->GetKeyboard();


	if (keyboard->GetKeyDown(KK_SPACE)) {
		Hit();
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

void PlayerControl::Hit()
{
    auto currentTime = Time::ElapsedTime();
    
    Notes* targetNote = nullptr;
    double minDiff = 10000.0;

    for (auto& note : gameManager->GetNotes()) {
		if (!note.IsActive()) continue;
        double diff = std::abs(note.GetTargetTime() - currentTime);

        if (diff < minDiff) {
            minDiff = diff;
            targetNote = &note;
        }
    }


}

REGISTERCLASS(PlayerControl);
