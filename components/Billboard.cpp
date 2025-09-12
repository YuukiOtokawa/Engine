#include "Billboard.h"

#include "Component_Transform.h"
#include "MainEngine.h"

void Billboard::Draw() {
	auto transform = owner->GetComponent<Transform>();

	XMMATRIX scale, angle, translation;

	
	XMMATRIX invView;
	auto view = Editor::GetInstance()->GetActiveCamera()->GetComponent<Camera>()->GetView();
	invView = XMMatrixInverse(nullptr, view);
	invView.r[3].m128_f32[0] = 0.0f;
	invView.r[3].m128_f32[1] = 0.0f;
	invView.r[3].m128_f32[2] = 0.0f;

	// 行列式を確認して、必要に応じて補正
	XMVECTOR det = XMMatrixDeterminant(invView);
	float determinant = XMVectorGetX(det);
	if (determinant < 0.0f) {
		// 行列式が負の場合、スケールのX軸を反転させて巻き方向を補正
		XMMATRIX flipMatrix = XMMatrixScaling(-1.0f, 1.0f, 1.0f);
		invView = XMMatrixMultiply(flipMatrix, invView);
	}

	{
		auto objectScale = transform->GetScale();
		auto objectRotation = transform->GetRotation().ToRadian();
		auto objectPosition = transform->GetPosition();

		scale = XMMatrixScaling(objectScale.x, objectScale.y, objectScale.z);
		//angle = XMMatrixRotationRollPitchYaw(objectRotation.x, objectRotation.y, objectRotation.z);
		angle = invView;
		translation = XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z);
	}

	MainEngine::GetInstance()->GetRenderCore()->SetTranslationMatrix(translation);
	MainEngine::GetInstance()->GetRenderCore()->SetScaleMatrix(scale);
	MainEngine::GetInstance()->GetRenderCore()->SetAngleMatrix(angle);

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetPrimitiveTopology(m_PrimitiveTopology);

}