#include "RenderQueueManager.h"

#include "IRenderable.h"

#include "MainEngine.h"

#include "Component_Camera.h"

std::map<RenderQueue, std::vector<RenderableEntry>> RenderQueueManager::renderQueues;
bool RenderQueueManager::s_useDeferredRendering = false;

void RenderQueueManager::SetCamera(RenderQueue queue)
{
	switch (queue)
	{
	case Background:
		break;
	case Geometry:
		break;
	case AlphaTest:
		break;
	case Transparent:
		break;
	case Overlay:
		MainEngine::GetInstance()->GetRenderCore()->SetWorldViewProjection2D();
		MainEngine::GetInstance()->GetRenderCore()->SetRasterizerState2D();
		break;
	default:
		break;
	}
}

void RenderQueueManager::Render(Camera* camera)
{
	for (auto& [queue, renderables] : renderQueues) {

		if (queue != RenderQueue::Overlay) {
			MainEngine::GetInstance()->GetRenderCore()->SetRasterizerState3D();
			camera->SetCamera();
		}
		else {
			MainEngine::GetInstance()->GetRenderCore()->SetWorldViewProjection2D();
			MainEngine::GetInstance()->GetRenderCore()->SetRasterizerState2D();
		}

		// デファードレンダリング時は、各オブジェクトの描画前にDeferredGeometryシェーダーを設定
		if (s_useDeferredRendering && queue != RenderQueue::Overlay) {
			MainEngine::GetInstance()->GetRenderCore()->SetPixelShader("DeferredGeometry");
		}

		for (auto& renderable : renderables) {
			renderable.renderable->Draw();

			// デファードレンダリング時は、各オブジェクトの描画後にシェーダーを再設定
			// （各マテリアルが独自のシェーダーを設定してしまうため）
			if (s_useDeferredRendering && queue != RenderQueue::Overlay) {
				MainEngine::GetInstance()->GetRenderCore()->SetPixelShader("DeferredGeometry");
			}
		}
	}
}
