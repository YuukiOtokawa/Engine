#include "Notes.h"
#include "ScriptFactory.h"

#include "Component_Transform.h"

#include "TimeSystem.h"

REGISTER_SCRIPT(Notes)

void Notes::Start()
{
	// 自作Vector3クラス
	direction = Vector3O{
		rand() / (float)RAND_MAX * 2.0f - 1.0f,
		rand() / (float)RAND_MAX * 2.0f - 1.0f,
		rand() / (float)RAND_MAX * 2.0f - 1.0f
	};
	// 正規化できます
	direction.Normalize();
	gameobject->GetComponent<Transform>()->SetPosition(direction * 10.0f);
	remainedTime = targetTime;
}

void Notes::Update()
{
	// 時間管理クラスがあります
	remainedTime -= Time::DeltaTime();

	gameobject->GetComponent<Transform>()->SetPosition(
		direction * 10.0f * (float)(remainedTime / targetTime)
	);
}

void Notes::Import(YAML::Node& node) {
}

void Notes::Export(YAML::Emitter& node) {
}

REGISTERCLASS(Notes);
