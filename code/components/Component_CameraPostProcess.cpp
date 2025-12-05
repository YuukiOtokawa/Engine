// ========================================================
//
// カメラポストプロセスコンポーネント[Component_CameraPostProcess.cpp]
//
//									Date:20251205
//									Author:Yuuki Otokawa
// ========================================================

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component_CameraPostProcess.h"

#include "MainEngine.h"
#include "Editor.h"

//==========================================================================
// メンバ関数定義
//==========================================================================

REGISTER_COMPONENT(CameraPostProcess)

// 引数: 分散値
void CameraPostProcess::CalcGaussianWeight(float dispersion)
{
	if (dispersion <= 0.001f) {
		// 分散値が0.001以下の場合、重みを初期化
		for (int i = 0; i < 8; i++)
			m_Weight[i] = 0.0f;
		m_Weight[0] = 1.0f; // 代表点のみ1.0

		return;
	}

	float total = 0;
	for (int i = 0; i < 8; i++) {
		float x = 1.0f + 2.0f * i;
		m_Weight[i] = expf(-0.5f * (float)(x * x) / dispersion); // ガウス関数

		if (i == 0)
			total += m_Weight[i];
		else
			total += m_Weight[i] * 2.0f; // 対称性を考慮して2倍
	}

	for (int i = 0; i < 8; i++) {
		m_Weight[i] /= total; // 正規化
	}
}

CameraPostProcess::CameraPostProcess()
{
	CalcGaussianWeight(0.0f);

	// 垂直方向のガウシアンブラー用マテリアル
	Material* material = new Material();
	material->SetVertexShaderKey("GaussianVS_V");
	material->SetPixelShaderKey("GaussianPS_V");
	m_pPostProcessMaterial[0] = material;

	// 水平方向のガウシアンブラー用マテリアル
	material = new Material();
	material->SetVertexShaderKey("GaussianVS_H");
	material->SetPixelShaderKey("GaussianPS_H");
	m_pPostProcessMaterial[1] = material;

	// 3つ目のマテリアル（予備）
	m_pPostProcessMaterial[2] = nullptr;
}

CameraPostProcess::~CameraPostProcess()
{
	for (int i = 0; i < 3; i++) {
		if (m_pPostProcessMaterial[i]) {
			delete m_pPostProcessMaterial[i];
			m_pPostProcessMaterial[i] = nullptr;
		}
	}
}

void CameraPostProcess::Start()
{
	// 初期化処理
}

void CameraPostProcess::Update()
{
	// ポストプロセスパラメータの更新
	// 必要に応じてキーボード入力などでパラメータを調整可能
	CalcGaussianWeight(m_Parameter.z);
}

void CameraPostProcess::DrawGUI()
{
	ImGui::Indent();
	ImGui::Checkbox("Enable Post Process", &m_IsEnabled);
	ImGui::DragFloat4("Parameter", &m_Parameter.x, 0.1f);

	// ガウシアンブラーの分散値を調整
	if (ImGui::DragFloat("Blur Dispersion", &m_Parameter.z, 0.1f, 0.0f, 100.0f)) {
		CalcGaussianWeight(m_Parameter.z);
	}

	// 重みの表示（デバッグ用）
	if (ImGui::TreeNode("Gaussian Weights")) {
		for (int i = 0; i < 8; i++) {
			ImGui::Text("Weight[%d]: %.4f", i, m_Weight[i]);
		}
		ImGui::TreePop();
	}

	ImGui::Unindent();
}

void CameraPostProcess::ImportFile(YAML::Node& node)
{
	if (node["tag"]) {
		tag = static_cast<Tag>(node["tag"].as<int>());
	}
	if (node["isEnabled"]) {
		m_IsEnabled = node["isEnabled"].as<bool>();
	}
	if (node["parameter"]) {
		auto param = node["parameter"];
		m_Parameter = Vector4O(
			param[0].as<float>(),
			param[1].as<float>(),
			param[2].as<float>(),
			param[3].as<float>()
		);
		CalcGaussianWeight(m_Parameter.z);
	}
}
