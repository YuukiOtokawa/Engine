#include "Inspector.h"

#include "imgui.h"

#include "Object.h"

#include "Component_Transform.h"
#include "Component_Camera.h"
#include "Component_MeshFilter.h"
#include "Component_MeshRenderer.h"
#include "Component_Light.h"



void OpenPopup()
{
	float windowWidth = 160.0f;
	float buttonWidth = 150.0f;

	if (ImGui::Button("Add Component", ImVec2(buttonWidth, 25.0f))) {
		ImGui::OpenPopup("AddComponentPopup");
	}
}

void BeginPopup(Object* object)
{
	if (ImGui::BeginPopup("AddComponentPopup")) {
		if (ImGui::MenuItem("Transform")) {
			object->AddComponent<Transform>();
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Camera")) {
			object->AddComponent<Camera>();
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("MeshFilter")) {
			object->AddComponent<MeshFilter>();
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("MeshRenderer")) {
			object->AddComponent<MeshRenderer>();
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Light")) {
			object->AddComponent<Light>();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

#include "CSVImporter.h"
#include "MainEngine.h"

void GetMesh(MeshFilter* meshFilter)
{
	int fileID = CSVImporter::ImportMesh(OpenImportFileDialog());
	if (fileID == -1) return;
	meshFilter->SetMesh(fileID);
}
