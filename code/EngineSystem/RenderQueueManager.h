#pragma once

#include <map>
#include <vector>

enum RenderQueue {
    Background = 1000,
    Geometry = 2000,
    AlphaTest = 2450,
    Transparent = 3000,
    Overlay = 4000
};

class IRenderable;
struct RenderableEntry {
    IRenderable* renderable;
    float distance;
};

class Camera;

class RenderQueueManager {
    
    static std::map<RenderQueue, std::vector<RenderableEntry>> renderQueues;
    static void SetCamera(RenderQueue queue);
public:
    static void RegisterRenderQueue(IRenderable* renderable, float dist, RenderQueue queue) {
        renderQueues[queue].push_back(RenderableEntry(renderable,dist));
    }

    static void Render(Camera* camera);

    static void BeginFrame() {
        renderQueues.clear();
    }
};
