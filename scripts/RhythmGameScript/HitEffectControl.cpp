#include "HitEffectControl.h"

#include "ScriptFactory.h"

#include "TimeSystem.h"

REGISTER_SCRIPT(HitEffectControl)

void HitEffectControl::Start()
{
    // Initialize your script here
}

void HitEffectControl::Update()
{
	effectRemainedTime -= Time::DeltaTime();
	if (effectRemainedTime <= 0.0) {
		// エフェクトの寿命が尽きたらオブジェクトを削除
		gameobject->Destroy();
	}
}


void HitEffectControl::Import(YAML::Node& node)
{
}
void HitEffectControl::Export(YAML::Emitter& node)
{
}
REGISTERCLASS(HitEffectControl);
