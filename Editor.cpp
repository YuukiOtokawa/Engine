#include "Editor.h"

#include "MainEngine.h"

#include "Component_Camera.h"
#include "Component_Transform.h"

Editor* Editor::m_pInstance;

void Editor::Initialize() {
	m_pGUI = new GUI();
	
	
	auto camera = new Object();
	camera->SetName("MainCamera");
	camera->AddComponent<Component_Camera>();
	camera->AddComponent<Component_Transform>();

	AddObject(camera);


	
}

void Editor::Update() {

}

void Editor::Draw() {
	
	MainEngine::GetInstance()->GetRenderer()->BufferClear();


	m_pGUI->Start();

	m_pGUI->Draw();

	m_pGUI->End();
	MainEngine::GetInstance()->GetRenderer()->BufferPresent();


}

void Editor::Finalize() {

}

void Editor::AddObject(Object* object)
{
	m_Objects.push_back(object);
}
