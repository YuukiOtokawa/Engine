// ========================================================
//
// RectTransformコンポーネント[RectTransform.cpp]
// UIオブジェクト用のトランスフォーム
//
//									Date:20251127
//									Author:Yuuki Otokawa
// ========================================================

#include "RectTransform.h"
#include "GUI.h"
#include "yaml.h"

REGISTER_COMPONENT(RectTransform)

void RectTransform::Update() {
	// 基底クラスの更新処理
	Transform::Update();
}

void RectTransform::DrawGUI() {
	ImGui::Indent();

	// Transformの基本情報
	Vector4O pos = GetPosition();
	Vector4O rot = GetRotation();
	Vector4O scale = GetScale();

	ImGui::InputFloat3("Position", &pos.x, "%.2f");
	ImGui::InputFloat3("Rotation", &rot.x, "%.2f");
	ImGui::InputFloat3("Scale", &scale.x, "%.2f");

	SetPosition(pos);
	SetRotation(rot);
	SetScale(scale);

	ImGui::Separator();

	// RectTransform固有の情報
	ImGui::Text("RectTransform");
	ImGui::InputFloat2("Anchored Position", &m_AnchoredPosition.x, "%.2f");
	ImGui::InputFloat2("Size", &m_Size.x, "%.2f");
	ImGui::InputFloat2("Pivot", &m_Pivot.x, "%.3f");
	ImGui::InputFloat2("Anchor Min", &m_AnchorMin.x, "%.3f");
	ImGui::InputFloat2("Anchor Max", &m_AnchorMax.x, "%.3f");

	ImGui::Unindent();
}

void RectTransform::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::UITag);
	owner->SetLayer(GameObjectTagLayer::UILayer);
}

void RectTransform::ExportComponent(YAML::Emitter& out) {
	// Transformの基本情報をエクスポート
	Transform::ExportComponent(out);

	// RectTransform固有の情報をエクスポート
	out << YAML::Key << "anchoredPosition" << YAML::Value << YAML::Flow << YAML::BeginSeq
		<< m_AnchoredPosition.x << m_AnchoredPosition.y << YAML::EndSeq;

	out << YAML::Key << "size" << YAML::Value << YAML::Flow << YAML::BeginSeq
		<< m_Size.x << m_Size.y << YAML::EndSeq;

	out << YAML::Key << "pivot" << YAML::Value << YAML::Flow << YAML::BeginSeq
		<< m_Pivot.x << m_Pivot.y << YAML::EndSeq;

	out << YAML::Key << "anchorMin" << YAML::Value << YAML::Flow << YAML::BeginSeq
		<< m_AnchorMin.x << m_AnchorMin.y << YAML::EndSeq;

	out << YAML::Key << "anchorMax" << YAML::Value << YAML::Flow << YAML::BeginSeq
		<< m_AnchorMax.x << m_AnchorMax.y << YAML::EndSeq;
}

void RectTransform::ImportFile(YAML::Node& node) {
	// Transformの基本情報をインポート
	Transform::ImportFile(node);

	// RectTransform固有の情報をインポート
	if (node["anchoredPosition"]) {
		auto pos = node["anchoredPosition"];
		m_AnchoredPosition = Vector2O(pos[0].as<float>(), pos[1].as<float>());
	}

	if (node["size"]) {
		auto size = node["size"];
		m_Size = Vector2O(size[0].as<float>(), size[1].as<float>());
	}

	if (node["pivot"]) {
		auto pivot = node["pivot"];
		m_Pivot = Vector2O(pivot[0].as<float>(), pivot[1].as<float>());
	}

	if (node["anchorMin"]) {
		auto anchor = node["anchorMin"];
		m_AnchorMin = Vector2O(anchor[0].as<float>(), anchor[1].as<float>());
	}

	if (node["anchorMax"]) {
		auto anchor = node["anchorMax"];
		m_AnchorMax = Vector2O(anchor[0].as<float>(), anchor[1].as<float>());
	}
}

Vector2O RectTransform::CalculateScreenPosition(float screenWidth, float screenHeight) {
	// アンカーの中心を計算
	Vector2O anchorCenter = (m_AnchorMin + m_AnchorMax) * 0.5f;

	// スクリーン上のアンカー位置を計算（画面中心が0,0）
	Vector2O screenAnchor = Vector2O(
		(anchorCenter.x - 0.5f) * screenWidth,
		(0.5f - anchorCenter.y) * screenHeight
	);

	// ピボットを考慮したオフセット
	Vector2O pivotOffset = Vector2O(
		(m_Pivot.x - 0.5f) * m_Size.x,
		(0.5f - m_Pivot.y) * m_Size.y
	);

	// 最終的な位置 = アンカー位置 + アンカーからのオフセット - ピボットオフセット
	return screenAnchor + m_AnchoredPosition - pivotOffset;
}
