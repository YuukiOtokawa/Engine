#include "Component_Camera.h"


#include "FrameWork.h"
#include "MainEngine.h"

#include "Component_Transform.h"

Camera::Camera()
{
	
}

void Camera::UpdateComponent() {
	
}

void Camera::Draw() {
	Vector4O clientSize = MainEngine::GetInstance()->GetRenderer()->GetClientSize();
	//プロジェクション行列を作成
	XMMATRIX	ProjectionMatrix =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(m_Fov),
			clientSize.x / clientSize.y,
			m_Near,
			m_Far
		);
	//カメラ行列を作成
	XMVECTOR	eyev = XMLoadFloat3(m_Target.ToFloat3());
	XMVECTOR	pos = XMLoadFloat3(owner->GetComponent<Transform>()->GetPosition().ToFloat3());
	XMVECTOR	up = XMLoadFloat3(m_Up.ToFloat3());
	XMMATRIX	ViewMatrix =
		XMMatrixLookAtLH(pos, eyev, up);

	//カメラ行列をセット
	MainEngine::GetInstance()->GetRenderer()->SetViewMatrix(ViewMatrix);
	MainEngine::GetInstance()->GetRenderer()->SetProjectionMatrix(ProjectionMatrix);
	//カメラ座標セット
	MainEngine::GetInstance()->GetRenderer()->SetCamera(owner->GetComponent<Transform>()->GetPosition());

}

void Camera::DrawGUI() {
	ImGui::Separator();
	ImGui::Text("Camera Component");
	ImGui::Indent();
	ImGui::Checkbox("Active", &m_IsActiveCamera);
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
