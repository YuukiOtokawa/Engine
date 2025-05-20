#pragma once

#include <list>

#include "FrameWork.h"

class Component;
class Object
{
private:
	std::list<Component*> m_Components;
	std::string m_Name;
	GameObjectTag m_Tag;
public:
	Object() = default;
	Object(const Object&) = delete; // Disable copy constructor
	Object(Object&&) = delete; // Disable move constructor
	Object& operator=(const Object&) = delete; // Disable copy assignment
	Object& operator=(Object&&) = delete; // Disable move assignment

	~Object() {
		for (Component* component : m_Components) {
			delete component; // Clean up dynamically allocated components
		}
	}

	void Initialize();
	void Update();
	void Draw();
	void Finalize();

	template<typename T, typename... Args>
	void AddComponent(Args&&... args) {
		static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
		T* component = new T(std::forward<Args>(args)...);
		AddComponent(component);
	}

	void AddComponent(Component* component);

	template<typename T>
	T* GetComponent() {
		for (Component* component : m_Components) {
			if (T* castedComponent = dynamic_cast<T*>(component)) {
				return castedComponent;
			}
		}
		return nullptr; // Return nullptr if no matching component is found
	}

	void SetName(const std::string& name) {
		m_Name = name;
	}
};

