#pragma once
#include "Script.h"
class Notes :
    public Script
{
    int noteID = -1;
    double targetTime = 0.0; // ノートがヒットする予定の時間（秒）
    bool isActive = true;    // ノートがアクティブかどうか

public:
    Notes() {
        REGISTER_PROPERTY(noteID, "noteID");
        REGISTER_PROPERTY(targetTime, "targetTime");
        REGISTER_PROPERTY(isActive, "isActive");
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


};

