#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Globals.h"
#include "Mesh.h"
#include "ModuleModelLoader.h"
#include <string>
#include <vector>

enum class Type { TRANSFORM, MESH, MATERIAL };

class Component;
class ComponentTransform;
class ComponentMesh;
class ComponentMaterial;

typedef unsigned int uint32_t;

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
	void CreateComponent(Type type);
	void DrawComponents();
	void DeleteChild(GameObject *child);
	void DeleteComponent(Type type);
	void DrawGO();

	GameObject* GetChild(unsigned childIndex) const;
	GameObject* GetChild(const std::string &childName) const;
	Component* GetComponent(Type type) const;
	Component* GetComponentInParent(Type type) const;
	Component* GetComponentInChild(unsigned childIndex, Type type) const;
	Component* GetComponentInChild(const std::string &childName, Type type) const;


public:
	uint32_t UUID;
	bool isActive = true;
	bool isStatic = false;
	std::string name = "GameObject";
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;
	ComponentTransform* transform = nullptr;
	ComponentMesh* mesh = nullptr;
	ComponentMaterial* material = nullptr;
	std::vector<Mesh> meshes;
	Model* model = nullptr;
	float distanceFromCamera = 0;
};

#endif // __GAMEOBJECT_H__