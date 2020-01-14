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
	GameObject(uint32_t UID, uint32_t ParentUID, const std::string &name, bool Active, bool Static, const char *modelFile = nullptr);
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
	void DeleteComponent(Type type);

	GameObject* GetChild(unsigned childIndex) const;
	GameObject* GetChild(const std::string &childName) const;
	Component* GetComponent(Type type) const;
	Component* GetComponentInParent(Type type) const;
	Component* GetComponentInChild(unsigned childIndex, Type type) const;
	Component* GetComponentInChild(const std::string &childName, Type type) const;

public:
	uint32_t UUID;
	uint32_t parentUUID;
	bool isActive = true;
	bool isStatic = false;
	std::string name = "GameObject";
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;
	std::vector<Mesh> meshes;
	Model* model = nullptr;
	std::string modelPath;
};

#endif // __GAMEOBJECT_H__