#include "GameManager.h"
#include "ScriptFactory.h"

REGISTER_SCRIPT(GameManager)

void GameManager::Start()
{
}

void GameManager::Update()
{
}

void GameManager::Import(YAML::Node& node) {
}

void GameManager::Export(YAML::Emitter& node) {
}

void GameManager::SpawnNote(double spawnAheadTime)
{
	auto currentTime = Time::ElapsedTime();
	auto targetTime = currentTime + spawnAheadTime;

	auto object = new Object();
	auto note = object->AddComponent<Notes>();
	note->SetParameter(nextNoteID++, targetTime, true);
	notes.push_back(*note);
}

REGISTERCLASS(GameManager);
