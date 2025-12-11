#include "IRenderable.h"

#include "RenderQueueManager.h"
#include "Editor.h"
#include "Component_Transform.h"
#include "Component_Camera.h"

void IRenderable::RegisterQueue(Camera* camera)
{
	float dist = (camera->owner->GetComponent<Transform>()->GetPosition() -
				  gameobject->GetComponent<Transform>()->GetPosition()).Length();
	RenderQueueManager::RegisterRenderQueue(this, dist, m_RenderQueue);
}

