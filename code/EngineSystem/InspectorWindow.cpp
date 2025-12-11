#include "InspectorWindow.h"

#include "imgui.h"
#include "Editor.h"

void InspectorWindow::Render()
{
	ImGui::Begin("Inspector");

	auto editor = Editor::GetInstance();

	// Prefab編集モード中の表示
	if (editor->IsEditingPrefab() && editor->GetEditingPrefabObject()) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
		ImGui::Text((const char*)u8"Prefab編集モード");
		ImGui::PopStyleColor();

		ImGui::Text((const char*)u8"編集中: %s", editor->GetEditingPrefabPath().c_str());
		ImGui::Separator();

		// 保存ボタン
		if (ImGui::Button((const char*)u8"保存 (Ctrl+S)")) {
			editor->SaveEditingPrefab();
		}
		ImGui::SameLine();
		// 閉じるボタン
		if (ImGui::Button((const char*)u8"閉じる")) {
			editor->CloseEditingPrefab();
		}

		ImGui::Separator();

		// Ctrl+Sで保存
		if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S)) {
			editor->SaveEditingPrefab();
		}
	}

	//選択されたオブジェクトの情報を表示
	if (editor->GetSelectedObject())
		editor->GetSelectedObject()->DrawGUI();

	ImGui::End();
}
