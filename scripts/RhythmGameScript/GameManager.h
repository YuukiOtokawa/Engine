#pragma once
#include "Script.h"

#include "TimeSystem.h"
#include "Notes.h"

class GameManager :
    public Script
{
    double startTime;

    std::vector<Notes> notes;
    int nextNoteID = 0;

public:
    GameManager() {
    }

    void Start() override;
    void Update() override;

    void Import(YAML::Node& node) override;
    void Export(YAML::Emitter& node) override;
    const char* GetScriptName() const override;


    void SpawnNote(double targetTime) {

    }


    double GetAudioTime() const {
        return Time::ElapsedTime() - startTime;
    }


    // ã»äJénéûÇ…åƒÇ—èoÇ∑
    void ResetTime() {
        startTime = Time::ElapsedTime();
    }
};

