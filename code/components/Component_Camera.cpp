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

void Camera::Update() {
	
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

void Camera::ImportFile(YAML::Node& node) {
	if (node["tag"]) {
		tag = static_cast<Tag>(node["tag"].as<int>());
	}
	if (node["isActiveCamera"]) {
		m_IsActiveCamera = node["isActiveCamera"].as<bool>();
		if (m_IsActiveCamera)
			Editor::GetInstance()->SetActiveCamera(owner);
	}
	if (node["target"]) {
		auto target = node["target"];
		SetTarget(Vector4O(target[0].as<float>(), target[1].as<float>(), target[2].as<float>(), target[3].as<float>()));
	}
	if (node["fov"]) {
		SetFov(node["fov"].as<float>());
	}
	if (node["near"]) {
		m_Near = node["near"].as<float>();
	}
	if (node["far"]) {
		m_Far = node["far"].as<float>();
	}
	if (node["up"]) {
		auto up = node["up"];
		m_Up = Vector4O(up[0].as<float>(), up[1].as<float>(), up[2].as<float>(), up[3].as<float>());
	}
}

bool Camera::IsInView(Vector3O position)
{
	XMMATRIX vp = m_View * m_Projection;

	XMMATRIX invVP = XMMatrixInverse(nullptr, vp);	//逆行列

	XMFLOAT3 vpos[4] = {
		XMFLOAT3(-1.0f,1.0f,1.0f),
		XMFLOAT3(1.0f,1.0f,1.0f),
		XMFLOAT3(-1.0f,-1.0f,1.0f),
		XMFLOAT3(1.0f,-1.0f,1.0f)
	};

	XMVECTOR vposv[4] = {
		XMLoadFloat3(&vpos[0]),
		XMLoadFloat3(&vpos[1]),
		XMLoadFloat3(&vpos[2]),
		XMLoadFloat3(&vpos[3])
	};

	XMVECTOR wposv[4] = {
		XMVector3TransformCoord(vposv[0], invVP),
		XMVector3TransformCoord(vposv[1], invVP),
		XMVector3TransformCoord(vposv[2], invVP),
		XMVector3TransformCoord(vposv[3], invVP)
	};

	XMFLOAT3 wpos[4];
	XMStoreFloat3(&wpos[0], wposv[0]);
	XMStoreFloat3(&wpos[1], wposv[1]);
	XMStoreFloat3(&wpos[2], wposv[2]);
	XMStoreFloat3(&wpos[3], wposv[3]);
	
	Vector3O camPos = owner->GetComponent<Transform>()->GetPosition().XYZ();
	Vector3O v = position - camPos;

	Vector3O wp[4] = {
		Vector3O(wpos[0]),
		Vector3O(wpos[1]),
		Vector3O(wpos[2]),
		Vector3O(wpos[3])
	};

	// 左側
	{
		Vector3O v1 = wp[0] - camPos;
		Vector3O v2 = wp[2] - camPos;

		Vector3O normal = v1.Cross(v2);

		float d = Vector3O::Dot(normal, v);

		if (d < 0.0f) return false;
	}

	// 右側
	{
		Vector3O v1 = wp[3] - camPos;
		Vector3O v2 = wp[1] - camPos;
		Vector3O normal = v1.Cross(v2);
		float d = Vector3O::Dot(normal, v);
		if (d < 0.0f) return false;
	}

	return true;
}

