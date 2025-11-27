#pragma once
#include "Script.h"

#include "TimeSystem.h"
#include "Notes.h"

#include "PrefabAsset.h"

class GameManager :
    public Script
{
    double startTime;

    std::vector<Notes*> notes;
    std::vector<Notes*> removeList;
    int nextNoteID = 0;

    float spawnInterval = 0.0f;

    PrefabAsset notePrefab = {};

public:
    GameManager() {
        REGISTER_PROPERTY(spawnInterval, "spawnInterval");
        REGISTER_PROPERTY(notePrefab, "notePrefab");
    }

    void Start() override;
    void Update() override;

    //void Import(YAML::Node& node) override;
    //void Export(YAML::Emitter& node) override;
    const char* GetScriptName() const override;


    void SpawnNote(double targetTime);


    double GetAudioTime() const {
        return Time::ElapsedTime() - startTime;
    }


    // 曲開始時に呼び出す
    void ResetTime() {
        startTime = Time::ElapsedTime();
    }

    std::vector<Notes*>& GetNotes() {
        return notes;
    }

    void RemoveNotes(Notes* note) {
        removeList.push_back(note);
    }
};

