#include "GameObject.h"
#include "Component.h"
#include "Globals.h"

GameObject::GameObject() {
}

GameObject::~GameObject() {
	for each(Component* comp in components)
		delete comp;
	components.clear();
}

void GameObject::Update() {
	for each(Component* comp in components)
		comp->Update();
}

Component* GameObject::CreateComponent(Type type) {
	Component* component = nullptr;

	switch (type) {
	case TRANSFORM:
		//component = new ComponentTransform();
		break;

	case MESH:
		//component = new ComponentMesh();
		break;

	case MATERIAL:
		//component = new ComponentMaterial();
		break;

	default:
		LOG("Incorrect component type!\n");
		break;
	}

	if (component != nullptr)
		components.push_back(component);

	return component;
}
