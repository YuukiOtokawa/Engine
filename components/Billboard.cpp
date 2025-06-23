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

	{
		auto objectScale = transform->GetScale();
		auto objectRotation = transform->GetRotation().ToRadian();
		auto objectPosition = transform->GetPosition();

		scale = XMMatrixScaling(objectScale.x, objectScale.y, objectScale.z);
		angle = XMMatrixRotationRollPitchYaw(objectRotation.x, objectRotation.y, objectRotation.z);
		translation = XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z);
	}

	MainEngine::GetInstance()->GetRenderer()->SetTranslationMatrix(translation);
	MainEngine::GetInstance()->GetRenderer()->SetScaleMatrix(scale);
	MainEngine::GetInstance()->GetRenderer()->SetAngleMatrix(angle);

	//XMMATRIX worldViewProjection;

	//world = XMMatrixTranspose(world);



	//CONSTANTBUFFER cb;
	////XMStoreFloat4x4(&cb.wvp, worldViewProjection);
	//XMStoreFloat4x4(&cb.world, world);

	//MainEngine::GetInstance()->GetRenderer()->SetConstantBuffer(&cb);

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	MainEngine::GetInstance()->GetRenderer()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	MainEngine::GetInstance()->GetRenderer()->GetDeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	MainEngine::GetInstance()->GetRenderer()->GetDeviceContext()->IASetPrimitiveTopology(m_PrimitiveTopology);

}