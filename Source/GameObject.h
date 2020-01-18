#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Globals.h"
#include "Mesh.h"
#include "ModuleModelLoader.h"
#include <string>
#include <vector>

enum class Type { TRANSFORM, MESH, MATERIAL, CAMERA };

class Component;
class ComponentTransform;
class ComponentMesh;
class ComponentMaterial;
class ComponentCamera;

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
	void CreateComponent(Type type);
	void DrawComponents();
	void DeleteChild(GameObject *child);
	void GameObject::DeleteChildFlag(GameObject *child);
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
	ComponentTransform* transform = nullptr;
	ComponentMesh* mesh = nullptr;
	ComponentMaterial* material = nullptr;
	ComponentCamera* camera = nullptr;
	bool isCamera = false;
	//std::vector<Mesh> meshes;
	float distanceFromCamera = 0;
	Mesh* model = nullptr;
	std::string modelPath;
	bool deleteFlag = false;

private:
	void SetChildrenActive(std::vector<GameObject*>& objects, bool active);
};

#endif // __GAMEOBJECT_H__