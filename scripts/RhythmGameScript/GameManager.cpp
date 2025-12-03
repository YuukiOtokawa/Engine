#include "GameManager.h"
#include "ScriptFactory.h"

#include "Component_Transform.h"

REGISTER_SCRIPT(GameManager)

void GameManager::Start()
{
}

void GameManager::Update()
{
	spawnInterval -= Time::DeltaTime();
	if (spawnInterval <= 0.0f) {
		SpawnNote(2.0); // 2秒先にスポーン
		spawnInterval = 0.5f; // 次のスポーンまでのインターバルをリセット
	}

	// removeListに入っているノーツを削除
	for (auto& note : removeList) {
		auto it = std::find(notes.begin(), notes.end(), note);
		if (it != notes.end()) {
			notes.erase(it);
			note->gameobject->Destroy();
		}
	}

	for (auto& note : notes) {
		if (!note->IsActive()) continue;

		if (note->GetRemainedTime() < -0.2) {
			// TODO [otokawa]: Miss判定
			EngineConsole::Log("TimeOver!!!");
			RemoveNotes(note);
		}
	}
}

//void GameManager::Import(YAML::Node& node) {
//}
//
//void GameManager::Export(YAML::Emitter& node) {
//}

void GameManager::SpawnNote(double spawnAheadTime)
{
	auto currentTime = Time::ElapsedTime();
	auto targetTime = currentTime + spawnAheadTime;

	//auto object = new Object();

	// TODO [otokawa]:スクリプト追加処理簡単にしたいね
	//auto script = object->AddComponent<ScriptComponent>();
	//auto note = dynamic_cast<Notes*>(ScriptFactory::GetInstance().CreateScript("Notes"));
	//object->AddComponent<Transform>();
	//script->SetScript(note);
	auto note = notePrefab.Instantiate();
	auto instance = dynamic_cast<Notes*>(note->GetComponent<ScriptComponent>()->GetScriptInstance());
	instance->SetParameter(nextNoteID++, spawnAheadTime, true);
	notes.push_back(instance);
}

REGISTERCLASS(GameManager);
