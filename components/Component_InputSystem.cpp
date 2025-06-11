#include "Component_InputSystem.h"
#include "MainEngine.h"

GamePad* InputSystem::m_pGamePad;
Keyboard* InputSystem::m_pKeyboard;
Mouse* InputSystem::m_pMouse;

InputSystem::InputSystem()
{
	tag = ComponentTag::InputSystemTag;

	m_pGamePad = new GamePad();
	m_pKeyboard = Keyboard::GetInstance();
	m_pMouse = Mouse::GetInstance(MainEngine::GetInstance()->GetWindow());
}

// Update
void InputSystem::UpdateComponent()
{
	m_pGamePad->Update();
	m_pKeyboard->Update();
	m_pMouse->Update();
}

void InputSystem::DrawGUI() {
	ImGui::Text("Input System");
}
