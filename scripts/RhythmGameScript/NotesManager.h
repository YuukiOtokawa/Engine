#pragma once
#include "Script.h"

#include "Notes.h"

class NotesManager :
    public Script
{
    std::vector<Notes> notes;
    int nextNoteID = 0;

    bool isActive = true;
public:
    NotesManager() {
        REGISTER_PROPERTY(isActive, "isActive");
    }

    void Start() override;
    void Update() override;

    void Import(YAML::Node& node) override;
    void Export(YAML::Emitter& node) override;
    const char* GetScriptName() const override;
};

