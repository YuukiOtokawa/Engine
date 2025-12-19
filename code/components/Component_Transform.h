// ========================================================
//
// トランスフォームコンポーネント[Component_Transform.h]
// 
//									Date:202505220
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component.h"

#include "VectorO.h"


//==========================================================================
// クラス定義
//==========================================================================

class Transform :
    public Component
{
private:
	// 座標
	Vector4O m_Position = Vector4O(0.0f,0.0f,0.0f);
	// 回転（ピッチ、ヨー、ロール）
	Vector4O m_Rotation = Vector4O(0.0f, 0.0f, 0.0f);
    Vector4O m_Quaternion = Vector4O(0.0f, 0.0f, 0.0f, 1.0f);
	// スケール（X, Y, Z）
	Vector4O m_Scale = Vector4O(1.0f, 1.0f, 1.0f);

    XMMATRIX m_TransformMatrix = XMMatrixIdentity();

public:
    DECLARE_COMPONENT(Transform)
    Transform() {
        m_ClassID = CID_Component_Transform;
    }
	/// @brief オブジェクトの状態を更新します。
	void Update() override;
	void DrawGUI() override;

	/// @brief コンポーネントに対応したタグを所有者のオブジェクトに設定します。
	void InitializeTag() override;

    void ExportComponent(YAML::Emitter& out) override {
        out << YAML::Key << "position" << YAML::Value << YAML::Flow << YAML::BeginSeq
            << m_Position.x << m_Position.y << m_Position.z << m_Position.w << YAML::EndSeq;

        out << YAML::Key << "rotation" << YAML::Value << YAML::Flow << YAML::BeginSeq
            << m_Rotation.x << m_Rotation.y << m_Rotation.z << m_Rotation.w << YAML::EndSeq;

        out << YAML::Key << "scale" << YAML::Value << YAML::Flow << YAML::BeginSeq
            << m_Scale.x << m_Scale.y << m_Scale.z << m_Scale.w << YAML::EndSeq;
    }

    void ImportFile(YAML::Node& node) override;


	/// @brief 位置を設定します。
	/// @param position 設定する位置を表す Vector4O 型の値。
	void SetPosition(Vector4O position) { m_Position = position; }
	/// @brief 回転を設定します。
	/// @param rotation 設定する回転値。
	void SetRotation(Vector4O rotation) {
        m_Rotation = rotation;
        // Euler角からQuaternionに同期
        Vector4O radians = m_Rotation * (XM_PI / 180.0f);
        XMVECTOR quatv = XMQuaternionRotationRollPitchYaw(radians.x, radians.y, radians.z);
        XMFLOAT4 quat;
        XMStoreFloat4(&quat, quatv);
        m_Quaternion = quat;
    }
    void SetQuaternion(Vector4O quaternion) {
        m_Quaternion = quaternion;
        // QuaternionからEuler角に同期
        Vector4O radians = VOQuaternionToEuler(m_Quaternion);
        m_Rotation = radians * (180.0f / XM_PI);
    }
	/// @brief スケール値を設定します。
	/// @param scale 設定するスケール値。
	void SetScale(Vector4O scale) { m_Scale = scale; }

	/// @brief 現在の位置ベクトルを取得します。
	/// @return 現在の位置を表す Vector4O 型の値。
	Vector4O GetPosition() { return m_Position; }
	/// @brief 現在の回転を表す Vector4O オブジェクトを取得します。
	/// @return 現在の回転を表す Vector4O 型の値。
	Vector4O GetRotation() { return m_Rotation; }
    Vector4O GetQuaternion() { return m_Quaternion; }
	/// @brief スケール値を取得します。
	/// @return オブジェクトのスケールを表す Vector4O 型の値。
	Vector4O GetScale() { return m_Scale; }

    void SetTransformMatrix(XMMATRIX matrix) { m_TransformMatrix = matrix; }
    Vector3O GetForward() {
        XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        forward = XMVector3TransformNormal(forward, m_TransformMatrix);
        XMFLOAT3 f;
        XMStoreFloat3(&f, forward);
        Vector3O forwardVec(f);
        forwardVec.Normalize();
        return forwardVec;
    }

    Vector3O GetUp() {
        XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        up = XMVector3TransformNormal(up, m_TransformMatrix);
        XMFLOAT3 u;
        XMStoreFloat3(&u, up);
        Vector3O upVec(u);
        upVec.Normalize();
        return upVec;
    }

    Vector3O GetRight() {
        XMVECTOR right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        right = XMVector3TransformNormal(right, m_TransformMatrix);
        XMFLOAT3 r;
        XMStoreFloat3(&r, right);
        Vector3O rightVec(r);
        rightVec.Normalize();
        return rightVec;
    }

    float GetDistance(Vector3O position) {
        auto direction = m_Position.XYZ() - position;
        return direction.Length();
    }

    float GetZ(Vector3O position, Vector3O forward) {
        auto direction = m_Position.XYZ() - position;
        return Vector3O::Dot(direction, forward);
    }
};

