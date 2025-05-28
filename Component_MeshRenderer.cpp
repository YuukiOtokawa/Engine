#include "Component_MeshRenderer.h"

#include "Component_Camera.h"
#include "Component_Transform.h"

#include "MainEngine.h"

MeshRenderer::MeshRenderer()
{
	m_pMaterial = new Material();

	m_pMaterial->SetVertexShaderKey(MainEngine::GetInstance()->GetRenderer()->CreateVertexShader("vertexDirectionalLightingVS.cso", "directional"));
	m_pMaterial->SetPixelShaderKey(MainEngine::GetInstance()->GetRenderer()->CreatePixelShader("vertexDirectionalLightingPS.cso", "directional"));

	m_pMaterial->SetTexture(MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/ThisIsPicture.png"));

	m_pMaterial->DrawMaterial();
}

void MeshRenderer::Update() {

}

void MeshRenderer::Draw() {
	//ƒJƒƒ‰‚ÌŽdŽ–
	//auto camera = owner->GetComponent<Camera>();

	//auto transform = owner->GetComponent<Transform>();

	//XMMATRIX translation, rotation, scale;

	//{
	//	auto position = transform->GetPosition();
	//	auto rotate = transform->GetRotation();
	//	auto size = transform->GetScale();

	//	translation = XMMatrixTranslation(position.x, position.y, position.z);
	//	rotation = XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);
	//	scale = XMMatrixScaling(size.x, size.y, size.z);
	//}

	//XMMATRIX world = scale * rotation * translation;

	//xmmatrix

	//ConstantBuffer* constantBuffer;
	//XMStoreFloat4x4(&constantBuffer->world, XMMatrixTranspose(world));

	m_pMaterial->DrawMaterial();
}

void MeshRenderer::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::ObjectTag);
}