#include "EditorViewWindow.h"

#include "imgui.h"
#include "ImGuizmo.h"
#include "MainEngine.h"

#include "Component_Camera.h"
#include "Component_Transform.h"

#include "GUI.h"

void EditorViewWindow::Render()
{
	StartWindow("SceneView", ImGuiWindowFlags_NoScrollbar);


	auto renderCore = MainEngine::GetInstance()->GetRenderCore();

	auto tex = renderCore->GetSceneViewTexture();

	// テクスチャサイズ
	ImVec2 texSize = ImVec2((float)tex->width, (float)tex->height);
	// ウィンドウサイズ
	ImVec2 avail = ImGui::GetWindowSize();

	// 拡縮倍率を計算
	float scaleX = texSize.x / avail.x;
	float scaleY = texSize.y / avail.y;
	float scale = 1.0f;

	if (scaleX < 1.0f) { // 横幅がウィンドウ内に収まっている場合
		if (scaleY >= 1.0f) { // 縦幅がウィンドウからはみ出している場合
			scale = 1.0f / scaleX;
		}
		else if (scaleY < 1.0f) { // 縦幅もウィンドウ内に収まっている場合
			scale = (std::max)(1.0f / scaleX, 1.0f / scaleY);
		}

	}
	else if (scaleY < 1.0f) { // 縦幅がウィンドウ内に収まっている場合
		if (scaleX >= 1.0f) { // 横幅がウィンドウからはみ出している場合
			scale = 1.0f / scaleY;
		}
	}
	else { // 横幅・縦幅共にウィンドウからはみ出している場合
		scale = 1.0f / (std::min)(scaleX, scaleY);
	}

	// 実際のテクスチャの描画サイズを計算
	ImVec2 drawSize = ImVec2(texSize.x * scale, texSize.y * scale);

	// テクスチャを中央に配置するためのオフセット計算
	ImVec2 offset = ImVec2(
		(avail.x - drawSize.x) * 0.5f,
		(avail.y - drawSize.y) * 0.5f
	);

	// カーソル位置を中央に移動
	ImGui::SetCursorPos(offset);

	ImGui::Image((ImTextureID)tex->shaderResourceView, drawSize, ImVec2(0, 0), ImVec2(1, 1));
	m_IsHovered = ImGui::IsItemHovered();

	CalcMatrix();
	DrawGizmo();

	ImGui::End();
}

#include "EngineConsole.h"
void EditorViewWindow::CalcMatrix()
{
	auto camera = Editor::GetInstance()->GetEditorCamera()->GetComponent<Camera>();
	if (!camera) return;

	// ImGuizmoの設定
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	// ウィンドウの位置とサイズを取得
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

	// カメラのビュー行列とプロジェクション行列を取得
	XMMATRIX viewMatrix = camera->GetView();
	XMMATRIX projectionMatrix = camera->GetProjection();

	// グリッド表示（XYZ軸の線を表示）
	//XMMATRIX identityMatrix = XMMatrixIdentity();
	//ImGuizmo::DrawGrid(
	//	(float*)&viewMatrix,
	//	(float*)&projectionMatrix,
	//	(float*)&identityMatrix,
	//	100.0f  // グリッドサイズ
	//);

	// ViewManipulate（右上にXYZ軸のビューキューブを表示）
	float viewManipulateSize = 128.0f;
	ImVec2 viewManipulatePos = ImVec2(
		windowPos.x + windowSize.x - viewManipulateSize - 10.0f,
		windowPos.y + 10.0f
	);

	// ViewManipulateのために行列を反転（カメラの向きを正しく表示するため）
	XMMATRIX viewMatrixInverse = XMMatrixInverse(nullptr, viewMatrix);
	ImGuizmo::ViewManipulate(
		(float*)&viewMatrixInverse,
		8.0f,  // カメラからの距離
		viewManipulatePos,
		ImVec2(viewManipulateSize, viewManipulateSize),
		0x10101010  // 背景色
	);

	// 右クリックドラッグでカメラ回転
	if (m_IsHovered && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
		static ImVec2 lastMousePos = ImGui::GetIO().MousePos;
		if (m_IsHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			lastMousePos = ImGui::GetIO().MousePos;
		}
		ImVec2 currentMousePos = ImGui::GetIO().MousePos;
		ImVec2 delta = ImVec2(
			currentMousePos.x - lastMousePos.x,
			currentMousePos.y - lastMousePos.y
		);
		float rotationSpeed = 0.01f; // 回転速度の調整
		auto transform = camera->owner->GetComponent<Transform>();
		transform->SetRotation(transform->GetRotation() + Vector4O(-delta.y * rotationSpeed, delta.x * rotationSpeed, 0.0f));
		// カメラのTargetを視線方向に合わせる
		auto rot = transform->GetRotation();
		auto pos = transform->GetPosition();
		m_CameraDirection = Vector4O(
            cosf(rot.x) * sinf(rot.y),
            sinf(rot.x),
            cosf(rot.x) * cosf(rot.y)
        ).Normalize();
        camera->SetTarget(pos + m_CameraDirection);
		//camera->SetTarget(camera->GetTarget() + Vector4O(delta.x * rotationSpeed, -delta.y * rotationSpeed));
		lastMousePos = currentMousePos;

	}
	else {
		// 右クリックが離されたらマウス位置をリセット
		static ImVec2 lastMousePos = ImGui::GetIO().MousePos;
		lastMousePos = ImGui::GetIO().MousePos;
	}
}

