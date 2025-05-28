#pragma once

#include <list>
#include <map>

#include "Object.h"

#include "EditorWindow.h"

#include "GUI.h"

class Editor
{
private:

	std::list<Object*> m_Objects;

	std::list<EditorWindow*> m_EditorWindows;

	GUI* m_pGUI = nullptr;

	Editor() = default;
	Editor(const Editor&) = delete;
	Editor& operator=(const Editor&) = delete;

	static Editor* m_pInstance;

public:
	static Editor* GetInstance()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new Editor();
		}
		return m_pInstance;
	}
	
	void Initialize();
	void Update();
	void Draw();
	void Finalize();

	void AddObject(Object* object);
	Object* GetObject(const std::string& name)
	{
		for (auto object : m_Objects)
		{
			if (object->GetName() == name)
			{
				return object;
			}
		}
		return nullptr;
	}
};

void Main();