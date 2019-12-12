#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Globals.h"
#include <string>
#include <vector>

enum class Type { TRANSFORM, MESH, MATERIAL };

class Component;
class ComponentTransform;
class ComponentMesh;
class ComponentMaterial;

class GameObject {
public:
	GameObject();
	GameObject(const std::string &name);
	~GameObject();

	virtual bool Init();
	virtual update_status PreUpdate();
	virtual update_status Update();
	virtual update_status PostUpdate();
	virtual bool CleanUp();

public:
	Component* CreateComponent(Type type);
	void DrawComponents();
	void DeleteChild(GameObject *child);

	GameObject* GetChild(unsigned childIndex) const;
	GameObject* GetChild(const std::string &childName) const;
	Component* GetComponent(Type type) const;
	Component* GetComponentInParent(Type type) const;
	Component* GetComponentInChild(unsigned childIndex, Type type) const;
	Component* GetComponentInChild(const std::string &childName, Type type) const;

public:
	bool isActive = true;
	bool isStatic = false;
	std::string name = "GameObject";
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;
};

#endif // __GAMEOBJECT_H__