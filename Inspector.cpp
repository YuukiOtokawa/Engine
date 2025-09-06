#include "Inspector.h"

#include "imgui.h"

#include "Object.h"

#include "../ComponentFactory.h"

void AddComponentPopup(Object* object)
{
	float windowWidth = ImGui::GetWindowSize().x;
	float buttonWidth = 150.0f;

	ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

	if (ImGui::Button("Add Component", ImVec2(buttonWidth, 25.0f))) {
		ImGui::OpenPopup("AddComponentPopup");
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		
		auto names = ComponentFactory::GetInstance()->GetRegisteredComponentNames();

		for (const auto& name : names) {
			if (ImGui::Selectable(name.c_str())) {
				Component* newComponent = ComponentFactory::GetInstance()->CreateComponent(name);
				if (newComponent) {
					object->AddComponentClass(newComponent);
				}
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}
}

#include "CSVImporter.h"
#include "MainEngine.h"

#include "Component_MeshFilter.h"
void GetMesh(MeshFilter* meshFilter)
{
	int fileID = CSVImporter::ImportMesh(OpenImportFileDialog());
	if (fileID == -1) return;
	meshFilter->SetMesh(fileID);
}
