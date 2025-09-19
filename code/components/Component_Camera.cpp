// ========================================================
//
// 基本カメラコンポーネント[Component_Camera.cpp]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component_Camera.h"


#include "FrameWork.h"
#include "MainEngine.h"

#include "Component_Transform.h"

//==========================================================================
// メンバ関数定義
//==========================================================================

REGISTER_COMPONENT(Camera)

Camera::Camera()
{
	m_Projection = XMMatrixIdentity();
	m_View = XMMatrixIdentity();
	m_ClassID = CID_Component_Camera;
}

void Camera::UpdateComponent() {
	
}

void Camera::Draw() {
	// 描画領域のサイズを取得
	Vector2O clientSize = MainEngine::GetInstance()->GetRenderCore()->GetClientSize();

	//プロジェクション行列を作成
	m_Projection =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(m_Fov),
			clientSize.x / clientSize.y,
			m_Near,
			m_Far
		);

	//カメラ行列を作成
	if (owner->GetComponent<Transform>()->GetPosition() == m_Target)
		m_Target.z += 1.0f;

	XMVECTOR eyev = XMLoadFloat3(m_Target.ToFloat3());										// カメラの視線ベクトル
	XMVECTOR pos = XMLoadFloat3(owner->GetComponent<Transform>()->GetPosition().ToFloat3());	// カメラの位置ベクトル
	XMVECTOR up = XMLoadFloat3(m_Up.ToFloat3());												// カメラの上方向ベクトル
	m_View = XMMatrixLookAtLH(pos, eyev, up);

	//カメラ行列をセット
	MainEngine::GetInstance()->GetRenderCore()->SetViewMatrix(m_View);
	MainEngine::GetInstance()->GetRenderCore()->SetProjectionMatrix(m_Projection);
	//カメラ座標セット
	MainEngine::GetInstance()->GetRenderCore()->SetCamera(owner->GetComponent<Transform>()->GetPosition());

}

void Camera::DrawGUI() {
	ImGui::Indent();
	ImGui::BeginDisabled();
	ImGui::Checkbox("Active", &m_IsActiveCamera);
	ImGui::EndDisabled();
	ImGui::SameLine();
	if (ImGui::Button("SetActive"))
		Editor::GetInstance()->SetActiveCamera(owner);
	ImGui::InputFloat3("Target", &m_Target.x, "%.2f");
	ImGui::InputFloat("Fov", &m_Fov);
	ImGui::Unindent();
}

void Camera::InitializeTag()
{
	owner->SetTag(GameObjectTagLayer::CameraTag);
	owner->SetLayer(GameObjectTagLayer::CameraLayer);
	owner->SetDrawable(false);
}

void Camera::ImportFile(std::vector<std::string>& tokens) {
	if (std::stoi(tokens[4]) == 1)
		Editor::GetInstance()->SetActiveCamera(owner);
	SetTarget(Vector4O(std::stof(tokens[5]), std::stof(tokens[6]), std::stof(tokens[7]), std::stof(tokens[8])));
	SetFov(std::stof(tokens[9]));
	m_Near = std::stof(tokens[10]);
	m_Far = std::stof(tokens[11]);
	m_Up = Vector4O(std::stof(tokens[12]), std::stof(tokens[13]), std::stof(tokens[14]), std::stof(tokens[15]));
}

