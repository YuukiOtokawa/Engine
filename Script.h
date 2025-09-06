#pragma once

class Object;

class Script 
{
    Object* gameobject = nullptr;

    friend class ScriptComponent;
public:
    virtual ~Script() = default;

    virtual void Start() {}

    virtual void Update() {}

    Object* GetGameObject() const { return gameobject; }
};

