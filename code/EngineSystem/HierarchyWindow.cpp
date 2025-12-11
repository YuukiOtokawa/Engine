#include "HierarchyWindow.h"

#include "imgui.h"
#include "Editor.h"

void HierarchyWindow::Render()
{
	ImGui::Begin("Hierarchy");

	

	//全オブジェクトの名前ボタンを描画
	for (auto& object : Editor::GetInstance()->GetObjects()) {
		if (!object) continue; // nullptrチェック

		if (object->HasChild()) {
			if (object->IsOpened()) {
				object->SetOpened(ImGui::Button("-")); // 開いている場合は▼を表示
				// 子オブジェクトの描画
				ImGui::Indent(); // 子オブジェクトのインデント
				for (auto& child : object->GetChildren()) {
					if (child) { // nullptrチェック
						if (child == Editor::GetInstance()->GetSelectedObject())
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.8f, 1.0f)); // 選択中のオブジェクトの色を変更
						else
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // 通常のオブジェクトの色
						if (ImGui::Button(child->GetName().c_str())) {
							Editor::GetInstance()->SetSelectedObject(child); // 選択されたオブジェクトを更新
						}
						object->RightClickMenu();
						ImGui::PopStyleColor();
					}
				}
			}
			else {
				object->SetOpened(ImGui::Button("+")); // 閉じている場合は►を表示
			}
		}

		if (object == Editor::GetInstance()->GetSelectedObject())
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.8f, 1.0f)); // 選択中のオブジェクトの色を変更
		else
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // 通常のオブジェクトの色

		if (object->GetName().empty()) {
			if (ImGui::Button("   ")) {
				Editor::GetInstance()->SetSelectedObject(object);
			}
		}
		else if (ImGui::Button(object->GetName().c_str())) {
			Editor::GetInstance()->SetSelectedObject(object);// 選択されたオブジェクトを更新
		}
		object->RightClickMenu();

		ImGui::PopStyleColor();
	}


	// --- ヒエラルキーの空きスペースの右クリック処理 ---
	// この関数が右クリックを検知してくれます
	if (ImGui::BeginPopupContextWindow("HierarchyWindowContextMenu", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
	{
		if (ImGui::MenuItem("Create Empty Object"))
		{
			new Object();
		}
		ImGui::EndPopup();
	}


	ImGui::End();
}
