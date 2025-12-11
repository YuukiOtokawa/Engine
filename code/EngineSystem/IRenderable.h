#pragma once

#include "RenderQueueManager.h"

class Camera;
class Object;

class IRenderable {
    RenderQueue m_RenderQueue;
    Object* gameobject;
public:
    IRenderable(RenderQueue queue) : m_RenderQueue(queue) {}

    IRenderable() = delete;

    virtual ~IRenderable() = default;

    void RegisterQueue(Camera* camera);

    virtual void Draw() = 0;

    void SetGameObject(Object* obj) { gameobject = obj; }

    RenderQueue GetRenderQueue() const { return m_RenderQueue; }
};
