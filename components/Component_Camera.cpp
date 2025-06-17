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

Camera::Camera()
{
	
}

void Camera::UpdateComponent() {
	
}

void Camera::Draw() {
	// 描画領域のサイズを取得
	Vector4O clientSize = MainEngine::GetInstance()->GetRenderer()->GetClientSize();

	//プロジェクション行列を作成
	m_Projection =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(m_Fov),
			clientSize.x / clientSize.y,
			m_Near,
			m_Far
		);

	//カメラ行列を作成
	XMVECTOR eyev = XMLoadFloat3(m_Target.ToFloat3());										// カメラの視線ベクトル
	XMVECTOR pos = XMLoadFloat3(owner->GetComponent<Transform>()->GetPosition().ToFloat3());	// カメラの位置ベクトル
	XMVECTOR up = XMLoadFloat3(m_Up.ToFloat3());												// カメラの上方向ベクトル
	m_View = XMMatrixLookAtLH(pos, eyev, up);

	//カメラ行列をセット
	MainEngine::GetInstance()->GetRenderer()->SetViewMatrix(m_View);
	MainEngine::GetInstance()->GetRenderer()->SetProjectionMatrix(m_Projection);
	//カメラ座標セット
	MainEngine::GetInstance()->GetRenderer()->SetCamera(owner->GetComponent<Transform>()->GetPosition());

}

void Camera::DrawGUI() {
	ImGui::Separator();
	ImGui::Text("Camera Component");
	ImGui::Indent();
	ImGui::BeginDisabled();
	ImGui::Checkbox("Active", &m_IsActiveCamera);
	ImGui::EndDisabled();
	ImGui::SameLine();
	if (ImGui::Button("SetActive"))
		Editor::GetInstance()->SetActiveCamera(owner);
	ImGui::InputFloat3("Target", &m_Target.x, "%.2f");
	ImGui::InputFloat3("Angle", &m_Angle.x, "%.2f");
	ImGui::InputFloat("Fov", &m_Fov);
	ImGui::Unindent();
}

void Camera::InitializeTag()
{
	owner->SetTag(GameObjectTagLayer::CameraTag);
	owner->SetDrawable(false);
}
