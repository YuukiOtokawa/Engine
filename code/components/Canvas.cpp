// ========================================================
//
// Canvasコンポーネント[Canvas.cpp]
// UIの描画を管理する
//
//									Date:20251127
//									Author:Yuuki Otokawa
// ========================================================

#include "Canvas.h"
#include "GUI.h"
#include "yaml.h"
#include "MainEngine.h"
#include "RenderCore.h"

REGISTER_COMPONENT(Canvas)

void Canvas::Update() {
	// スクリーンサイズを更新
	auto renderCore = MainEngine::GetInstance()->GetRenderCore();
	if (renderCore) {
		m_ScreenSize = renderCore->GetClientSize();
	}
}

void Canvas::Draw() {
	// Canvasの描画設定を適用
	SetupCanvasProjection();
}

void Canvas::DrawGUI() {
	ImGui::Indent();

	// レンダーモード選択
	const char* renderModes[] = { "Screen Space Overlay", "Screen Space Camera", "World Space" };
	int currentMode = static_cast<int>(m_RenderMode);
	if (ImGui::Combo("Render Mode", &currentMode, renderModes, 3)) {
		m_RenderMode = static_cast<RenderMode>(currentMode);
	}

	// ソート順序
	ImGui::InputInt("Sort Order", &m_SortOrder);

	// スクリーンサイズ表示（読み取り専用）
	ImGui::Text("Screen Size: %.0f x %.0f", m_ScreenSize.x, m_ScreenSize.y);

	ImGui::Unindent();
}

void Canvas::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::UITag);
	owner->SetLayer(GameObjectTagLayer::UILayer);
}

void Canvas::ExportComponent(YAML::Emitter& out) {
	out << YAML::Key << "renderMode" << YAML::Value << static_cast<int>(m_RenderMode);
	out << YAML::Key << "sortOrder" << YAML::Value << m_SortOrder;
}

void Canvas::ImportFile(YAML::Node& node) {
	if (node["tag"]) {
		tag = static_cast<Tag>(node["tag"].as<int>());
	}

	if (node["renderMode"]) {
		m_RenderMode = static_cast<RenderMode>(node["renderMode"].as<int>());
	}

	if (node["sortOrder"]) {
		m_SortOrder = node["sortOrder"].as<int>();
	}
}

void Canvas::SetupCanvasProjection() {
	auto renderCore = MainEngine::GetInstance()->GetRenderCore();
	if (!renderCore) return;

	switch (m_RenderMode) {
	case RenderMode::ScreenSpaceOverlay:
		// 2D用の射影行列を設定
		renderCore->SetWorldViewProjection2D();
		renderCore->SetRasterizerState2D();
		break;

	case RenderMode::ScreenSpaceCamera:
		// TODO: カメラベースの射影実装
		renderCore->SetWorldViewProjection2D();
		renderCore->SetRasterizerState2D();
		break;

	case RenderMode::WorldSpace:
		// TODO: ワールド空間での描画実装
		// 3D空間で描画する場合は通常のワールド行列を使用
		break;
	}
}
