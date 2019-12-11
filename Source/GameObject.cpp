#include "GameObject.h"
#include "Globals.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "imgui.h"

GameObject::GameObject() {
}

GameObject::GameObject(const std::string &name) : name(name) {
}

GameObject::~GameObject() {
	for (int i = 0; i < components.size(); ++i)
		delete components[i];
	components.clear();
}

bool GameObject::Init() {
	return true;
}

update_status GameObject::PreUpdate() {
	return UPDATE_CONTINUE;
}

update_status GameObject::Update() {
	return UPDATE_CONTINUE;
}

update_status GameObject::PostUpdate() {
	return UPDATE_CONTINUE;
}

bool GameObject::CleanUp() {
	return true;
}

Component* GameObject::CreateComponent(Type type) {
	Component* component = nullptr;

	switch (type) {
	case Type::TRANSFORM:
		component = new ComponentTransform();
		break;

	case Type::MESH:
		component = new ComponentMesh();
		break;

	case Type::MATERIAL:
		component = new ComponentMaterial();
		break;

	default:
		LOG("Incorrect component type!\n");
		break;
	}

	if (component != nullptr)
		components.push_back(component);

	return component;
}

void GameObject::DrawComponents() {
	ImGui::Checkbox("Active", &isActive);
	ImGui::SameLine();
	ImGui::InputText("", &name[0], name.size());
	ImGui::SameLine();
	ImGui::Checkbox("Static", &isStatic);
	for (int i = 0; i < components.size(); ++i) {
		components[i]->OnEditor();
	}
	ImGui::Button("Add Component");
}

GameObject* GameObject::GetChild(unsigned childIndex) const {
	GameObject* child = nullptr;

	childIndex > children.size() - 1 ? LOG("Child index is out of range!\n") : child = children[childIndex];

	return child;
}

GameObject* GameObject::GetChild(const std::string &name) const {
	GameObject* child = nullptr;

	if (name.empty()) {
		LOG("Child name is empty!\n");
	}
	else {
		for (int i = 0; i < children.size(); ++i) {
			if (name == children[i]->name) {
				child = children[i];
				break;
			}
		}
	}

	return child;
}

Component* GameObject::GetComponent(Type type) const {
	Component* component = nullptr;

	for (int i = 0; i < components.size(); ++i) {
		if (type == components[i]->type) {
			component = components[i];
			break;
		}
	}

	if (component == nullptr)
		LOG("Component %d doesn't exist on gameobject!\n", type);

	return component;
}

Component* GameObject::GetComponentInParent(Type type) const {
	Component* component = nullptr;

	if (parent == nullptr) {
		LOG("Gameobject doesn't have parent!\n", type);
	}
	else {
		for (int i = 0; i < parent->components.size(); ++i) {
			if (type == parent->components[i]->type) {
				component = parent->components[i];
			}
		}

		if (component == nullptr)
			LOG("Component %d doesn't exist on parent gameobject!\n", type);
	}

	return component;
}

Component* GameObject::GetComponentInChild(unsigned childIndex, Type type) const {
	Component* component = nullptr;

	if (children.size() < 1) {
		LOG("Gameobject doesn't have children!\n", type);
	}
	else if (childIndex > children.size() - 1) {
		LOG("Child index is out of range!\n");
	}
	else {
		component = children[childIndex]->GetComponent(type);

		if (component == nullptr)
			LOG("Component %d doesn't exist on child gameobject!\n", type);
	}

	return component;
}

Component* GameObject::GetComponentInChild(const std::string &childName, Type type) const {
	Component* component = nullptr;

	if (children.size() < 1) {
		LOG("Gameobject doesn't have children!\n", type);
	}
	else if (childName.empty()) {
		LOG("Child name is empty!\n");
	}
	else {
		for (int i = 0; i < children.size(); ++i) {
			if (childName == children[i]->name) {
				children[i]->GetComponent(type);
				break;
			}
		}

		if (component == nullptr)
			LOG("Component %d doesn't exist on child gameobject!\n", type);
	}

	return component;
}
