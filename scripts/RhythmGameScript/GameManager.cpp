#include "GameManager.h"
#include "ScriptFactory.h"

// 必要なものをインクルード
// 後々必要なくしたい
#include "Component_Transform.h"

#include "CSVImporter.h"

#include "Audio.h"

#include "Script.h"

REGISTER_SCRIPT(GameManager)

void GameManager::Start()
{
	auto list = CSVImporter::Import(noteDataFile.filepath);

	spawnNoteInfos.clear();

	for (auto line : list) {
		auto it = line.begin();
		if (it == line.end()) continue;
		float spawnTime = std::stof(*it);
		++it;
		if (it == line.end()) continue;
		float toTargetTime = std::stof(*it);
		spawnNoteInfos.push_back({ spawnTime, toTargetTime });
	}

	// spawnNoteInfosをspawnTimeでソート
	std::sort(spawnNoteInfos.begin(), spawnNoteInfos.end(),
		[](const SpawnNoteInfo& a, const SpawnNoteInfo& b) {
			return a.spawnTime < b.spawnTime;
			  });

	nextSpawnIt = spawnNoteInfos.begin();
	spawnInterval = 0.0f;

	auto audioComp = gameobject->GetComponent<Audio>();
	audioComp->LoadAudio(soundFile.filepath.c_str());
	audioComp->SetVolume(0.5f);
	audioComp->PlayOneShot();
}

void GameManager::Update()
{
	spawnInterval += Time::DeltaTime();
	if (nextSpawnIt != spawnNoteInfos.end()) {
		while (spawnInterval >= nextSpawnIt->spawnTime) {
			SpawnNote(nextSpawnIt->toTargetTime);
			++nextSpawnIt;
			if (nextSpawnIt == spawnNoteInfos.end()) {
				break;
			}
		}
	}

	// removeListに入っているノーツを削除
	for (auto& note : removeList) {
		auto it = std::find(notes.begin(), notes.end(), note);
		if (it != notes.end()) {
			notes.erase(it);
			// Destroy()でゲームオブジェクトを削除
			note->gameobject->Destroy();
		}
	}

	for (auto& note : notes) {
		if (!note->IsActive()) continue;

		if (note->GetRemainedTime() < -0.2) {
			// TODO [otokawa]: Miss判定
			// Debug.Log()
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

	auto note = notePrefab.Instantiate();
	auto instance = dynamic_cast<Notes*>(note->GetComponent<ScriptComponent>()->GetScriptInstance());
	instance->SetParameter(nextNoteID++, spawnAheadTime, true);
	notes.push_back(instance);
}

void GameManager::Perfect()
{
	if (activeEffect != nullptr)
		activeEffect->Destroy();
	activeEffect = perfectPrefab.Instantiate();
}

void GameManager::Great()
{
	if (activeEffect != nullptr)
		activeEffect->Destroy();
	activeEffect = greatPrefab.Instantiate();
}

void GameManager::Good()
{
	if (activeEffect != nullptr)
		activeEffect->Destroy();
	activeEffect = goodPrefab.Instantiate();
}

void GameManager::Miss()
{
	if (activeEffect != nullptr)
		activeEffect->Destroy();
	activeEffect = missPrefab.Instantiate();
}

REGISTERCLASS(GameManager);
