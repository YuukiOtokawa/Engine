#include "Component.h"
#include "Editor.h"

Component::Component() : EngineMetaFile(CID_None) {
    Editor::GetInstance()->CreateComponent(this);
}
