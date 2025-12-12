#pragma once

#include "GUIWindow.h"

class HierarchyWindow : public GUIWindow {
    void DragObject(std::list<Object*>* objectList, Object* target);
    void DropSort(std::list<Object*>* objectList, Object* target, bool isRoot);
    void DropChild(std::list<Object*>* objectList, Object* parent);
public:
    void Render() override;
};
