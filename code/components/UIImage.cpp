// ========================================================
//
// UIImageコンポーネント[UIImage.cpp]
// UI用のスプライト描画コンポーネント（Rendererベース）
//
//									Date:20251127
//									Author:Yuuki Otokawa
// ========================================================

#include "UIImage.h"
#include "UIImageMesh.h"
#include "RectTransform.h"
#include "Canvas.h"
#include "MainEngine.h"
#include "RenderCore.h"
#include "GUI.h"
#include "yaml.h"
#include "Editor.h"
#include "Material.h"

REGISTER_COMPONENT(UIImage)

void UIImage::Update() {
	// 特に更新処理なし
}

void UIImage::Render() {
	if (!m_bIsVisible) return;

	// UIImageMeshコンポーネントを取得
	auto meshFilter = owner->GetComponent<UIImageMesh>();
	if (!meshFilter || !meshFilter->GetVertexBuffer()) {
		return;
	}

	auto renderCore = MainEngine::GetInstance()->GetRenderCore();
	if (!renderCore) return;

	// Canvasコンポーネントを取得（親階層を遡って探す）
	Canvas* canvas = nullptr;
	Object* current = owner;
	while (current) {
		canvas = current->GetComponent<Canvas>();
		if (canvas) break;
		if (current->IsChild()) {
			current = current->GetParent();
		}
		else {
			break;
		}
	}

	// Canvasの設定を適用
	if (canvas) {
		canvas->SetupCanvasProjection();
	}
	else {
		// Canvasがない場合はデフォルトで2D描画
		renderCore->SetWorldViewProjection2D();
		renderCore->SetRasterizerState2D();
	}

	// RectTransformを取得
	auto rectTransform = owner->GetComponent<RectTransform>();
	if (!rectTransform) return;

	// スクリーンサイズを取得
	Vector2O screenSize = renderCore->GetClientSize();
	if (canvas) {
		screenSize = canvas->GetScreenSize();
	}

	// スクリーン座標を計算
	Vector2O screenPos = rectTransform->CalculateScreenPosition(screenSize.x, screenSize.y);
	Vector2O size = rectTransform->GetSize();

	// Transformの設定（PositionとAnchoredPositionを合成）
	Vector4O position = rectTransform->GetPosition();
	position.x += screenPos.x;
	position.y += screenPos.y;

	Vector4O rotation = rectTransform->GetRotation();
	Vector4O scale = Vector4O(size.x, size.y, 1.0f);

	// 行列を設定
	XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
		rotation.x * (XM_PI / 180.0f),
		rotation.y * (XM_PI / 180.0f),
		rotation.z * (XM_PI / 180.0f)
	);
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);

	renderCore->SetTranslationMatrix(translationMatrix);
	renderCore->SetAngleMatrix(rotationMatrix);
	renderCore->SetScaleMatrix(scaleMatrix);

	// 頂点バッファとインデックスバッファを設定
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	renderCore->GetDeviceContext()->IASetVertexBuffers(0, 1, meshFilter->GetVertexBuffer(), &stride, &offset);
	if (meshFilter->GetIndexBuffer())
		renderCore->GetDeviceContext()->IASetIndexBuffer(meshFilter->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	// プリミティブトポロジーを設定
	renderCore->GetDeviceContext()->IASetPrimitiveTopology(meshFilter->GetPrimitiveTopology());

	// マテリアルとシェーダーを設定
	if (m_pMaterial) {
		m_pMaterial->SetShader();
		m_pMaterial->DrawMaterial();
	}

	// 描画
	if (meshFilter->GetIndexCount() == 0)
		renderCore->GetDeviceContext()->Draw(meshFilter->GetVertexCount(), 0);
	else
		renderCore->GetDeviceContext()->DrawIndexed(meshFilter->GetIndexCount(), 0, 0);
}

void UIImage::DrawGUI() {
	ImGui::Indent();

	// テクスチャ選択
	if (m_pMaterial) {
		m_pMaterial->DrawGUI();
	}

	// 色設定
	ImGui::ColorEdit4("Color", &m_Color.x);

	// UV矩形設定
	ImGui::InputFloat4("UV Rect (min_u, min_v, max_u, max_v)", &m_UVRect.x);

	// 可視性
	ImGui::Checkbox("Visible", &m_bIsVisible);

	ImGui::Unindent();
}

void UIImage::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::UITag);
	owner->SetLayer(GameObjectTagLayer::UILayer);
}

void UIImage::SetTexture(int fileID) {
	if (m_pMaterial) {
		m_pMaterial->SetTexture(fileID);
	}
}

int UIImage::GetTexture() const {
	if (m_pMaterial) {
		return m_pMaterial->GetTexture();
	}
	return -1;
}

void UIImage::AddExportList()
{
	SceneExporter::AddExportList(this);
	SceneExporter::AddExportList(m_pMaterial);
}

void UIImage::ExportComponent(YAML::Emitter& out) {
	// Rendererの基本情報をエクスポート
	Renderer::ExportComponent(out);

	out << YAML::Key << "color" << YAML::Value << YAML::Flow << YAML::BeginSeq
		<< m_Color.x << m_Color.y << m_Color.z << m_Color.w << YAML::EndSeq;

	out << YAML::Key << "uvRect" << YAML::Value << YAML::Flow << YAML::BeginSeq
		<< m_UVRect.x << m_UVRect.y << m_UVRect.z << m_UVRect.w << YAML::EndSeq;

	out << YAML::Key << "isVisible" << YAML::Value << m_bIsVisible;
	out << YAML::Key << "castShadows" << YAML::Value << m_bCastShadows;
	if (m_pMaterial) {
		out << YAML::Key << "materialFileID" << YAML::Value << m_pMaterial->GetFileID();
	}
}

void UIImage::ImportFile(YAML::Node& node) {
	if (node["tag"]) {
		tag = static_cast<Tag>(node["tag"].as<int>());
	}

	if (node["color"]) {
		auto color = node["color"];
		m_Color = Vector4O(color[0].as<float>(), color[1].as<float>(), color[2].as<float>(), color[3].as<float>());
	}

	if (node["uvRect"]) {
		auto uvRect = node["uvRect"];
		m_UVRect = Vector4O(uvRect[0].as<float>(), uvRect[1].as<float>(), uvRect[2].as<float>(), uvRect[3].as<float>());
	}

	if (node["isVisible"]) {
		m_bIsVisible = node["isVisible"].as<bool>();
	}

	if (node["castShadows"]) {
		m_bCastShadows = node["castShadows"].as<bool>();
	}

	if (node["materialFileID"]) {
		m_pMaterial = Editor::GetInstance()->GetMaterialByFileID(node["materialFileID"].as<int>());
	}
}
