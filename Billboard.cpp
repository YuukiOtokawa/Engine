//==========================================================================
// GM31課題用コメント
// SpriteMeshから派生したクラスで頂点情報のみビルボード用に変更しています
//==========================================================================


#include "Billboard.h"

#include "components/Component_Transform.h"
#include "EngineSystem/MainEngine.h"

void Billboard::Draw() {
	auto transform = owner->GetComponent<Transform>();

	XMMATRIX scale, angle, translation;
	XMMATRIX world;

	//ビューの逆行列
	XMMATRIX invView;
	auto view = Editor::GetInstance()->GetActiveCamera()->GetComponent<Camera>()->GetView();
	invView = XMMatrixInverse(nullptr, view); //逆行列の生成
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

	world = scale * invView * translation;

	MainEngine::GetInstance()->GetRenderer()->SetWorldMatrix(world);

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