void EditorViewWindow::DrawGizmo()
{
	auto selected = Editor::GetInstance()->GetSelectedObject();

	// 選択中のオブジェクトにギズモを表示
	if (selected) {
		auto transform = selected->GetComponent<Transform>();

		if (transform) {
			// キーボード入力で操作モードを切り替え (Scene Viewがホバーされている場合のみ)
			if (m_IsHovered && !ImGuizmo::IsUsing() && !ImGui::IsKeyDown(ImGuiKey_MouseRight)) {
				if (ImGui::IsKeyPressed(ImGuiKey_W)) {
					m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE; // 移動モード
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_E)) {
					m_GizmoOperation = ImGuizmo::OPERATION::ROTATE; // 回転モード
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_R)) {
					m_GizmoOperation = ImGuizmo::OPERATION::SCALE; // スケールモード
				}
			}

			// Transformからワールド行列を作成
			XMMATRIX translationMatrix = XMMatrixTranslation(
				transform->GetPosition().x,
				transform->GetPosition().y,
				transform->GetPosition().z
			);
			XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(transform->GetRotation().x),
				XMConvertToRadians(transform->GetRotation().y),
				XMConvertToRadians(transform->GetRotation().z)
			);
			XMMATRIX scaleMatrix = XMMatrixScaling(
				transform->GetScale().x,
				transform->GetScale().y,
				transform->GetScale().z
			);
			XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

			// 操作モードの選択
			ImGuizmo::OPERATION operation;
			switch (m_GizmoOperation) {
			case 0:
				operation = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case 1:
				operation = ImGuizmo::OPERATION::ROTATE;
				break;
			case 2:
				operation = ImGuizmo::OPERATION::SCALE;
				break;
			default:
				operation = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}

			// ImGuizmoでギズモを操作
			// 回転時はLOCALモードを使用してジンバルロックを軽減
			ImGuizmo::MODE mode = (operation == ImGuizmo::OPERATION::ROTATE)
				? ImGuizmo::MODE::LOCAL
				: ImGuizmo::MODE::WORLD;

			ImGuizmo::Manipulate(
				(float*)&m_ViewMatrix,
				(float*)&m_ProjectionMatrix,
				operation,
				mode,
				(float*)&worldMatrix,
				nullptr,
				nullptr
			);

			// ギズモで変更された行列から位置、回転、スケールを抽出
			if (ImGuizmo::IsUsing()) {
				float position[3], rotation[3], scale[3];
				ImGuizmo::DecomposeMatrixToComponents(
					(float*)&worldMatrix,
					position,
					rotation,
					scale
				);

				// Transformに反映
				transform->SetPosition(Vector4O(position[0], position[1], position[2]));
				transform->SetRotation(Vector4O(rotation[0], rotation[1], rotation[2]));
				transform->SetScale(Vector4O(scale[0], scale[1], scale[2]));
			}
		}
	}
}

