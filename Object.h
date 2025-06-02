#pragma once

#include <list>

#include "FrameWork.h"

using namespace GameObjectTagLayer;

class Component; // Forward declaration of Component class
class Object
{
private:
	std::list<Component*> m_Components;
	std::string m_Name;
	GameObjectTag m_Tag;

	bool m_IsActive = true; // Flag to indicate if the object is active
	bool m_IsDrawable = true; // Flag to indicate if the object is drawable

	int m_iVertexCount = 0;
	int m_iIndexCount = 0;

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

	virtual void Initialize();
	void Update();
	void Draw();
	void DrawGUI();
	void Finalize();

	template<typename T, typename... Args>
	void AddComponent(Args&&... args) {
		static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
		T* component = new T(std::forward<Args>(args)...);
		AddComponentClass(component);
	}

	void AddComponentClass(Component* component);

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

	std::string GetName() const {
		return m_Name;
	}

	void SetTag(GameObjectTag tag) {
		m_Tag = tag;
	}

	GameObjectTag GetTag() const {
		return m_Tag;
	}

	void SetActive(bool isActive) {
		m_IsActive = isActive;
	}

	void SetDrawable(bool isDrawable) {
		m_IsDrawable = isDrawable;
	}

	void SetVertexCount(int count) {
		m_iVertexCount = count;
	}

	void SetIndexCount(int count) {
		m_iIndexCount = count;
	}
};

