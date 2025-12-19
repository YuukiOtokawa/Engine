#pragma once
#include "Script.h"
class Notes :
    public Script
{
    int noteID = -1;
    double targetTime = 0.0; // ノートがヒットする予定の時間（秒）
    double remainedTime = 0.0;
    bool isActive = true;    // ノートがアクティブかどうか

    // ゼロ初期化してます
    Vector3O direction = Vector3O::Zero();
public:
    Notes() {
        REGISTER_PROPERTY(noteID, "noteID");
        REGISTER_PROPERTY(targetTime, "targetTime");
        REGISTER_PROPERTY(remainedTime, "remainedTime");
    }

    void Start() override;
    void Update() override;

    void Import(YAML::Node& node) override;
    void Export(YAML::Emitter& node) override;
    const char* GetScriptName() const override;

    void SetParameter(int id, double time, bool active) {
        noteID = id;
        targetTime = time;
        isActive = active;
    }

    bool IsActive() const {
        return isActive;
    }

    double GetTargetTime() const {
        return targetTime;
    }

    double GetRemainedTime() const {
        return remainedTime;
    }
};

