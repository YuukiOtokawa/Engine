#include "PlayerControl.h"

#include "ScriptFactory.h"

#include "Editor.h"
#include "Component_InputSystem.h"


#include "GameManager.h" 
#include "TimeSystem.h"

#include "EngineConsole.h"

REGISTER_SCRIPT(PlayerControl)

const double HIT_PERFECT = 0.050;
const double HIT_GREAT = 0.100;
const double HIT_GOOD = 0.200;
const double HIT_MISS = 0.500;

void PlayerControl::Start()
{
    gameManager = dynamic_cast<GameManager*>(Editor::GetInstance()->GetObject("GameManager")->GetComponent<ScriptComponent>()->GetScriptInstance());
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
		if (!note->IsActive()) continue;
        double diff = std::abs(note->GetRemainedTime());

        if (diff < minDiff) {
            minDiff = diff;
            targetNote = note;
        }
    }

    // TODO [otokawa]: 判定取る
    if (targetNote != nullptr && minDiff <= HIT_MISS) {

        if (minDiff <= HIT_PERFECT)
            EngineConsole::Log("PERFECT!");
        else if (minDiff <= HIT_GREAT)
            EngineConsole::Log("GREAT!");
        else if (minDiff <= HIT_GOOD)
            EngineConsole::Log("GOOD!");
        else
            EngineConsole::Log("MISS!!!");
        gameManager->RemoveNotes(targetNote);
    }
}

REGISTERCLASS(PlayerControl);
