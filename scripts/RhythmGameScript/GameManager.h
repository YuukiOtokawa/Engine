#pragma once
#include "Script.h"

#include "TimeSystem.h"
#include "Notes.h"

#include "PrefabAsset.h"

struct SpawnNoteInfo {
    float spawnTime;
    float toTargetTime; // 目標時間までの時間
};

class GameManager :
    public Script
{
    double startTime;

    std::vector<Notes*> notes;
    std::vector<Notes*> removeList;
    int nextNoteID = 0;

    float spawnInterval = 0.0f;

    PrefabAsset notePrefab = {};

    Object* activeEffect = nullptr;

    PrefabAsset perfectPrefab = {};
    PrefabAsset greatPrefab = {};
    PrefabAsset goodPrefab = {};
    PrefabAsset missPrefab = {};

    CSVFile noteDataFile = { "","" };

    std::vector<SpawnNoteInfo> spawnNoteInfos;
    std::vector<SpawnNoteInfo>::iterator nextSpawnIt;

    SoundFileInfo soundFile = { "","" };

    float soundPlayLag = 0.0f;
    float BeatPerMin = 120.0f;

public:
    GameManager() {
        // unityの[SerializeField]のような役割
        // 変数名とGUI上の表示名を渡す
        REGISTER_PROPERTY(spawnInterval, "spawnInterval");
        REGISTER_PROPERTY(notePrefab, "notePrefab");
        REGISTER_PROPERTY(perfectPrefab, "perfectPrefab");
        REGISTER_PROPERTY(greatPrefab, "greatPrefab");
        REGISTER_PROPERTY(goodPrefab, "goodPrefab");
        REGISTER_PROPERTY(missPrefab, "missPrefab");

        REGISTER_PROPERTY(noteDataFile, "noteDataFile");
        REGISTER_PROPERTY(soundFile, "soundFile");

        REGISTER_PROPERTY(soundPlayLag, "Lag");
        REGISTER_PROPERTY(BeatPerMin, "BPM");
    }

    void Start() override;
    void Update() override;

    //void Import(YAML::Node& node) override;
    //void Export(YAML::Emitter& node) override;
    const char* GetScriptName() const override;

    // ここから下は自由なメンバー関数

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

    void Perfect();
    void Great();
    void Good();
    void Miss();

};

