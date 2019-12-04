#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <string>
#include <vector>

enum Type { TRANSFORM, MESH, MATERIAL };

class Component;

class GameObject {
public:
	GameObject();
	~GameObject();

	void Update();
	Component* CreateComponent(Type type);

public:
	bool active;
	std::string name;
	std::vector<Component*> components;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
};

#endif // __GAMEOBJECT_H__