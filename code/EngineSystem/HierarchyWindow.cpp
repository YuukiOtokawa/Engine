#include "HierarchyWindow.h"

#include "imgui.h"
#include "Editor.h"

#include "Prefab.h"

#include "Object.h"

void HierarchyWindow::DragObject(std::list<Object*>* objectList, Object* target)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		// ペイロードとしてObjectポインタを設定
		ImGui::SetDragDropPayload("OBJECT_ITL", &target, sizeof(Object*));
		// ドラッグ中の表示
		ImGui::Text(target->GetName().c_str());
		ImGui::EndDragDropSource();
	}
}

void HierarchyWindow::DropSort(std::list<Object*>* objectList, Object* target, bool isRoot)
{

	ImGui::PushID(target->GetFileID());

	ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(1, 1, 0, 1));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(1, 1, 0, 1));

	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));

	ImGui::Button("##Dummy", ImVec2(-1.0f, 10.0f));

	ImGui::PopID();

	ImGui::PopStyleColor(4);

	auto pObj = GetDropIterator("OBJECT_ITL");

	if (pObj != nullptr) {
		auto targetItr = objectList->begin();
		for (; targetItr != objectList->end(); ++targetItr) {
			if (*targetItr == target) {
				break;
			}
		}
		
		std::list<Object*> l = {};
		if (pObj->IsChild()) {
			auto lastList = pObj->GetParent()->GetChildren();
			l.splice(l.begin(), *lastList, std::find(lastList->begin(), lastList->end(), pObj), std::next(std::find(lastList->begin(), lastList->end(), pObj)));
			pObj->SetParent(nullptr);
		}
		else {
			l.splice(l.begin(), *objectList, std::find(objectList->begin(), objectList->end(), pObj), std::next(std::find(objectList->begin(), objectList->end(), pObj)));
		}

		if (!isRoot) 
			target->GetParent()->AddChild(pObj);


		objectList->splice(std::next(targetItr), l, l.begin());
	}
}

void HierarchyWindow::DropChild(std::list<Object*>* objectList, Object* parent)
{
	auto it = GetDropIterator("OBJECT_ITL");

	if (it != nullptr) {
		parent->AddChild(it);
		objectList->remove(it);

	}
}

void HierarchyWindow::Render()
{
	if (!StartWindow("Hierarchy")) return;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	auto pList = Editor::GetInstance()->GetObjects();
	std::list<Object*> objectList = *pList;

	//全オブジェクトの名前ボタンを描画
	for (auto& object : objectList) {
		if (!object) continue; // nullptrチェック

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
		DropChild(pList, object);
		DragObject(pList, object);
		DropSort(pList, object, true);
		object->RightClickMenu();

		ImGui::PopStyleColor();


		if (object->HasChild()) {
			ImGui::Indent(); // 子オブジェクトのインデント
			auto pList = object->GetChildren();
			auto childList = *pList;
			for (auto& child : childList) {
				if (child) { // nullptrチェック
					if (child == Editor::GetInstance()->GetSelectedObject())
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.8f, 1.0f)); // 選択中のオブジェクトの色を変更
					else
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // 通常のオブジェクトの色
					if (ImGui::Button(child->GetName().c_str())) {
						Editor::GetInstance()->SetSelectedObject(child); // 選択されたオブジェクトを更新
					}
					DropChild(pList, child);
					DragObject(pList, child);

					DropSort(pList, child, false);
					object->RightClickMenu();
					ImGui::PopStyleColor();
				}
			}
			ImGui::Unindent(); // インデントを戻す
		}

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

	auto path = GetDropPath("PREFAB");
	if (path != "") {
		Prefab::Import(path);
	}

	ImGui::PopStyleVar();


	ImGui::End();
}
