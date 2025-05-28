#include "Component_MeshFilter.h"

#include "Component_MeshRenderer.h"

#include "Component_Transform.h"

#include "MainEngine.h"

MeshFilter::MeshFilter(int vertexCount, int indexCount) : m_iVertexCount(vertexCount), m_iIndexCount(indexCount) {
}

void MeshFilter::Update() {

}

void MeshFilter::Draw() {
	auto transform = owner->GetComponent<Transform>();

	XMMATRIX scale, angle, translation;
	XMMATRIX world;

	{
		auto objectScale = transform->GetScale();
		auto objectRotation = transform->GetRotation();
		auto objectPosition = transform->GetPosition();

		scale = XMMatrixScaling(objectScale.x, objectScale.y, objectScale.z);
		angle = XMMatrixRotationRollPitchYaw(objectRotation.x, objectRotation.y, objectRotation.z);
		translation = XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z);
	}

	world = scale * angle * translation;

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

void MeshFilter::SetOwner(Object* owner) {
	this->owner = owner;
	owner->SetDrawable(true);
	//owner->AddComponent<MeshRenderer>();
}

void MeshFilter::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::ObjectTag);
	owner->SetIndexCount(m_iIndexCount);
}