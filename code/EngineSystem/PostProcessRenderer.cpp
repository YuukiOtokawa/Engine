#include "PostProcessRenderer.h"

#include "MainEngine.h"

#include "Component_InputSystem.h"

#include "Component_MeshFilter.h"
#include "Component_Transform.h"

// 引数 分散値
void PostProcessRenderer::CalcGaussianWeight(float dispersion)
{
	if (dispersion <= 0.001f) {
		// 分散値が0.001以下の場合、重みを初期化
		for (int i = 0; i < 8; i++) 
			m_Weight[i] = 0.0f;
		m_Weight[0] = 1.0f; //代表点のみ1.0
		
		return;
	}

	float total = 0;
	for (int i = 0; i < 8; i++) {
		float x = 1.0f + 2.0f * i;
		m_Weight[i] = expf(-0.5f * (float)(x * x) / dispersion); // ガウス関数

		if ( i ==0)
			total += m_Weight[i];
		else
			total += m_Weight[i] * 2.0f; // 対称性を考慮して2倍
	}

	for (int i=0;i<8;i++) {
		m_Weight[i] *= total; // 正規化
	}
}

PostProcessRenderer::PostProcessRenderer() : MeshRenderer()
{
	CalcGaussianWeight(0.0f);

	Material* material = new Material();
	material->SetVertexShaderKey("GaussianVS_V");
	material->SetPixelShaderKey("GaussianPS_V");

	m_pPostProcessMaterial[0] = material;

	material = new Material();
	material->SetVertexShaderKey("GaussianVS_H");
	material->SetPixelShaderKey("GaussianPS_H");
	m_pPostProcessMaterial[1] = material;
}

void PostProcessRenderer::UpdateComponent()
{
	float ofs = 0.4f;

	auto keyboard = Editor::GetInstance()->GetObject("InputSystem")->GetComponent<InputSystem>()->GetKeyboard();

	if (keyboard->GetKeyDown(KK_O)) {
		m_Parameter.z -= ofs;
		if (m_Parameter.z < 0.0f)
			m_Parameter.z = 0.001f;
	}else if(keyboard->GetKeyDown(KK_P)){
		m_Parameter.z += ofs;
		if (m_Parameter.z > 1000.0f)
			m_Parameter.z = 1000.0f;
	}

	CalcGaussianWeight(m_Parameter.z);
}

void PostProcessRenderer::DrawPostProcess(int n)
{
	if (!m_bIsVisible) return;

	MainEngine::GetInstance()->GetRenderCore()->SetWeight(&m_Weight[0]);

	// 1. 所有者オブジェクトからMeshFilterコンポーネントを取得
	auto meshFilter = owner->GetComponent<MeshFilter>();
	if (!meshFilter) {
		return;
	}

	auto vertexBuffer = meshFilter->GetVertexBuffer();
	auto indexBuffer = meshFilter->GetIndexBuffer();
	if (!vertexBuffer) {
		return;
	}

	// 2. Transformコンポーネントを取得
	auto transform = owner->GetComponent<Transform>();
	if (!transform) return;
	XMMATRIX scale, angle, translation;
	{
		if (owner->IsChild()) {
			/// 子オブジェクトのtransformは親オブジェクトからの相対位置であるため、親オブジェクトのTransformを考慮
			auto parentTransform = owner->GetParent()->GetComponent<Transform>();

			auto parentScale = parentTransform->GetScale();
			auto parentRotation = parentTransform->GetRotation().ToRadian();
			auto parentPosition = parentTransform->GetPosition();

			auto objectScale = transform->GetScale();
			auto objectRotation = transform->GetRotation().ToRadian();
			auto objectPosition = transform->GetPosition();

			scale = XMMatrixScaling(objectScale.x * parentScale.x, objectScale.y * parentScale.y, objectScale.z * parentScale.z);
			angle = XMMatrixRotationRollPitchYaw(objectRotation.x + parentRotation.x, objectRotation.y + parentRotation.y, objectRotation.z + parentRotation.z);
			translation = XMMatrixTranslation(
				objectPosition.x + parentPosition.x,
				objectPosition.y + parentPosition.y,
				objectPosition.z + parentPosition.z
			);
		}
		else {
			auto objectScale = transform->GetScale();
			auto objectRotation = transform->GetRotation().ToRadian();
			auto objectPosition = transform->GetPosition();

			scale = XMMatrixScaling(objectScale.x, objectScale.y, objectScale.z);
			angle = XMMatrixRotationRollPitchYaw(objectRotation.x, objectRotation.y, objectRotation.z);
			translation = XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z);
		}

	}

	// 行列をレンダラーに設定する
	MainEngine::GetInstance()->GetRenderCore()->SetTranslationMatrix(translation);
	MainEngine::GetInstance()->GetRenderCore()->SetScaleMatrix(scale);
	MainEngine::GetInstance()->GetRenderCore()->SetAngleMatrix(angle);

	// 3. 頂点・インデックスバッファを設定する (MeshFilter::Draw()から移動)
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer, &stride, &offset);
	if (indexBuffer)
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 4. プリミティブトポロジーを設定する (MeshFilter::Draw()から移動)
	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetPrimitiveTopology(meshFilter->GetPrimitiveTopology());

	// 5. シェーダーとマテリアルのプロパティを設定する (すでにあなたのコードに記述されています)
	auto ownerScale = transform->GetScale();
	m_pPostProcessMaterial[n]->GetMaterial()->Aspect = Vector2O(ownerScale.x, ownerScale.y);
	m_pPostProcessMaterial[n]->SetShader();
	m_pPostProcessMaterial[n]->DrawMaterial();

	// 6. 最終的な描画呼び出しを実行する
	if (meshFilter->GetVertexCount() == 0) return;
	if (meshFilter->GetIndexCount() == 0)
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->Draw(meshFilter->GetVertexCount(), 0);
	else
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->DrawIndexed(meshFilter->GetIndexCount(), 0, 0);
}

void PostProcessRenderer::DrawGUI()
{
	//auto renderer = MainEngine::GetInstance()->GetRenderCore();
	//if (m_pPostProcessMaterial[0])
	//	ImGui::Image((ImTextureID)renderer->GetTexture(m_pPostProcessMaterial[0]->GetTexture()), ImVec2(300, 300));
	//if (m_pPostProcessMaterial[1])
	//	ImGui::Image((ImTextureID)renderer->GetTexture(m_pPostProcessMaterial[1]->GetTexture()), ImVec2(300, 300));
	//if (m_pPostProcessMaterial[2])
	//	ImGui::Image((ImTextureID)renderer->GetTexture(m_pPostProcessMaterial[2]->GetTexture()), ImVec2(300, 300));
}
