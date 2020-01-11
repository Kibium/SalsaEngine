#include "GameObject.h"
#include "Globals.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "imgui.h"
#include "pcg/pcg_basic.h"

GameObject::GameObject() {
	CreateComponent(Type::TRANSFORM);
	UUID = pcg32_random();
}

GameObject::GameObject(const std::string &name) : name(name) {
	CreateComponent(Type::TRANSFORM);
	UUID = pcg32_random();
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
	update_status ret = UPDATE_CONTINUE;

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it) {
		if ((*it)->active)
			ret = (*it)->Update();
	}

	return ret;
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
		component->myGo = this;
		break;

	case Type::MESH:
		component = new ComponentMesh();
		component->myGo = this;
		break;

	case Type::MATERIAL:
		component = new ComponentMaterial();
		component->myGo = this;
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
	//ImGui::Text("UUID: %d", UUID);
	if (ImGui::Checkbox("Active##ObjectActive", &isActive)) {
		if (isActive) {
			for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it) {
				(*it)->active = true;
			}
			if (model != nullptr) model->isActive = true;
		}
		else {
			for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it) {
				(*it)->active = false;
			}
			if (model != nullptr) model->isActive = false;
		}
	}
	ImGui::SameLine();
	ImGui::InputText("##ObjectName", &name[0], name.size());
	ImGui::SameLine();
	ImGui::Checkbox("Static", &isStatic);
	for (int i = 0; i < components.size(); ++i) {
		components[i]->OnEditor();
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Add Component")) {
		ImGui::OpenPopup("Add Component Popup");
	}
	if (ImGui::BeginPopup("Add Component Popup")) {
		if (ImGui::MenuItem("Transform")) {
			if (GetComponent(Type::TRANSFORM) == nullptr) {
				CreateComponent(Type::TRANSFORM);
			}
		}
		if (ImGui::MenuItem("Mesh")) {
			if (GetComponent(Type::MESH) == nullptr) {
				CreateComponent(Type::MESH);
			}
		}
		if (ImGui::MenuItem("Material")) {
			if (GetComponent(Type::MATERIAL) == nullptr) {
				CreateComponent(Type::MATERIAL);
			}
		}
		ImGui::EndPopup();
	}
}

void GameObject::DeleteChild(GameObject *child) {
	assert(child != nullptr);

	for (int i = 0; i < children.size(); ++i) {
		if (child == children[i]) {
			children.erase(children.begin() + i);
			break;
		}
	}
}

void GameObject::DeleteComponent(Type type) {
	for (int i = 0; i < components.size(); ++i) {
		if (type == components[i]->type) {
			components.erase(components.begin() + i);
			break;
		}
	}
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
